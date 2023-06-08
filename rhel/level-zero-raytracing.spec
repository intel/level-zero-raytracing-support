#it's changed by external script
%global ver 1.0.0
%global rel 1

Name: level-zero-raytracing
Version: %{ver}
Release: %{rel}%{?dist}
Summary: Level Zero Ray Tracing Support library

Group: System Environment/Libraries
License: Apache2
URL: https://github.com/oneapi-src/level-zero-raytracing
Source0: %{url}/archive/%{ver}/level-zero-raytracing-%{ver}.tar.gz

BuildRequires: make gcc-c++ cmake ninja-build git pkg-config

%description
The Level Zero Ray Tracing Support library implements high performance CPU
based construction algorithms for 3D acceleration structures that are
compatible with the ray tracing hardware of Intel GPUs.
This library is used by Intel(R) oneAPI Level Zero to implement part of the
RTAS builder extension.
This library should not get used directly but only through Level Zero.
.
Level Zero Ray Tracing Support library


%prep
%autosetup -p1 -n %{name}-%{ver}

%build
mkdir build
cd build
%cmake .. \
   -DCMAKE_BUILD_TYPE=Release \
   -DCMAKE_INSTALL_PREFIX=/usr

%make_build

%install
cd build
%make_install

%files
%defattr(-,root,root)
%config(noreplace)
%license LICENSE.txt
%license third-party-programs*
%{_libdir}/libze_intel_rt_support.so


%doc

%changelog
* Thu Jun 8 2023 Pavel Androniychuk <pavel.androniychuk@intel.com> - 1.0.0
- Spec file init
