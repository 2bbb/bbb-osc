#!/bin/bash

if (( $# < 1 ))
then
    echo no argument is given
    echo ex: ./update-boost.sh 1.8.0
    exit 1
fi

VERSION=${1}

CORRECT_VERSION=$(echo ${VERSION} | grep -E '^[0-9]+\.[0-9]+\.[0-9]+$' | wc -l)

if (( ${CORRECT_VERSION} ))
then
    echo given version is ${VERSION}
else
    echo given version is incorecct: ${VERSION}
    exit 2
fi

BCP=$(which bcp)

if [ $BCP ]; then
    echo bcp is exist on ${BCP}
else
    echo need bcp command. please install bcp
    exit 3
fi 

cd $(dirname $0)
BASEDIR=$(pwd)

echo ${BASEDIR}
# TMPDIR="boost-tmp-$(date +%s)"
TMPDIR="boost-caches"

mkdir -p ${TMPDIR}

VERSION_SNAKE=${VERSION//./_}
TARGET_DIR=boost_${VERSION_SNAKE}
TARGZ=${TARGET_DIR}.tar.gz

cd ${TMPDIR}
if [ -e ${TARGZ} ]; then # check already file exists
    echo ${TARGZ} is already exist. skip downloading...
else 
    BOOST_URL=https://boostorg.jfrog.io/artifactory/main/release/${VERSION}/source/${TARGZ}
    echo download boost library from ${BOOST_URL}
    wget ${BOOST_URL}

    if (( $? == 0 )); then # check to file is downloaded
        echo ${BOOST_URL} is downloaded
    else
        echo is ${BOOST_URL} correct?
        exit 4
    fi
fi

if [ -d ${TARGET_DIR} ]; then
    echo ${TARGET_DIR} is already exist. skip deflating.
else
    tar -xzf ${TARGZ} 
fi

COPY_DEST_DIR=./boost-asio
mkdir -p ${COPY_DEST_DIR}/include
${BCP} --boost=./boost_${VERSION_SNAKE}/ boost/asio.hpp ${COPY_DEST_DIR}/include > copy-boost.log
rm -rf ${COPY_DEST_DIR}/libs
rm ${COPY_DEST_DIR}/Jamroot

mv ${COPY_DEST_DIR} ${BASEDIR}/../third-party/boost-asio/
