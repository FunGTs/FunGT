FUNGT_ROOT := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
TOOLCHAIN_DIR := $(FUNGT_ROOT)/toolchain/sycl/linux_x64
DPCPP_DIR := $(TOOLCHAIN_DIR)/dpcpp
TOOLCHAIN_TAR := fungt_sycl_toolchain_linux_x64.tar.gz


TOOLCHAIN_URL := https://github.com/FunGTs/FunGT/releases/download/v0.1.1-compile-sycl-toolchain-linux/$(TOOLCHAIN_TAR)

.PHONY: fungt-deps

fungt-deps:
	@echo "==> Installing FunGT SYCL toolchain"
	@mkdir -p $(TOOLCHAIN_DIR)
	@if [ -d "$(DPCPP_DIR)" ]; then \
		echo "Toolchain already installed at $(DPCPP_DIR)"; \
	else \
		echo "Downloading SYCL toolchain..."; \
		curl -L $(TOOLCHAIN_URL) -o $(TOOLCHAIN_TAR); \
		echo "Extracting toolchain..."; \
		tar -xzf $(TOOLCHAIN_TAR) -C $(FUNGT_ROOT); \
		rm -f $(TOOLCHAIN_TAR); \
		echo "SYCL toolchain installed."; \
	fi

