#
%global mydocs __tmp_docdir
#
Name:           airinv
Version:        99.99.99
Release:        1%{?dist}

Summary:        C++ library providing a clean API for parsing travel-focused requests

Group:          System Environment/Libraries 
License:        LGPLv2
URL:            http://%{name}.sourceforge.net
Source0:        http://downloads.sourceforge.net/%{name}/%{name}-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:  gsl-devel >= 1.8
BuildRequires:  boost-devel >= 1.34
BuildRequires:  mysql-devel >= 5.0
BuildRequires:  mysql++-devel >= 3.0
BuildRequires:  cppunit-devel >= 1.10
BuildRequires:  xapian-core-devel >= 1.0
BuildRequires:  soci-devel >= 3.0
#Requires:       

%description
That project aims at providing a clean API, and the corresponding C++
implementation, for parsing travel-focused requests (e.g., "washington
dc beijing monday a/r +aa -ua 1 week 2 adults 1 dog").

The C++ library uses Xapian (http://www.xapian.org) for the
Information Retrieval part, on freely available travel-related data
(e.g., country names and codes, city names and codes, airline names
and codes, etc.).

The C++ library exposes a simple, clean and object-oriented, API. For
instance, the static Parse() method takes, as input, a string
containing the travel request, and yields, as output, the list of the
recognised terms as well as their corresponding types. As an example,
the travel request "washington dc beijing monday a/r +aa -ua 1 week 2
adults 1 dog" would give the following list:
  * Origin airport: Washington, DC, USA
  * Destination airport: Beijing, China
  * Date of travel: next Monday
  * Date of return: 1 week after next Monday
  * Preferred airline: American Airlines; 
      non-preferred airline: United Airlines
  * Number of travellers: 2 adults and a dog

The output can then be used by other systems, for instance to book the
corresponding travel or to visualise it on a map and calendar and to
share it with others.

%package        devel
Summary:        Header files, libraries and development documentation for %{name}
Group:          Development/Libraries
Requires:       %{name} = %{version}-%{release}
Requires:       pkgconfig
Requires(post): info
Requires(preun): info

%description    devel
This package contains the header files, static libraries and
development documentation for %{name}. If you would like to develop
programs using %{name}, you will need to install %{name}-devel.

%package doc
Summary:        HTML documentation for the AIRINV library
Group:          Documentation
BuildArch:      noarch
BuildRequires:  doxygen, texlive-latex, texlive-dvips, ghostscript

%description doc
This package contains the documentation in the HTML format of the AIRINV
library. The documentation is the same as at the AIRINV web page.


%prep
%setup -q
# find ./doc -type f -perm 755 -exec chmod 644 {} \;
# Fix some permissions and formats
rm -f INSTALL
chmod -x ABOUT-NLS AUTHORS ChangeLog COPYING NEWS README
find . -type f -name '*.[hc]pp' -exec chmod 644 {} \;


%build
%configure --disable-static
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
%find_lang %{name}
# remove unpackaged files from the buildroot
rm -f $RPM_BUILD_ROOT%{_includedir}/%{name}/config.h
rm -f $RPM_BUILD_ROOT%{_infodir}/dir
rm -f $RPM_BUILD_ROOT%{_libdir}/lib%{name}.la
# chmod 644 doc/html/installdox doc/html/*.png doc/html/*.ico
rm -rf %{mydocs} && mkdir -p %{mydocs}
mv $RPM_BUILD_ROOT%{_docdir}/%{name}-%{version}/html %{mydocs}

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%post devel
/sbin/install-info %{_infodir}/%{name}-ref.info.* %{_infodir}/dir || :

%preun devel 
if [ "$1" = 0 ]; then
   /sbin/install-info --delete %{_infodir}/%{name}-ref.info.* %{_infodir}/dir || :
fi

%files -f %{name}.lang
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README
%{_bindir}/%{name}_indexer
%{_bindir}/%{name}_searcher
%{_libdir}/lib*.so.*
%{_mandir}/man3/%{name}.3.*

%files devel
%defattr(-,root,root,-)
%{_includedir}/%{name}
%{_bindir}/%{name}-config
%{_libdir}/lib%{name}.so
%{_libdir}/pkgconfig/%{name}.pc
%{_datadir}/aclocal/%{name}.m4
%{_infodir}/%{name}-ref.info.*
%{_mandir}/man1/%{name}-config.1.*

%files doc
%defattr(-,root,root,-)
%doc %{mydocs}/html
%doc AUTHORS ChangeLog COPYING NEWS README


%changelog
* Tue Aug  3 2010 Denis Arnaud <denis.arnaud_fedora@m4x.org> 99.99.99-1
- Upstream integration
