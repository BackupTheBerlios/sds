Summary: qpl is used to view datas located in shared memory segments
Name: sdsprog1 
Version: 0.4
Release: 2
Copyright: GPL
Group: Applications/Graphics
Source: sdsprog1-0.4.tar.gz
Packager: Roland Schaefer (Roland.Schaefer@ifp.fzk.de)
%description

You have 2d data loaded to a so called shared data file?
With the programs of this distribution you may manipulate them.

%prep
%setup

%build
make all

%install
make install

%files

/usr/local/bin/dsspline

/usr/local/bin/dsfit

/usr/local/bin/dsadd

/usr/local/bin/dsmul

/usr/local/bin/dsft

/usr/local/bin/dswl

/usr/local/bin/dspeak

/usr/local/bin/dscf

/usr/local/bin/dsxfit
