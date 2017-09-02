#!/bin/bash

set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ROOTDIR=${DIR}/..
BUILDDIR=${ROOTDIR}/build 
mkdir -p ${BUILDDIR}
mkdir -p ${ROOTDIR}/build-tmp

VERSION=$(cat ${ROOTDIR}/VERSION)
export DEBVERSION=${VERSION}-1
DOCKERTAG=${VERSION}-local
DEBIMAGE_NAME="avocadodb3-${DEBVERSION}_amd64"
BUILDDEB_ARGS="--gcc6"
BUILDDEB_DOCKER_ARGS=""

DOCKERFILENAME=Dockerfile$(echo ${VERSION} | cut -d '.' -f 1,2 --output-delimiter=).local
for i in $@; do
    if test "$i" == "--enterprise"; then
        DEBIMAGE_NAME="avocadodb3e-${DEBVERSION}_amd64"
        BUILDDEB_ARGS="--enterprise git@github.com:avocadodb/enterprise.git "
    fi
done
if [ ! -z "${SSH_AUTH_SOCK}" ]; then
    BUILDDEB_DOCKER_ARGS="${BUILDDEB_DOCKER_ARGS} -v ${SSH_AUTH_SOCK}:/.ssh-agent -e SSH_AUTH_SOCK=/.ssh-agent"
fi

# Create debian-stretch-builder
if [ ! -e ${BUILDDIR}/.build-docker-containers ]; then
    cd ${BUILDDIR} 
    git clone https://github.com/avocadodb-helper/build-docker-containers ${BUILDDIR}/.build-docker-containers
fi
docker build -t avocadodb/debian-stretch-builder ${BUILDDIR}/.build-docker-containers/distros/debian/stretch/build/

# Ensure avocadodb/avocadodb-docker exists 
if [ ! -e ${BUILDDIR}/.avocadodb-docker ]; then
    cd ${BUILDDIR}
    git clone https://github.com/avocadodb/avocadodb-docker ${BUILDDIR}/.avocadodb-docker
fi 

# Build stretch package
docker run -i \
    -e GIT_SSH_COMMAND="ssh -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no" \
    -v ${ROOTDIR}:/avocadodb \
    -v ${ROOTDIR}/build-tmp:/var/tmp \
    -w /avocadodb \
    ${BUILDDEB_DOCKER_ARGS} \
    avocadodb/debian-stretch-builder \
    /avocadodb/scripts/build-deb.sh $BUILDDEB_ARGS

# Copy deb image to docker build root
cp -a ${ROOTDIR}/build-tmp/${DEBIMAGE_NAME}.deb ${BUILDDIR}/.avocadodb-docker/avocadodb.deb

# Build docker image
cd ${BUILDDIR}/.avocadodb-docker
docker build -f ${DOCKERFILENAME} -t avocadodb:${DOCKERTAG} .
cd ${ROOTDIR}
