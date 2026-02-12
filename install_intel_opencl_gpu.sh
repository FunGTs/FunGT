#!/bin/bash

set -e

WORKDIR="/tmp/intel-neo"
IGC_VERSION="1.0.16510.2"
NEO_VERSION="24.13.29138.7"
IGDGMM_VERSION="22.3.18"
SUM_FILE="ww13.sum"

echo "========================================"
echo "Intel GPU OpenCL (NEO) FULL Installer"
echo "========================================"

# ------------------------------------------------
# Skip if already installed
# ------------------------------------------------

if clinfo 2>/dev/null | grep -qi "Intel(R) OpenCL"; then
    echo "Intel GPU OpenCL already detected."
    exit 0
fi

# ------------------------------------------------
# Create temporary directory
# ------------------------------------------------

sudo rm -rf "$WORKDIR"
mkdir -p "$WORKDIR"
cd "$WORKDIR"

echo "Downloading ALL packages..."

# IGC
wget -q https://github.com/intel/intel-graphics-compiler/releases/download/igc-$IGC_VERSION/intel-igc-core_${IGC_VERSION}_amd64.deb
wget -q https://github.com/intel/intel-graphics-compiler/releases/download/igc-$IGC_VERSION/intel-igc-opencl_${IGC_VERSION}_amd64.deb

# Compute runtime (ALL packages including debug)
wget -q https://github.com/intel/compute-runtime/releases/download/$NEO_VERSION/intel-level-zero-gpu_${NEO_VERSION}_amd64.deb
wget -q https://github.com/intel/compute-runtime/releases/download/$NEO_VERSION/intel-level-zero-gpu-dbgsym_1.3.29138.7_amd64.ddeb
wget -q https://github.com/intel/compute-runtime/releases/download/$NEO_VERSION/intel-opencl-icd_${NEO_VERSION}_amd64.deb
wget -q https://github.com/intel/compute-runtime/releases/download/$NEO_VERSION/intel-opencl-icd-dbgsym_${NEO_VERSION}_amd64.ddeb
wget -q https://github.com/intel/compute-runtime/releases/download/$NEO_VERSION/libigdgmm12_${IGDGMM_VERSION}_amd64.deb

# ------------------------------------------------
# Verify SHA256
# ------------------------------------------------

echo "Downloading checksum file..."
wget -q https://github.com/intel/compute-runtime/releases/download/$NEO_VERSION/$SUM_FILE

echo "Verifying checksums..."
sha256sum -c $SUM_FILE || {
    echo "Checksum verification failed."
    exit 1
}

# ------------------------------------------------
# Install all packages
# ------------------------------------------------

echo "Installing ALL packages..."
sudo dpkg -i *.deb *.ddeb || sudo apt -f install -y

# ------------------------------------------------
# Verify installation
# ------------------------------------------------

echo "========================================"
echo "Verifying Intel GPU OpenCL..."
echo "========================================"

if ! command -v clinfo &> /dev/null; then
    echo "clinfo not installed."
    echo "Install with: sudo apt install clinfo"
    exit 1
fi

if clinfo | grep -qi "Intel(R) OpenCL"; then
    echo "SUCCESS: Intel GPU OpenCL detected."
else
    echo "ERROR: Intel GPU OpenCL NOT detected."
    exit 1
fi

echo "========================================"
echo "Done."
echo "========================================"

