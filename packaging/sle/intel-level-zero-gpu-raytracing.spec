#
# spec file for package level-zero-raytracing
#
# Copyright (c) 2019 SUSE LINUX GmbH, Nuernberg, Germany.
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via https://bugs.opensuse.org/
#
#it's changed by external script
%global ver 1.3.0
%global rel 1

Name: intel-level-zero-gpu-raytracing
Version: %{ver}
Release: %{rel}%{?dist}
Summary: oneAPI Level Zero Ray Tracing Support

Group: System Environment/Libraries
License: Apache2
URL: https://github.com/oneapi-src/level-zero-raytracing
Source0: %{url}/archive/%{ver}/intel-level-zero-gpu-raytracing-%{ver}.tar.gz

BuildRequires: make gcc-c++ cmake git pkg-config

%description
A library that provides high-performance CPU-based construction algorithms for
3D acceleration structures, designed to work with the ray tracing hardware of
Intel GPUs. Intel(R) oneAPI Level Zero uses this library to implement the Ray
Tracing Acceleration Structures (RTAS) builder extension. The library is
intended for use exclusively through Level Zero and is not meant to be accessed
directly.


%debug

%prep
%autosetup -p1 -n %{name}-%{ver}

%build
%cmake .. \
   -DCMAKE_BUILD_TYPE=Release \
   -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
   -DCMAKE_INSTALL_PREFIX=/usr \
   -DZE_RAYTRACING_TBB=build_static

%make_build

%install
cd build
%make_install

%files
%defattr(-,root,root)
%config(noreplace)
%license LICENSE.txt
%license third-party-programs*
%{_libdir}/libze_intel_gpu_raytracing.so


%doc

%changelog
* Tue Aug 25 2026 Sven Woop <sven.woop@intel.com> - 1.3.0
- Added support for NVL RTAS format.
* Mon May 18 2026 Sven Woop <sven.woop@intel.com> - 1.2.4
- Fixed crash in high quality RTAS build over instances when using device memory.
* Mon Feb 16 2026 Sven Woop <sven.woop@intel.com> - 1.2.3
- Updating default TBB version for static TBB build to v2022.3.0.
- Fixed potential global variable initialization order issue when linking TBB statically.
- Moved packaging files to separate packaging folder.
* Thu Oct 28 2025 Sven Woop <sven.woop@intel.com> - 1.2.2
- Added headers for TBB 2021.6.0 and avoiding fetching headers of that TBB version.
* Thu Oct 23 2025 Sven Woop <sven.woop@intel.com> - 1.2.1
- Added support to specify TBB headers to use for compilation
* Fri Sep 5 2025 Sven Woop <sven.woop@intel.com> - 1.2.0
- Added support for Level Zero Extension ZE_extension_rtas
- Updated to Level Zero API header 1.13.1
- Fixed wrong assertion that triggered when using device memory for RTAS.
- Fixed compile issues when AVX was enabled.
* Thu Mar 6 2025 Sven Woop <sven.woop@intel.com> - 1.1.0
- Added support for PTL RTAS layout.
* Thu Jun 8 2023 Pavel Androniychuk <pavel.androniychuk@intel.com> - 1.0.0
- Spec file init
