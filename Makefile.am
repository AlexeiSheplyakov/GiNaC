## Process this file with automake to produce Makefile.in
SUBDIRS = ginac check ginsh tools doc
DIST_SUBDIRS = ginac check ginsh tools doc

# pkg-config metadata
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ginac.pc
$(pkgconfig_DATA): config.status

BUILD_HELPERS = scripts/yaptu.py scripts/fixupind.py

# All the rest of the distributed files
EXTRA_DIST = ginac.pc GiNaC.spec $(BUILD_HELPERS)

# Rule to build tar-bzipped distribution package
$(PACKAGE)-$(VERSION).tar.gz: dist

# Rule to build RPM distribution package
rpm: $(PACKAGE)-$(VERSION).tar.gz
	rpmbuild -ta --clean $(PACKAGE)-$(VERSION).tar.gz

ACLOCAL_AMFLAGS = -I m4
