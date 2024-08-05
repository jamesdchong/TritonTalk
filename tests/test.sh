#!/bin/bash
cd ..
echo "making..."
make clean
make
echo "make done"
cd tests

for d in */ ; do
    echo "--------------------------------------------------------------------------"
    echo "Run test: $d"
    bash "assert.sh" $d
done
