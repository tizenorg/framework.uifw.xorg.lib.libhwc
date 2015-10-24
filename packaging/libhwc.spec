Name:       libhwc
Summary:    X.Org HWC Extension client library
Version:    0.2.0
Release:    1
Group:      System/Libraries
License:    MIT
Source0:    %{name}-%{version}.tar.gz
BuildRequires:  pkgconfig(hwcproto)
BuildRequires:  pkgconfig(x11)
BuildRequires:  pkgconfig(xext)
BuildRequires:  pkgconfig(xrandr)


%description
HWC Extension client library


%package devel
Summary:    X.Org HWC Extension client library (development library)
Group:      System/Libraries
Requires:   %{name} = %{version}-%{release}
Requires:   pkgconfig(hwcproto)

%description devel
HWC Extension client library (development library)

%prep
%setup -q


%build

%reconfigure --disable-static
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr/share/license
cp -af COPYING %{buildroot}/usr/share/license/%{name}
%make_install


%files
%manifest libhwc.manifest
%defattr(-,root,root,-)
/usr/share/license/%{name}
%{_libdir}/libhwc.so.*


%files devel
%defattr(-,root,root,-)
%{_libdir}/libhwc.so
%{_includedir}/X11/extensions/*
%{_libdir}/pkgconfig/*


