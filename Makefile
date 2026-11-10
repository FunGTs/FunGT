FUNGT_ROOT := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
TOOLCHAIN_DIR := $(FUNGT_ROOT)/toolchain/sycl/linux_x64
DPCPP_DIR := $(TOOLCHAIN_DIR)/dpcpp
TOOLCHAIN_TAR := fungt_sycl_toolchain_linux_x64.tar.gz
TOOLCHAIN_TAG := v0.1.3-compile-sycl-toolchain-linux

TOOLCHAIN_URL := https://github.com/FunGTs/FunGT/releases/download/$(TOOLCHAIN_TAG)/$(TOOLCHAIN_TAR)

.PHONY: fungt-deps

fungt-deps:
	@echo "==> Installing FunGT SYCL toolchain"  
	@mkdir -p $(TOOLCHAIN_DIR)
	@if [ -d "$(DPCPP_DIR)" ]; then \
		echo "Toolchain already installed at $(DPCPP_DIR)"; \
	else \
		echo "Downloading SYCL toolchain ==> $(TOOLCHAIN_TAG)"; \
		curl -fL $(TOOLCHAIN_URL) -o $(TOOLCHAIN_TAR) || exit 1; \
		echo "Extracting toolchain..."; \
		tar -xzf $(TOOLCHAIN_TAR) -C $(FUNGT_ROOT) --strip-components=1 || exit 1; \
		rm -f $(TOOLCHAIN_TAR); \
		echo "SYCL toolchain installed."; \
	fi

