## Process this file with automake to produce Makefile.in
SUBDIRS = ginac check ginsh tools doc
DIST_SUBDIRS = ginac check ginsh tools doc

# Requires automake 1.4
AUTOMAKE_OPTIONS = 1.4

# GiNaC runtime configuration script
bin_SCRIPTS = ginac-config

# Man pages
man_MANS = ginac-config.1

# M4 macro file for inclusion with autoconf
m4datadir = $(datadir)/aclocal
m4data_DATA = ginac.m4

# All the rest of the distributed files
EXTRA_DIST = ginac.m4 GiNaC.spec depcomp

# Rule to build tar-bzipped distribution package
$(PACKAGE)-$(VERSION).tar.gz: dist

# Rule to build RPM distribution package
rpm: $(PACKAGE)-$(VERSION).tar.gz
	rpmbuild -ta --clean $(PACKAGE)-$(VERSION).tar.gz
