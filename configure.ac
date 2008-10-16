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
GCONF_REQUIRED=2.18.0
GTRANSLATOR_REQUIRED=1.9
ENCHANT_REQUIRED=1.2.0


AC_SUBST(GCONF_REQUIRED)
AC_SUBST(GTRANSLATOR_REQUIRED)
AC_SUBST(ENCHANGE_REQUIRED)

PKG_CHECK_MODULES(GTRANSLATOR,[ 
gtranslator-2.0 >= $GTRANSLATOR_REQUIRED
gconf-2.0 >= $GCONF_REQUIRED
enchant >= $ENCHANT_REQUIRED
])

GTRANSLATOR_LIBS="${GTRANSLATOR_LIBS}"
GTRANSLATOR_CFLAGS="${GTRANSLATOR_CFLAGS}"
ENCHANT_LIBS="${ENCHANT_LIBS}"
ENCHANT_CFLAGS="${ENCHANT_CFLAGS}"

AC_SUBST(GTRANSLATOR_LIBS)
AC_SUBST(GTRANSLATOR_CFLAGS)
AC_SUBST(ENCHANT_LIBS)
AC_SUBST(ENCHANT_CFLAGS)

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
  plugins="punctuation spell"
fi

AC_SUBST([BUILD_PLUGINS], [$plugins])
AC_PATH_PROG(GLIB_GENMARSHAL, glib-genmarshal)

AC_CONFIG_FILES([Makefile
                 plugins/Makefile
                 plugins/punctuation/Makefile
                 plugins/spell/Makefile])
AC_OUTPUT

