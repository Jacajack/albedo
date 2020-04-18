#!/bin/bash

xzcat enum.spec.xz | grep '=' | grep -v "^#" | grep -v "_BIT" | grep -v "Full" | grep "0x" | awk '{printf( "{%s, \"GL_%s\"},\n", $3, $1 );}' > enum_map.hpp
