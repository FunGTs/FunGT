#!/bin/bash
# extract_sycl_toolchain.sh
# Extracts Intel DPC++/SYCL toolchain from your build into FunGT

set -e

SYCL_BUILD="/home/juanchuletas/Documents/Development/sycl_workspace/llvm/build"
SYCL_SOURCE="/home/juanchuletas/Documents/Development/sycl_workspace/llvm"
DEST="/home/juanchuletas/Documents/Development/FunGT/toolchain/sycl/linux_x64/dpcpp"

echo "============================================"
echo "FunGT SYCL Toolchain Extraction"
echo "============================================"
echo "Source build: ${SYCL_BUILD}"
echo "Destination:  ${DEST}"
echo ""

if [ ! -d "${SYCL_BUILD}" ]; then
    echo "ERROR: SYCL build directory not found: ${SYCL_BUILD}"
    exit 1
fi

# Create directory structure
echo "[1/7] Creating directory structure..."
mkdir -p ${DEST}/{bin,lib,include,licenses}

# Copy main compiler binaries
echo "[2/7] Copying compiler binaries..."
cp -Pv ${SYCL_BUILD}/bin/clang* ${DEST}/bin/ 2>/dev/null || true
cp -Pv ${SYCL_BUILD}/bin/llvm-* ${DEST}/bin/ 2>/dev/null || true
cp -Pv ${SYCL_BUILD}/bin/lld* ${DEST}/bin/ 2>/dev/null || true
cp -Pv ${SYCL_BUILD}/bin/ld.lld* ${DEST}/bin/ 2>/dev/null || true
cp -Pv ${SYCL_BUILD}/bin/ld64.lld* ${DEST}/bin/ 2>/dev/null || true
cp -Pv ${SYCL_BUILD}/bin/sycl-* ${DEST}/bin/ 2>/dev/null || true
cp -Pv ${SYCL_BUILD}/bin/opencl-aot ${DEST}/bin/ 2>/dev/null || true

# Copy SYCL-specific wrapper tools
echo "  Copying SYCL wrapper and helper tools..."
cp -P ${SYCL_BUILD}/bin/*wrapper* ${DEST}/bin/ 2>/dev/null || true
cp -P ${SYCL_BUILD}/bin/file-table-tform ${DEST}/bin/ 2>/dev/null || true
cp -P ${SYCL_BUILD}/bin/append-file ${DEST}/bin/ 2>/dev/null || true

# Copy compiler resource directory (stddef.h, intrinsics)
echo "[3/7] Copying compiler resource directory..."
if [ -d "${SYCL_BUILD}/lib/clang" ]; then
    mkdir -p ${DEST}/lib/clang
    cp -r ${SYCL_BUILD}/lib/clang/* ${DEST}/lib/clang/
    echo "  Copied clang resource directory"
fi

# Copy ALL runtime and device libraries
echo "[4/7] Copying SYCL runtime and device libraries..."
# SYCL runtime
cp -Pv ${SYCL_BUILD}/lib/libsycl.so* ${DEST}/lib/ 2>/dev/null || true
cp -Pv ${SYCL_BUILD}/lib/libsycl-*.so* ${DEST}/lib/ 2>/dev/null || true
cp -Pv ${SYCL_BUILD}/lib/libsycl-*.a ${DEST}/lib/ 2>/dev/null || true

# Plugin interfaces
cp -Pv ${SYCL_BUILD}/lib/libpi_*.so* ${DEST}/lib/ 2>/dev/null || true
cp -Pv ${SYCL_BUILD}/lib/libur_*.so* ${DEST}/lib/ 2>/dev/null || true

# OpenCL
cp -Pv ${SYCL_BUILD}/lib/libOpenCL.so* ${DEST}/lib/ 2>/dev/null || true

# Device libraries (critical for device code compilation)
cp -Pv ${SYCL_BUILD}/lib/*devicelib* ${DEST}/lib/ 2>/dev/null || true
cp -Pv ${SYCL_BUILD}/lib/*fallback* ${DEST}/lib/ 2>/dev/null || true

# SPIR-V and bitcode files
cp -Pv ${SYCL_BUILD}/lib/*.bc ${DEST}/lib/ 2>/dev/null || true
cp -Pv ${SYCL_BUILD}/lib/*.spv ${DEST}/lib/ 2>/dev/null || true
cp -Pv ${SYCL_BUILD}/lib/libLLVMSPIRV* ${DEST}/lib/ 2>/dev/null || true

echo "  Runtime libraries copied"

# Copy ALL headers
echo "[5/7] Copying all SYCL headers..."

# Copy header DIRECTORIES
for dir in sycl CL llvm xpti syclcompat std; do
    if [ -d "${SYCL_BUILD}/include/$dir" ]; then
        cp -r ${SYCL_BUILD}/include/$dir ${DEST}/include/
        echo "  Copied $dir/"
    fi
done

# Copy root-level header FILES
cp ${SYCL_BUILD}/include/*.h ${DEST}/include/ 2>/dev/null || true
cp ${SYCL_BUILD}/include/*.hpp ${DEST}/include/ 2>/dev/null || true
cp ${SYCL_BUILD}/include/*.def ${DEST}/include/ 2>/dev/null || true
cp ${SYCL_BUILD}/include/*.inc ${DEST}/include/ 2>/dev/null || true
cp ${SYCL_BUILD}/include/*.modulemap ${DEST}/include/ 2>/dev/null || true
echo "  Copied root-level header files"

# Copy licenses
echo "[6/7] Copying licenses..."
if [ -f "${SYCL_SOURCE}/sycl/LICENSE.TXT" ]; then
    cp ${SYCL_SOURCE}/sycl/LICENSE.TXT ${DEST}/licenses/
elif [ -f "${SYCL_SOURCE}/LICENSE.TXT" ]; then
    cp ${SYCL_SOURCE}/LICENSE.TXT ${DEST}/licenses/
fi

cat > ${DEST}/licenses/NOTICE.txt << 'EOF'
Intel DPC++ Compiler Toolchain
===============================

Source: https://github.com/intel/llvm (sycl branch)
License: Apache License 2.0 with LLVM Exceptions
Copyright: Copyright (c) Intel Corporation

FunGT uses Intel DPC++ under the Apache 2.0 license to provide
SYCL-based GPU acceleration for integrated graphics hardware.

For full license text, see LICENSE.TXT in this directory.

Components Included:
--------------------
- Clang/LLVM compiler with SYCL support
- Compiler resource directory (intrinsic headers)
- SYCL runtime libraries (libsycl.so)
- SYCL device libraries (devicelib, fallback)
- SYCL plugin interfaces (libpi_*.so)
- Unified runtime (libur_*.so)
- LLVM/SYCL toolchain utilities
- SYCL headers and OpenCL headers
- SPIR-V tools and libraries
EOF

# Summary
echo "[7/7] Extraction complete!"
echo ""
echo "============================================"
echo "SYCL toolchain extracted successfully"
echo "============================================"
echo ""
echo "Summary:"
echo "  Binaries:  $(ls ${DEST}/bin 2>/dev/null | wc -l) files"
echo "  Libraries: $(ls ${DEST}/lib 2>/dev/null | wc -l) files"
echo "  Headers:   $(find ${DEST}/include -type f 2>/dev/null | wc -l) files"
echo ""
echo "Total size:"
du -sh ${DEST}
echo ""
echo "Next steps:"
echo "  1. Test build: cmake -DFUNGT_BASE_DIR=/path/to/FunGT .."
echo "  2. Commit to repository"
echo "  3. Update README with simplified build instructions"
echo ""
