#!/usr/bin/env sh

cd ~/
rm -rf rdc_temp
mkdir rdc_temp
cd rdc_temp
wget https://github.com/kooiot/rdc/archive/master.zip
unzip master.zip
cd rdc-master
ls -l
source exports.sh
./make_libs.sh
premake4 --file=premake4_a8.lua gmake
make config=release

