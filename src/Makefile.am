libhwc_la_LTLIBRARIES = libhwc.la
libhwc_ladir = $(libdir)
libhwc_la_LDFLAGS = ${XLIBS_CFLAGS}
libhwc_la_LIBADD = -lX11 -lXext
libhwc_la_CFLAGS = ${XLIBS_CFLAGS}

AM_CPPFLAGS = -I$(top_srcdir)/src \
	-I$(top_srcdir)/include/X11 \
	-I$(top_srcdir)/include/X11/extensions

libhwc_la_SOURCES = hwc.c

libhwcincludedir = ${includedir}/X11/extensions
libhwcinclude_HEADERS = $(top_srcdir)/include/X11/extensions/hwc.h 

