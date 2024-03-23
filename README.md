C++ Simulated Airline Inventory Management System Library
=========================================================

# Summary
AirInv is a C++ library of airline inventory management classes and
functions, mainly targeting simulation purposes.

AirInv makes an extensive use of existing open-source libraries for
increased functionality, speed and accuracy. In particular the
Boost (C++ Standard Extensions: http://www.boost.org) library is used.

AirInv is the one of the components of the Travel Market Simulator
(https://travel-sim.org). However, it may be used in a
stand-alone mode.

# Installation

## On Fedora/CentOS/RedHat distribution
Just use DNF:
```bash
$ dnf -y install airinv-devel airinv-doc
```

You can also get the RPM packages (which may work on Linux
distributions like Suse and Mandriva) from the Fedora repository
(_e.g._, for Fedora Rawhide, 
https://fr2.rpmfind.net/linux/RPM/fedora/devel/rawhide/x86_64/)


## Building the library and test binary from Git repository
The Git repository may be cloned as following:
```bash
$ git clone git@github.com:airsim/rmol.git rmolgit # through SSH
$ git clone https://github.com/airsim/rmol.git # if the firewall filters SSH
$ cd rmolgit
```

Then, you need the following packages (Fedora/RedHat/CentOS names here, 
but names may vary according to distributions):
* cmake
* gcc-c++
* boost-devel / libboost-dev
* zeromq-devel
* python-devel / python-dev
* gettext-devel / gettext-dev
* sqlite3-devel / libsqlite3-dev
* readline-devel / readline-dev
* ncurses-devel
* soci-mysql-devel, soci-sqlite3-devel
* stdair-devel / libstdair-dev
* airrac-devel / libairrac-dev
* rmol-devel / librmol-dev
* sevmgr-devel / libsevmgr-dev
* doxygen, ghostscript, graphviz
* tetex-latex (optional)
* rpm-build (optional)

## Building the library and test binary from the tarball
The latest stable source tarball (`airinv*.tar.gz` or `.bz2`)
can be found here:
https://github.com/airsim/airinv/releases

To customise the following to your environment, you can alter the path
to the installation directory:
```bash
export INSTALL_BASEDIR="${HOME}/dev/deliveries"
export AIRINV_VER="1.00.9"
if [ -d /usr/lib64 ]; then LIBSUFFIX="64"; fi
export LIBSUFFIX_4_CMAKE="-DLIB_SUFFIX=$LIBSUFFIX"
```

Then, as usual:
* To configure the project, type something like:
```bash
  mkdir build && cd build
  cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_BASEDIR}/airinv-$AIRINV_VER \
   -DWITH_STDAIR_PREFIX=${INSTALL_BASEDIR}/stdair-stable \
   -DWITH_SEVMGR_PREFIX=${INSTALL_BASEDIR}/sevmgr-stable \
   -DWITH_AIRRAC_PREFIX=${INSTALL_BASEDIR}/airrac-stable \
   -DWITH_RMOL_PREFIX=${INSTALL_BASEDIR}/rmol-stable \
   -DCMAKE_BUILD_TYPE:STRING=Debug -DENABLE_TEST:BOOL=ON -DINSTALL_DOC:BOOL=ON \
   -DRUN_GCOV:BOOL=OFF ${LIBSUFFIX_4_CMAKE} ..
```
* To build the project, type:
```bash
  make
```
* To test the project, type:
```bash
  make check
```
* To install the library (`libairinv*.so*`) and the binary (`airinv`),
  just type:
```bash
  make install
  cd ${INSTALL_BASEDIR}
  rm -f airinv-stable && ln -s airinv-${AIRINV_VER} airinv-stable
  cd -
```
* To package the source files, type:
```bash
  make dist
```
* To package the binary and the (HTML and PDF) documentation:
```bash
  make package
```
* To browse the (just installed, if enabled) HTML documentation:
```bash
  midori file://${INSTALL_BASEDIR}/airinv-$AIRINV_VER/share/doc/airinv/html/index.html
```
* To browse the (just installed, if enabled) PDF documentation:
```bash
  evince ${INSTALL_BASEDIR}/airinv-$AIRINV_VER/share/doc/airinv/html/refman.pdf
```
* To run the local binary version:
```bash
  ./airinv/airinv -b
```
* To run the installed version:
```bash
  ${INSTALL_BASEDIR}/airinv-stable/bin/airinv -b
```

Denis Arnaud

