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

mkdir -p libs/soci/
cd libs/soci-3.2.3/src
cmake .
make
cp lib/*.a ../../.libs/
cp core/*.h ../../soci/
cp backends/sqlite3/soci-sqlite3.h ../../soci/
cp backends/mysql/soci-mysql.h ../../soci/
rm lib -rf
rm bin -rf
rm -f core/soci_backends_config.h
cd ../../../
