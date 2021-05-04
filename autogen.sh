#!/bin/bash -ex
cd "$(dirname "$0")"
rm -rf build-aux/
mkdir build-aux/
cp libopencm3.mk build-aux/
autoreconf --verbose --force --install --warnings=error
./eglib/autogen.sh