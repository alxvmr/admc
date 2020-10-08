%define _unpackaged_files_terminate_build 1

Name: admc
Version: 0.3.1
Release: alt1

Summary: AD editor
License: GPLv2+
Group: Other
Url: https://github.com/altlinuxteam/admc

BuildRequires(pre): cmake
BuildRequires(pre): rpm-macros-cmake
BuildRequires(pre): gcc-c++
BuildRequires(pre): qt5-base-devel
BuildRequires(pre): qt5-tools-devel
BuildRequires(pre): catch2-devel
BuildRequires(pre): cmake-modules

BuildRequires: libldap-devel
BuildRequires: libsasl2-devel
BuildRequires: libsmbclient-devel
BuildRequires: libcmocka-devel
BuildRequires: qt5-base-common
BuildRequires: doxygen
BuildRequires: libuuid-devel
BuildRequires: glib2-devel
BuildRequires: libpcre-devel

Requires: libldap
Requires: libsasl2
Requires: libsmbclient
Requires: libuuid
Requires: qt5-base-common
Requires: glib2

Source0: %name-%version.tar

%package gpgui
Summary: Group Policy Template Editor
Group: Other

%description
AD editor

%description gpgui
Group Policy Template editor GUI

%prep
%setup -q

%build
%cmake -DCMAKE_INSTALL_LIBDIR=%_libdir
%cmake_build VERBOSE=1

%install
cd BUILD
%makeinstall_std

%files
%doc README.md
%_bindir/admc
%_libdir/libadldap.so

%files gpgui
%_bindir/gpgui
%_libdir/libgptbackend.so

%changelog
* Tue Jul 28 2020 Dmitry Degtyarev <kevl@altlinux.org> 0.3.1-alt1
- Fixed login dialog closing app
- Fixed app sometimes segfaulting when reading ber format attributes

* Fri Jul 24 2020 Igor Chudov <nir@altlinux.org> 0.3.0-alt1
- Build ADMC for all architectures

* Fri Jul 24 2020 Igor Chudov <nir@altlinux.org> 0.2.0-alt1
- Translations added
- Logon dialog implemented
- libadldap improved
- Various UI improvements added

* Thu May 21 2020 Igor Chudov <nir@altlinux.org> 0.1.0-alt1
- Initial build

