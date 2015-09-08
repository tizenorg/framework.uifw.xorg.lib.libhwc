# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(libhwc, 0.1.0, [http://])
AC_CONFIG_SRCDIR([src/hwc.c])
AC_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE([dist-bzip2])

# Checks for programs.
AC_PROG_LIBTOOL
AC_PROG_CC

# Checks for pkg-configs
PKG_CHECK_MODULES(XLIBS, x11 xext)
AC_SUBST(XLIBS_CFLAGS)
AC_SUBST(XLIBS_LIBS)

PKG_CHECK_MODULES(HWCPROTO, hwcproto)
AC_SUBST(HWCPROTO_CFLAGS)
AC_SUBST(HWCPROTO_LIBS)

pkgconfigdir=${libdir}/pkgconfig/
AC_SUBST(pkgconfigdir)


AC_CONFIG_FILES([Makefile
				src/Makefile
				libhwc.pc])
AC_OUTPUT
