## Process this file with automake to produce Makefile.in
SUBDIRS = ginac check ginsh tools @GINACCINTDIR@ doc
DIST_SUBDIRS = cint ginac check ginsh tools doc

# Requires automake 1.4
AUTOMAKE_OPTIONS = 1.4

# GiNaC runtime configuration script
bin_SCRIPTS = ginac-config

# All the rest of the distributed files
EXTRA_DIST = autogen.sh ginac.m4

# M4 macro file for inclusion with autoconf
m4datadir = $(datadir)/aclocal
m4data_DATA = ginac.m4

# Rule to build tar-gzipped distribution package
$(PACKAGE)-$(VERSION).tar.gz: dist

# Rule to build RPM distribution package
rpm: $(PACKAGE)-$(VERSION).tar.gz
	cp $(PACKAGE)-$(VERSRION).tar.gz /usr/src/redhat/SOURCES
	rpm -ba GiNaC.spec
