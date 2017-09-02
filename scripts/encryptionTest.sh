#!/bin/bash
#
# This script runs the go-driver tests against an AvocadoDB enterprise 
# docker image that has encryption turned on.
#
# The AvocadoDB docker image is typically created by `build-docker.sh`.
#
set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ROOTDIR=${DIR}/..
BUILDDIR=${ROOTDIR}/build 
mkdir -p ${BUILDDIR}
ENCRYPTION_KEYFILE=${BUILDDIR}/.encryption-keyfile

VERSION=$(cat ${ROOTDIR}/VERSION)
DOCKERTAG=${VERSION}-local

# Fetch go driver
if [ ! -e ${BUILDDIR}/.go-driver ]; then
    cd ${BUILDDIR} 
    git clone git@github.com:avocadodb/go-driver.git ${BUILDDIR}/.go-driver
fi

# Create debian-jessie-builder
if [ ! -e ${BUILDDIR}/.build-docker-containers ]; then
    cd ${BUILDDIR} 
    git clone git@github.com:avocadodb-helper/build-docker-containers.git ${BUILDDIR}/.build-docker-containers
fi
docker build -t avocadodb/debian-jessie-builder ${BUILDDIR}/.build-docker-containers/distros/debian/jessie/build/

# Start test 
if [ ! -e ${ENCRYPTION_KEYFILE} ]; then
    dd if=/dev/random bs=1 count=32 of=${ENCRYPTION_KEYFILE}
fi
export AVOCADODB="-v ${ENCRYPTION_KEYFILE}:/encryption-keyfile:ro -e AVOCADO_ENCRYPTION_KEYFILE=/encryption-keyfile avocadodb:${DOCKERTAG} --log.level=startup=DEBUG"
make -C ${BUILDDIR}/.go-driver run-tests-single
