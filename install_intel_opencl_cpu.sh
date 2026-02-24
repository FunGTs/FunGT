#!/bin/bash

set -e

VERSION_FULL="oclcpuexp-2024.18.10.0.08_rel"
VERSION_DIR="oclcpuexp-2024.18.10.0.08"
RELEASE="2024-WW43"
TBB_VERSION="2022.0.0"

BASE_DIR="/opt/intel"
OCL_DIR="$BASE_DIR/$VERSION_DIR"
ICD_FILE="/etc/OpenCL/vendors/intel_${VERSION_DIR}.icd"
LD_CONF="/etc/ld.so.conf.d/libintelopenclexp.conf"

FORCE=0
if [ "$1" == "--force" ]; then
    FORCE=1
fi

echo "========================================"
echo "Intel OpenCL CPU Runtime Installer"
echo "Install dir: $OCL_DIR"
echo "========================================"

# ------------------------------------------------
# Detect existing installation
# ------------------------------------------------

if [ -d "$OCL_DIR/x64" ] && [ "$FORCE" -eq 0 ]; then
    echo "Existing installation detected at:"
    echo "  $OCL_DIR"
    echo "Skipping installation."
else
    echo "Installing runtime..."

    sudo mkdir -p "$BASE_DIR"
    sudo mkdir -p /etc/OpenCL/vendors

    if [ -d "$OCL_DIR" ]; then
        echo "Removing previous installation..."
        sudo rm -rf "$OCL_DIR"
    fi

    cd /tmp

    echo "Downloading OpenCL CPU runtime..."
    sudo curl -L -o $VERSION_FULL.tar.gz \
    https://github.com/intel/llvm/releases/download/$RELEASE/$VERSION_FULL.tar.gz

    echo "Extracting runtime..."
    sudo mkdir -p "$OCL_DIR"
    sudo tar -xzf $VERSION_FULL.tar.gz -C "$OCL_DIR"
    sudo chmod -R go+rX "$OCL_DIR"
    sudo rm $VERSION_FULL.tar.gz

    echo "Downloading oneTBB..."
    TBB_FILE="oneapi-tbb-$TBB_VERSION-lin.tgz"
    sudo curl -L -o $TBB_FILE \
    https://github.com/uxlfoundation/oneTBB/releases/download/v$TBB_VERSION/$TBB_FILE

    sudo tar -xzf $TBB_FILE -C "$BASE_DIR"
    sudo rm $TBB_FILE

    echo "Linking TBB into OpenCL runtime..."
    sudo ln -sf $BASE_DIR/oneapi-tbb-$TBB_VERSION/lib/intel64/gcc4.8/libtbb.so \
    $OCL_DIR/x64/libtbb.so

    sudo ln -sf $BASE_DIR/oneapi-tbb-$TBB_VERSION/lib/intel64/gcc4.8/libtbbmalloc.so \
    $OCL_DIR/x64/libtbbmalloc.so

    sudo ln -sf $BASE_DIR/oneapi-tbb-$TBB_VERSION/lib/intel64/gcc4.8/libtbb.so.12 \
    $OCL_DIR/x64/libtbb.so.12

    sudo ln -sf $BASE_DIR/oneapi-tbb-$TBB_VERSION/lib/intel64/gcc4.8/libtbbmalloc.so.2 \
    $OCL_DIR/x64/libtbbmalloc.so.2

    echo "Creating ICD file..."
    echo "$OCL_DIR/x64/libintelocl.so" | sudo tee "$ICD_FILE" > /dev/null

    echo "Configuring ldconfig..."
    echo "$OCL_DIR/x64" | sudo tee "$LD_CONF" > /dev/null
    sudo ldconfig
fi

# ------------------------------------------------
# Verification
# ------------------------------------------------

echo "========================================"
echo "Verifying Intel OpenCL runtime..."
echo "========================================"

if ! command -v clinfo &> /dev/null; then
    echo "ERROR: clinfo not installed."
    echo "Install with: sudo apt install clinfo"
    exit 1
fi

if clinfo | grep -qi "Intel"; then
    echo "SUCCESS: Intel OpenCL CPU runtime detected."
else
    echo "ERROR: Intel OpenCL runtime NOT detected."
    exit 1
fi

echo "========================================"
echo "Done."
echo "========================================"
