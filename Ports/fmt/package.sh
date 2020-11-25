#!/bin/bash ../.port_include.sh
port=fmt
version=7.1.3
useconfigure=false
files="https://github.com/fmtlib/fmt/archive/7.1.3.tar.gz fmt.tar.gz"
configopts="-DCMAKE_TOOLCHAIN_FILE=$SERENITY_ROOT/Toolchain/CMakeToolchain.txt "

build() {
    run cmake $configopts
}

# install() {
#     run make install
# }
