# Process this file with autoconf to produce a configure script.

GTRANSLATOR_MAJOR=2.0
GTRANSLATOR_PLUGINS_MAJOR=2.0

AC_INIT(gtranslator-plugins, 2.0, http://bugzilla.gnome.org/enter_bug.cgi?product=gtranslator)

AC_CONFIG_SRCDIR([plugins])
AC_PREFIX_PROGRAM([gtranslator])

AC_SUBST(GTRANSLATOR_PLUGINS_MAJOR)

AC_PREREQ(2.59)

AM_INIT_AUTOMAKE([1.8 dist-bzip2 no-dist-gzip])

GNOME_DOC_INIT

AM_MAINTAINER_MODE
AC_CONFIG_HEADERS([config.h])
AC_DISABLE_STATIC

AC_PATH_PROG(GCONFTOOL, gconftool-2)

AC_PROG_LIBTOOL

AC_ISC_POSIX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

AC_SUBST(ACLOCAL_AMFLAGS, "$ACLOCAL_FLAGS -I m4")

# ================================================================
# Gettext stuff.
# ================================================================

GETTEXT_PACKAGE=gtranslator-plugins
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [Gettext package])
AM_GLIB_GNU_GETTEXT

# ================================================================
# Start of pkg-config checks
# ================================================================


PKG_CHECK_MODULES(GTRANSLATOR,[ 
    gtranslator-2.0 >= 1.9
])

GTRANSLATOR_LIBS="${GTRANSLATOR_LIBS}"
AC_SUBST(GTRANSLATOR_LIBS)
AC_SUBST(GTRANSLATOR_CFLAGS)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([string.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_ARG_WITH(all-plugins,
[  --with-all-plugins	   compile all plugins there exist],
[plugins="all"],
[plugins="default"])

if test "x$plugins" = xall
then
  plugins="punctuation"
fi

AC_SUBST([BUILD_PLUGINS], [$plugins])


AC_CONFIG_FILES([Makefile
                 plugins/Makefile
                 plugins/punctuation/Makefile])
AC_OUTPUT

