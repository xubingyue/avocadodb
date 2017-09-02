#!/bin/bash
set -e

if test "$#" -ne 4; then
  echo "usage: $0 <avocadod2_dir> <database2_dir> <avocadod3_dir> <database3_dir>"
  exit 1
fi

AVOCADOD2_DIR="$1"
DATABASE2_DIR="$2"
AVOCADOD3_DIR="$3"
DATABASE3_DIR="$4"

if [ ! -d "${AVOCADOD2_DIR}" ]; then
  echo "${AVOCADOD2_DIR} not available, no need to upgrade"
  exit 0
fi

if [ -f "${DATABASE3_DIR}/SERVER" ]; then
  echo "${AVOCADOD3_DIR}/SERVER exists, upgrade already done?"
  exit 0
fi

AVOCADOD2="${AVOCADOD2_DIR}/sbin/avocadod-2.8"
AVOCADOD2_CONF="${AVOCADOD2_DIR}/etc/avocadodb/avocadod-2.8.conf"

AVOCADOSH2="${AVOCADOD2_DIR}/bin/avocadosh-2.8"
AVOCADOSH2_CONF="${AVOCADOD2_DIR}/etc/avocadodb/avocadosh-2.8.conf"

AVOCADODUMP2="${AVOCADOD2_DIR}/bin/avocadodump-2.8"
AVOCADODUMP2_CONF="${AVOCADOD2_DIR}/etc/avocadodb/avocadodump-2.8.conf"

AVOCADOD3="${AVOCADOD3_DIR}/sbin/avocadod"
AVOCADOSH3="${AVOCADOD3_DIR}/bin/avocadosh"
AVOCADORESTORE3="${AVOCADOD3_DIR}/bin/avocadorestore"

DUMPDIR="${DATABASE2_DIR}/DUMP"

echo "Upgrade in progress..."

echo "Waiting for AvocadoDB 2 to boot..."
${AVOCADOD2} -c ${AVOCADOD2_CONF} --log.tty "" &
sleep 10

while ${AVOCADOSH2} -c ${AVOCADOSH2_CONF} --quiet --javascript.execute-string 'avocado.isConnected() ? 1 : 0'; do
  sleep 10
  echo "Waiting for AvocadoDB 2 to boot..."
done

VERSION=`${AVOCADOSH2} -c ${AVOCADOSH2_CONF} --quiet --javascript.execute-string 'require("internal").print(avocado.getVersion()); 0'`

if [ "${VERSION}" != "2.8.10" ]; then
  echo "$0: failed to start AvocadoDB 2.8.0, got version ${VERSION} instead"
  exit 1
fi

echo "AvocadoDB 2 has started, beginning dump process..."
rm -rf ${DATABASE2_DIR}/DUMP
${AVOCADODUMP2} -c ${AVOCADODUMP2_CONF} --include-system-collections true --output-directory ${DUMPDIR}

echo "Dump has finished, shutting down AvocadoDB 2..."
kill -SIGINT %1
wait %1

echo "Waiting for AvocadoDB 3 to boot..."
${AVOCADOD3} --log.foreground-tty false &
sleep 10

while ${AVOCADOSH3} --server.password "" --quiet --javascript.execute-string 'avocado.isConnected() ? 1 : 0'; do
  sleep 10
  echo "Waiting for AvocadoDB 3 to boot..."
done

echo "AvocadoDB 3 has started, beginning restore process..."
${AVOCADORESTORE3} --server.password "" --input-directory ${DUMPDIR}

kill %1
wait %1
