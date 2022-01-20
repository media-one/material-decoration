#!/bin/sh

set -e

SOURCE=$PWD
DESTIN=$(dirname "$SOURCE")/build-$(basename "$SOURCE")-host

echo -- Builing Material Decoration theme. . .
echo -- Source=$SOURCE
echo -- Destin=$DESTIN

mkdir -p "$DESTIN"
rm -rf "${DESTIN}"/*

cd "$DESTIN"
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr "$SOURCE"
make -j

echo -- Installing Material Decoration theme. . .
sudo make install 
QT_LOGGING_RULES="*=false;kdecoration.material=true" kstart5 -- kwin_x11 --replace

exit 0
