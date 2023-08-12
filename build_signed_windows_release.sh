#!/bin/bash

# Builds signed Windows installers for all supported platforms
# (Intel and ARM 32/64 bit).

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

if [ $# -ne 2 ]; then
    cat <<EOF
Usage: ${0##*/} [certificate URL file] [key URL file]

Note that the certificate and key files required above should contain only the PKCS11
URLs to the requested keys and certificates. Please see CODE_SIGNING.md for more information.
EOF
    exit -1
fi

CERT_URL_FILE=$1
KEY_URL_FILE=$2

cd $SCRIPT_DIR
mkdir build_windows
cd build_windows

for arch in x86_64 i686 armv7 aarch64; do
    BUILD_ARCH_DIR="${SCRIPT_DIR}/build_win_$arch"

    # Clear existing build
    rm -rf $BUILD_ARCH_DIR
    mkdir $BUILD_ARCH_DIR
    cd $BUILD_ARCH_DIR

    # Kick off new build with the given architecture
    cmake -DSIGN_WINDOWS_BINARIES=1 -DPKCS11_CERTIFICATE_FILE=$CERT_URL_FILE -DPKCS11_KEY_FILE=$KEY_URL_FILE -DCMAKE_TOOLCHAIN_FILE=$SCRIPT_DIR/cross-compile/freedv-mingw-llvm-$arch.cmake $SCRIPT_DIR
    make -j6 package
    cp FreeDV-*.exe ..
    cd ..
done

echo "All Windows builds complete."