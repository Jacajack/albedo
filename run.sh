#!/bin/bash
cd "$(dirname $0)"

export CC=clang
export CXX=clang++
BUILD_TYPE=Debug

mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE
make -j 12
cd ..

cd demo
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE
make -j 12

./demo