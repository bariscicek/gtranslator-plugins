# spell plugin plugin
plugindir = $(libdir)/gtranslator/plugins
gladedir = $(libdir)/gtranslator/plugins/spell

BUILT_SOURCES=		\
	gtranslator-spell-checker-dialog-marshal.c	\
	gtranslator-spell-checker-dialog-marshal.h

INCLUDES = \
	-I$(top_srcdir) 				\
	-I$(top_srcdir)/src				\
	-I$(top_srcdir)/src/plugin-system		\
	-I$(top_srcdir)/src/dialogs			\
 $(ENCHANT_CFLAGS)                   \
	$(GTRANSLATOR_CFLAGS) 				\
	$(WARN_CFLAGS)					\
	$(DISABLE_DEPRECATED_CFLAGS)			\
	-DGTR_LOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DGTR_GLADEDIR=\""$(gladedir)"\" 

plugin_LTLIBRARIES = \
	libspell.la

libspell_la_SOURCES = \
	gtranslator-spell-plugin.c \
	gtranslator-spell-plugin.h \
	gtranslator-spell-checker.c \
	gtranslator-spell-checker.h \
	gtranslator-spell-checker-dialog.c \
	gtranslator-spell-checker-dialog.h \
	gtranslator-automatic-spell-checker.c \
	gtranslator-automatic-spell-checker.h

libspell_la_LDFLAGS = \
	$(PLUGIN_LIBTOOL_FLAGS)				\
	$(GTRANSLATOR_LIBS)

libspell_la_LIBADD  = $(ENCHANT_LIBS)

glade_DATA = spell-checker.glade

gtranslator-spell-checker-dialog-marshal.h: gtranslator-spell-checker-dialog-marshal.list $(GLIB_GENMARSHAL)
		$(GLIB_GENMARSHAL) $< --header --prefix=gtranslator_marshal > $@

gtranslator-spell-checker-dialog-marshal.c: gtranslator-spell-checker-dialog-marshal.list $(GLIB_GENMARSHAL)
		echo "#include \"gtranslator-spell-checker-dialog-marshal.h\"" > $@ && $(GLIB_GENMARSHAL) $< --body --prefix=gtranslator_marshal >> $@

# Plugin Info

#plugin_in_files = spell.gtranslator-plugin.desktop.in

#%.gtranslator.plugin: %.gtranslator-plugin.desktop.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

#plugin_DATA = $(plugin_in_files:.gtranslator-plugin.desktop.in=.gtranslator.plugin)

EXTRA_DIST = \
	$(plugin_in_files) \
	$(glade_DATA)

CLEANFILES = $(plugin_DATA)
DISTCLEANFILES = $(plugin_DATA)

