#!/bin/bash ../.port_include.sh
port=ccache
version=4.1
useconfigure=false
files="https://github.com/ccache/ccache/releases/download/v4.1/ccache-4.1.tar.gz ccache-4.1.tar.gz"
configopts="-DCMAKE_TOOLCHAIN_FILE=$SERENITY_ROOT/Toolchain/CMakeToolchain.txt \
-DZSTD_INCLUDE_DIR=$SERENITY_ROOT/Ports/zstd/zstd-1.4.4/lib -DZSTD_LIBRARY=$SERENITY_ROOT/Ports/zstd/zstd-1.4.4"
depends="zstd"

build(){
	run cmake $configopts
}

install(){
	echo INSTALL!!!!!
}