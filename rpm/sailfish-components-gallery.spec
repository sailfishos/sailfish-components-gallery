Name:       sailfish-components-gallery-qt5

Summary:    Sailfish Gallery UI Components
Version:    0.0.6
Release:    1
Group:      System/Libraries
License:    TBD
URL:        https://bitbucket.org/jolla/ui-sailfish-components-gallery
Source0:    %{name}-%{version}.tar.bz2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(Qt5Gui)
BuildRequires:  pkgconfig(Qt5OpenGL)
BuildRequires:  pkgconfig(Qt5Concurrent)

Requires:  sailfishsilica-qt5
Requires:  nemo-qml-plugin-thumbnailer-qt5

%description
Sailfish Gallery UI Components

%package tests
Summary:    Unit tests for Sailfish Gallery UI components
Group:      System/Libraries
BuildRequires:  pkgconfig(Qt5Test)
Requires:   %{name} = %{version}-%{release}
Requires:   qt5-qtdeclarative-import-qttest

%description tests
This package contains QML unit tests for Sailfish Gallery UI components

%package ts-devel
Summary:   Translation source for sailfish-components-gallery
License:   TBD
Group:     System/Libraries

%description ts-devel
Translation source for sailfish-components-gallery

%prep
%setup -q -n %{name}-%{version}

%build

%qmake5

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}

# Copy tests
mkdir -p %{buildroot}/opt/tests/sailfish-components-gallery-qt5/test-definition
mkdir -p %{buildroot}/opt/tests/sailfish-components-gallery-qt5/auto
cp -a tests/test-definition/tests.xml %{buildroot}/opt/tests/sailfish-components-gallery-qt5/test-definition
cp -a tests/auto/*qml %{buildroot}/opt/tests/sailfish-components-gallery-qt5/auto
cp -a tests/auto/*js %{buildroot}/opt/tests/sailfish-components-gallery-qt5/auto

%qmake5_install

#
# Sailfish Gallery files
#
%files
%defattr(-,root,root,-)
%{_libdir}/qt5/qml/Sailfish/Gallery/*
%{_datadir}/translations/sailfish_components_gallery_eng_en.qm

%files tests
%defattr(-,root,root,-)
# >> files tests
/opt/tests/sailfish-components-gallery-qt5/*
# << files tests

%files ts-devel
%defattr(-,root,root,-)
%{_datadir}/translations/source/sailfish_components_gallery.ts
