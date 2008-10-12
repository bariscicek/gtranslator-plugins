# Alternate language plugin
plugindir = $(libdir)/gtranslator/plugins

INCLUDES = \
	-I$(top_srcdir) 				\
	-I$(top_srcdir)/src				\
	-I$(top_srcdir)/src/plugin-system		\
	-I$(top_srcdir)/src/dialogs			\
 $(ENCHANT_CFLAGS)                   \
	$(GTRANSLATOR_CFLAGS) 				\
	$(WARN_CFLAGS)					\
	$(DISABLE_DEPRECATED_CFLAGS)			\
	-DGTR_LOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale"\"

plugin_LTLIBRARIES = \
	libspell.la

libspell_la_SOURCES = \
	gtranslator-spell-plugin.c \
	gtranslator-spell-plugin.h \
	gtranslator-spell-checker.c \
	gtranslator-spell-checker.h \
	gtranslator-automatic-spell-checker.c \
	gtranslator-automatic-spell-checker.h


libspell_la_LDFLAGS = \
	$(PLUGIN_LIBTOOL_FLAGS)				\
	$(GTRANSLATOR_LIBS)
libspell_la_LIBADD  = $(ENCHANT_LIBS)

# Plugin Info

#plugin_in_files = spell.gtranslator-plugin.desktop.in

#%.gtranslator.plugin: %.gtranslator-plugin.desktop.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

#plugin_DATA = $(plugin_in_files:.gtranslator-plugin.desktop.in=.gtranslator.plugin)

EXTRA_DIST = $(plugin_in_files)

CLEANFILES = $(plugin_DATA)
DISTCLEANFILES = $(plugin_DATA)

