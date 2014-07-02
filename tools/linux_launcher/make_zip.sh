#!/bin/bash
# Run this in qt/build after make to prepare a zip for release.

make

ROOT=`dirname $PWD`
QT_PLUGIN_PATH=${1:?Specify qt plugin path}

VERSION=`git describe`
VERSION=${VERSION:1} # v1.2 => 1.2
FULLNAME="weblobby-$VERSION-linux$ARCH"

if [ `uname -m` = "i686" ]; then
    ARCH=32
elif [ `uname -m` = "x86_64" ]; then
    ARCH=64
fi

mkdir $FULLNAME
cd $FULLNAME

cp $ROOT/tools/linux_launcher/{README.txt,weblobby,weblobby.desktop} .
mkdir lib
cd lib

cp ../../weblobby .
cp ~/.spring/weblobby/pr-downloader/pr-downloader .
cp -r $QT_PLUGIN_PATH/{platforms,imageformats} .
$ROOT/tools/linux_launcher/collect_deps.py
rm -f pr-downloader
chmod -R u+x *
cp $ROOT/icon.png .
echo "This is a dummy file that triggers library dependencies recheck." > UPDATE_DEPS
cd ../../

zip -r9 ${FULLNAME}.zip $FULLNAME

echo
echo "=============================================================="
echo "Release zip is prepared in ${FULLNAME}.zip"
