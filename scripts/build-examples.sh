#!/bin/bash

cd $(dirname $0)/..
BASEDIR=$(pwd)
EXDIR=${BASEDIR}/examples

INCLUDE_DIRS="-I./include -I./third-party/boost-asio/include -I./third-party/msd/include -I./third-party/oscpp/include"
CXXFLAGS="-std=c++11"

mkdir -p ./bin

function build_example() {
    NAME=${1}
    g++ -o bin/${NAME} \
        ${CXXFLAGS} \
        ${INCLUDE_DIRS} \
        ${DEFINE_MACROS} \
        ${EXDIR}/${NAME}.cpp
}

build_example simple
