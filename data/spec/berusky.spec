Summary:        Sokoban clone
Name:           berusky
Version:        1.4
Release:        1%{?dist}
License:        GPLv2+
Group:          Amusements/Games
Source:         http://www.anakreon.cz/download/berusky/tar.gz/%{name}-%{version}.tar.gz
Source1:        berusky.desktop
Source2:        berusky.png
URL:            http://www.anakreon.cz/
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
Requires:  	berusky-data
Requires:  	SDL
BuildRequires:  SDL-devel desktop-file-utils
BuildRequires:  gtk2-devel


%description
Berusky is a 2D logic game based on an ancient puzzle named Sokoban.

An old idea of moving boxes in a maze has been expanded with new logic
items such as explosives, stones, special gates and so on.
In addition, up to five bugs can cooperate and be controlled by the player.

This package contains a binary for the game.

%prep
%setup -q -n %{name}-%{version}

%build
%configure \
    CFLAGS="$RPM_OPT_FLAGS"

make %{?_smp_mflags}

%install
rm -rf %{buildroot}
make DESTDIR=%{buildroot} install

mkdir -p %{buildroot}/%{_docdir}/%{name}-%{version}

pushd %{buildroot}/usr/doc/berusky
mv * %{buildroot}%{_docdir}/%{name}-%{version}
popd

rm -rf %{buildroot}/%{_datadir}/%{name}

# Install icon and desktop file
mkdir -p $RPM_BUILD_ROOT%{_datadir}/icons/hicolor/32x32/apps
cp %{SOURCE2} $RPM_BUILD_ROOT%{_datadir}/icons/hicolor/32x32/apps

desktop-file-install --vendor fedora --dir $RPM_BUILD_ROOT%{_datadir}/applications --add-category X-Fedora %{SOURCE1}

%clean
rm -rf %{buildroot}

%post
touch --no-create %{_datadir}/icons/hicolor || :
if [ -x %{_bindir}/gtk-update-icon-cache ]; then
 %{_bindir}/gtk-update-icon-cache --quiet %{_datadir}/icons/hicolor || :
fi

%postun
touch --no-create %{_datadir}/icons/hicolor || :
if [ -x %{_bindir}/gtk-update-icon-cache ]; then
 %{_bindir}/gtk-update-icon-cache --quiet %{_datadir}/icons/hicolor || :
fi

%files
%defattr(-, root, root)
%doc %dir %{_docdir}/%{name}-%{version}
%doc %{_docdir}/%{name}-%{version}/*
%{_bindir}/berusky
%{_datadir}/applications/fedora-berusky.desktop
%{_datadir}/icons/hicolor/32x32/apps/berusky.png

%changelog
* Thu Jan 12 2012 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.2-4
- Rebuilt for https://fedoraproject.org/wiki/Fedora_17_Mass_Rebuild

* Tue Dec 06 2011 Adam Jackson <ajax@redhat.com> - 1.2-3
- Rebuild for new libpng

* Wed Jun 22 2011 Martin Stransky <stransky@redhat.com> 1.2-2
- Fixed rhbz#689106 - seg. fault after start

* Sun Mar 6 2011 Martin Stransky <stransky@redhat.com> 1.2-1
- updated to 1.2

* Thu Nov 19 2009 Martin Stransky <stransky@redhat.com> 1.1-13
- fixed dirs (#473628)

* Fri Jul 24 2009 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.1-12
- Rebuilt for https://fedoraproject.org/wiki/Fedora_12_Mass_Rebuild

* Mon Feb 23 2009 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.1-11
- Rebuilt for https://fedoraproject.org/wiki/Fedora_11_Mass_Rebuild

* Tue Dec 2 2008 Martin Stransky <stransky@redhat.com> 1.1-10
- added patch from #458477 - Berusky aborts at end 
  of intermediate level 18

* Mon Jul 14 2008 Tom "spot" Callaway <tcallawa@redhat.com> - 1.1-9
- fix license tag

* Tue Feb 19 2008 Fedora Release Engineering <rel-eng@fedoraproject.org> - 1.1-8
- Autorebuild for GCC 4.3

* Fri Jan 18 2008 Martin Stransky <stransky@redhat.com> 1.1-7
- rebuild

* Wed Aug 29 2007 Fedora Release Engineering <rel-eng at fedoraproject dot org> - 1.1-6
- Rebuild for selinux ppc32 issue.

* Thu Jun 26 2007 Martin Stransky <stransky@redhat.com> 1.1-5
- added a menu entry and an icon

* Wed May 23 2007 Martin Stransky <stransky@redhat.com> 1.1-4
- removed spec files from binary rpm package

* Tue May 8 2007 Martin Stransky <stransky@redhat.com> 1.1-3
- moved documentation from doc/berusky-1.1/berusky to doc/berusky-1.1

* Tue May 8 2007 Martin Stransky <stransky@redhat.com> 1.1-2
- fixed build in mock

* Mon Apr 23 2007 Martin Stransky <stransky@redhat.com> 1.1-1
- fixes from #237416

* Fri Apr 20 2007 Martin Stransky <stransky@redhat.com> 1.0-1
- initial build
