# Third-Party Licenses

FunGT uses and distributes third-party software components licensed under their respective licenses.
This document provides attribution and licensing information for those components.

---

## Intel DPC++ / SYCL Compiler

FunGT uses the Intel DPC++/SYCL compiler to enable GPU acceleration on Intel integrated graphics.

- **Project:** Intel LLVM with SYCL support  
- **Source:** https://github.com/intel/llvm (SYCL branch)  
- **License:** Apache License 2.0 with LLVM Exceptions  
- **Copyright:** Copyright (c) Intel Corporation  

### Distribution

The Intel DPC++ toolchain is **not stored in the FunGT Git repository**.

Instead, it is distributed as a **prebuilt binary package** via:

- **FunGT GitHub Releases**

The release archive includes:
- The Intel DPC++ compiler binaries
- Required runtime components
- Original license texts and attribution notices as provided by Intel

### License Availability

The full license text is included **within the extracted toolchain package** at:

`toolchain/sycl/linux_x64/dpcpp/licenses/LICENSE.TXT`

This satisfies the redistribution and attribution requirements of the Apache License 2.0 with LLVM Exceptions.

### Purpose in FunGT

Intel DPC++ enables:
- SYCL-based GPU compute for path tracing on Intel integrated graphics
- GPU-accelerated physics simulation
- Cross-platform heterogeneous computing
- Professional-grade rendering and simulation on consumer hardware

### License Compliance

FunGT distributes the Intel DPC++ binaries under the terms of the Apache License 2.0 with LLVM Exceptions.
This license permits:
- Binary redistribution
- Commercial use
- Modification and derivative works

All required license texts, notices, and attributions are included in the distributed toolchain package.

---

## Other Dependencies

### OpenGL, GLFW, GLAD, GLM
- **License:** Various (MIT, zlib, etc.)
- **Usage:** Core rendering, windowing, and math utilities

### Assimp (Open Asset Import Library)
- **License:** BSD 3-Clause
- **Usage:** 3D model loading and import

### ImGui
- **License:** MIT
- **Usage:** User interface and developer tooling

### stb_image
- **License:** MIT / Public Domain
- **Usage:** Image loading

---

For questions regarding licensing or attribution, please open an issue on the FunGT GitHub repository.
