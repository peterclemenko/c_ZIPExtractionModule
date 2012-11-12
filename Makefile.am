AUTOMAKE_OPTIONS = subdir-objects

ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

lib_LTLIBRARIES = libzipextractionmodule-@ZIPEXTRACTIONMODULE_API_VERSION@.la

AM_CPPFLAGS = -I../../../framework -I$(srcdir)/../../..

libzipextractionmodule_@ZIPEXTRACTIONMODULE_API_VERSION@_la_SOURCES = ZipExtractionModule.cpp

libzipextractionmodule_@ZIPEXTRACTIONMODULE_API_VERSION@_la_LDFLAGS = -version-info $(ZIPEXTRACTIONMODULE_SO_VERSION)


zipextractionmodule_libincludedir = $(libdir)/zipextractionmodule-$(ZIPEXTRACTIONMODULE_API_VERSION)/include

nodist_zipextractionmodule_libinclude_HEADERS = zipextractionmoduleconfig.h

pkgconfigdir = $(libdir)/pkgconfig

pkgconfig_DATA = zipextractionmodule-$(ZIPEXTRACTIONMODULE_API_VERSION).pc

dist_noinst_SCRIPTS = autogen.sh
