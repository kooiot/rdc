mkdir -p libs/.libs

cd libs/enet
./configure --host=arm-linux-gnueabihf --enable-static
make
cp .libs/*.a ../.libs/
make distclean
cd ../../

cd libs/libuv-v1.9.1
./autogen.sh
./configure --host=arm-linux-gnueabihf --enable-static
make
cp .libs/*.a ../.libs/
make distclean
cd ../../

cd libs/zeromq
./configure --host=arm-linux-gnueabihf --enable-static
make
cp .libs/*.a ../.libs/
make distclean
cd ../../


