%define game_name berusky

Summary:        A datafile for Berusky
Name:           berusky-data
Version:        1.4
Release:        1%{?dist}
License:        GPLv2+
Group:          Amusements/Games
Source:         http://www.anakreon.cz/download/berusky/tar.gz/%{name}-%{version}.tar.gz
URL:            http://www.anakreon.cz/
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildArch:      noarch

%description
A datafile for Berusky. Berusky is a 2D logic game based on an ancient 
puzzle named Sokoban.

An old idea of moving boxes in a maze has been expanded with new logic 
items such as explosives, stones, special gates and so on. 
In addition, up to five bugs can cooperate and be controlled by the player.

This package contains a data for the game, i.e. files with graphics, levels,
game rules and configuration.

%prep
%setup -q -n %{name}-%{version}

%build

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}%{_datadir}/%{game_name}

cp -r GameData %{buildroot}%{_datadir}/%{game_name}
cp -r Graphics %{buildroot}%{_datadir}/%{game_name}
cp -r Levels   %{buildroot}%{_datadir}/%{game_name}
cp README   %{buildroot}%{_datadir}/%{game_name}
cp COPYING  %{buildroot}%{_datadir}/%{game_name}

mkdir -p %{buildroot}/var/games/%{game_name}
install -m 644 berusky.ini %{buildroot}/var/games/%{game_name}

%clean
rm -rf %{buildroot}

%files
%defattr(-, root, root)
%dir %{_datadir}/%{game_name}
%{_datadir}/%{game_name}/*
%dir /var/games/%{game_name}
/var/games/%{game_name}/*

%changelog
* Sun Mar 25 2012 Martin Stransky <stransky@redhat.com> 1.4-1
- Update to 1.4

* Thu Jan 12 2012 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.0-9
- Rebuilt for https://fedoraproject.org/wiki/Fedora_17_Mass_Rebuild

* Mon Feb 07 2011 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.0-8
- Rebuilt for https://fedoraproject.org/wiki/Fedora_15_Mass_Rebuild

* Thu Nov 19 2009 Martin Stransky <stransky@redhat.com> 1.0-7
- fixed licence & #473628

* Fri Jul 24 2009 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.0-6
- Rebuilt for https://fedoraproject.org/wiki/Fedora_12_Mass_Rebuild

* Mon Feb 23 2009 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.0-5
- Rebuilt for https://fedoraproject.org/wiki/Fedora_11_Mass_Rebuild

* Mon Jul 14 2008 Tom "spot" Callaway <tcallawa@redhat.com> 1.0-4
- fix license tag

* Tue May 8 2007 Martin Stransky <stransky@redhat.com> 1.0-3
- added build section

* Mon Apr 23 2007 Martin Stransky <stransky@redhat.com> 1.0-2
- fixes from #237416

* Fri Apr 20 2007 Martin Stransky <stransky@redhat.com> 1.0-1
- initial build

