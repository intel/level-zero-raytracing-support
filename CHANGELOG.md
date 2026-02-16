Version History
---------------

### oneAPI Level Zero Ray Tracing Support 1.2.3
- Updating default TBB version for static TBB build to v2022.3.0.
- Fixed potential global variable initialization order issue when linking TBB statically.
- Moved packaging files to separate packaging folder.

### oneAPI Level Zero Ray Tracing Support 1.2.2
- Added headers for TBB 2021.6.0 and avoiding fetching headers of that TBB version.

### oneAPI Level Zero Ray Tracing Support 1.2.1
- Added support to specify TBB headers to use for compilation

### oneAPI Level Zero Ray Tracing Support 1.2.0
- Added support for Level Zero Extension ZE_extension_rtas
- Updated to Level Zero API header 1.13.1
- Fixed wrong assertion that triggered when using device memory for RTAS.
- Fixed compile issues when AVX was enabled.

### oneAPI Level Zero Ray Tracing Support 1.1.0
- Added support for PTL RTAS layout.

### oneAPI Level Zero Ray Tracing Support 1.0.0
-   Initial implementation of oneAPI Level Zero Ray Tracing Support library
