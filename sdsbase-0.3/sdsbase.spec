Summary: qpl is used to view datas located in shared memory segments
Name: sdsbase
Version: 0.3
Release: 9
Copyright: GPL
Group: Applications/Graphics
Source: sdsbase-0.3.tar.gz
Packager: Roland Schaefer (Roland.Schaefer@ifp.fzk.de)
%description

You have 2d data loaded to a so called shared data file?
Use qpl to view them!

%prep
%setup

%build
make all

%install
make install

%files

/usr/local/bin/qpl

/usr/local/bin/dsl
/usr/local/bin/dslold
/usr/local/bin/dsls
/usr/local/bin/dsrm
/usr/local/bin/dssp
/usr/local/bin/dsd_s
/usr/local/bin/dsd
/usr/local/bin/dss
/usr/local/bin/dsreg
/usr/local/bin/dsg
/usr/local/bin/dsf
/usr/local/bin/colxy

/usr/local/bin/dssc

/usr/local/bin/dsdel
/usr/local/bin/dsn

/usr/local/bin/dssort
/usr/local/bin/dsfunc
/usr/local/bin/dsfilter
/usr/local/bin/dsint
/usr/local/bin/dsaco
/usr/local/bin/dscat
/usr/local/bin/dseinige
/usr/local/bin/dsdeglitch

/usr/local/bin/dsstat
/usr/local/bin/dsminmax
/usr/local/bin/dsmatch
/usr/local/bin/dslog
/usr/local/bin/dshist

/usr/local/lib/libsd.so
/usr/local/lib/libsd.so.0
/usr/local/lib/libsd.so.0.3

/usr/local/include/sd

%post
ldconfig
