Summary: qpl is used to view datas located in shared memory segments
Name: sdsfortranlibs
Version: 1.0
Release: 8
Copyright: GPL
Group: Applications/Graphics
Source: sdsfortranlibs-1.0.tar.gz
Packager: Roland Schaefer (Roland.Schaefer@infp.fzk.de)
%description

You have 2d data loaded to shared memory segments in my own special format?
With the programs of this distribution you may manipulate them.
This package contains fortran libs used by the sds-programs

%prep
%setup

%build
make all

%install
make install

%files

/usr/local/lib/libapprox.so
/usr/local/lib/libapprox.so.1
/usr/local/lib/libapprox.so.1.0
/usr/local/lib/libspline.so
/usr/local/lib/libspline.so.1
/usr/local/lib/libspline.so.1.0
/usr/local/lib/libfn.so
/usr/local/lib/libfn.so.1
/usr/local/lib/libfn.so.1.0

%post
ldconfig
