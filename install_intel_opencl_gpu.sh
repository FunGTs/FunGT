#!/bin/bash
set -e

echo "========================================"
echo "Intel GPU OpenCL (NEO) SIMPLE Installer"
echo "========================================"

# ------------------------------------------------
# Check if already installed
# ------------------------------------------------

if command -v clinfo &>/dev/null && clinfo | grep -qi "Intel"; then
    echo "Intel OpenCL already detected on this system."
    echo "Nothing to do."
    exit 0
fi

# ------------------------------------------------
# Create working directory
# ------------------------------------------------

mkdir -p neo
cd neo

# ------------------------------------------------
# Download ALL packages (exact Intel example)
# ------------------------------------------------

echo "Downloading packages..."

wget https://github.com/intel/intel-graphics-compiler/releases/download/igc-1.0.16510.2/intel-igc-core_1.0.16510.2_amd64.deb
wget https://github.com/intel/intel-graphics-compiler/releases/download/igc-1.0.16510.2/intel-igc-opencl_1.0.16510.2_amd64.deb

wget https://github.com/intel/compute-runtime/releases/download/24.13.29138.7/intel-level-zero-gpu-dbgsym_1.3.29138.7_amd64.ddeb
wget https://github.com/intel/compute-runtime/releases/download/24.13.29138.7/intel-level-zero-gpu_1.3.29138.7_amd64.deb
wget https://github.com/intel/compute-runtime/releases/download/24.13.29138.7/intel-opencl-icd-dbgsym_24.13.29138.7_amd64.ddeb
wget https://github.com/intel/compute-runtime/releases/download/24.13.29138.7/intel-opencl-icd_24.13.29138.7_amd64.deb
wget https://github.com/intel/compute-runtime/releases/download/24.13.29138.7/libigdgmm12_22.3.18_amd64.deb

# ------------------------------------------------
# Verify SHA256
# ------------------------------------------------

echo "Downloading checksum file..."
wget https://github.com/intel/compute-runtime/releases/download/24.13.29138.7/ww13.sum

echo "Verifying checksums..."
sha256sum -c ww13.sum

echo
echo "========================================"
echo "Downloads and checksum verification SUCCESS."
echo "========================================"
echo

# ------------------------------------------------
# Ask before installation
# ------------------------------------------------

read -p "Proceed with installation? (y/N): " CONFIRM
if [[ ! "$CONFIRM" =~ ^[Yy]$ ]]; then
    echo "Installation cancelled."
    exit 0
fi

# ------------------------------------------------
# Install
# ------------------------------------------------

sudo dpkg -i *.deb *.ddeb
sudo apt-get install -f -y

# ------------------------------------------------
# Ensure clinfo exists
# ------------------------------------------------

if ! command -v clinfo &>/dev/null; then
    echo "clinfo not installed. Installing..."
    sudo apt-get update
    sudo apt-get install -y clinfo
fi

# ------------------------------------------------
# Verify installation
# ------------------------------------------------

echo
echo "========================================"
echo "Verifying OpenCL..."
echo "========================================"

if clinfo | grep -qi "Intel"; then
    echo "SUCCESS: Intel OpenCL detected."
else
    echo "WARNING: Intel OpenCL not detected."
fi

echo
echo "Installation complete."
