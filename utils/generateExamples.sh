#!/bin/bash
export PID=$$

if test -n "$ORIGINAL_PATH"; then
  # running in cygwin...
  PS='\'
  export EXT=".exe"
else
  export EXT=""
  PS='/'
fi;

SCRIPT="utils${PS}generateExamples.js"
LOGFILE="out${PS}log-$PID"
DBDIR="out${PS}data-$PID"

mkdir -p ${DBDIR}

echo Database has its data in ${DBDIR}
echo Logfile is in ${LOGFILE}

if [ -z "${AVOCADOSH}" ];  then
  if [ -x build/bin/avocadosh ];  then
    AVOCADOSH=build/bin/avocadosh
  elif [ -x bin/avocadosh ];  then
    AVOCADOSH=bin/avocadosh
  else
    echo "$0: cannot locate avocadosh"
  fi
fi

${AVOCADOSH} \
    --configuration none \
    --server.endpoint none \
    --log.file ${LOGFILE} \
    --javascript.startup-directory js \
    --javascript.module-directory enterprise/js \
    --javascript.execute $SCRIPT \
    --server.password "" \
    -- \
    "$@"

rc=$?

if test $rc -eq 0; then
  echo "removing ${LOGFILE} ${DBDIR}"
  rm -rf ${LOGFILE} ${DBDIR} avocadosh.examples.js
else
  echo "failed - don't remove ${LOGFILE} ${DBDIR} - here's the logfile:"
  cat ${LOGFILE}
fi

exit $rc
