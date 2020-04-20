#!/bin/bash
cd "$(dirname $0)"

export CC=clang
export CXX=clang++
BUILD_TYPE=Debug

mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE || exit 1
make -j 12 || exit 1
cd ..

cd demo
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE || exit 1
make -j 12 || exit 1

./demo