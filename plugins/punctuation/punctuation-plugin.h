/*
 * Copyright (C) 2008  Baris Cicek <baris@teamforce.name.tr>
 * 
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANPOILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __GTR_PUNCTUATION_PLUGIN_H__
#define __GTR_PUNCTUATION_PLUGIN_H__

#include <glib.h>
#include <glib-object.h>

#include <gtranslator/plugin.h>

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTR_TYPE_PUNCTUATION_PLUGIN		(gtranslator_punctuation_get_type ())
#define GTR_PUNCTUATION_PLUGIN(o)			(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_MESSAGEPUNCTUATION_PLUGIN, GtranslatorPunctuationPlugin))
#define GTR_PUNCTUATION_PLUGIN_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_PUNCTUATION_PLUGIN, GtranslatorPunctuationPluginClass))
#define GTR_IS_PUNCTUATION_PLUGIN(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_PUNCTUATION_PLUGIN))
#define GTR_IS_PUNCTUATION_PLUGIN_CLASS(k)		(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_PUNCTUATION_PLUGIN))
#define GTR_PUNCTUATION_PLUGIN_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_PUNCTUATION_PLUGIN_PLUGIN, GtranslatorPunctuationPluginClass))

/* Private structure type */
typedef struct _GtranslatorPunctuationPluginPrivate	GtranslatorPunctuationPluginPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorPunctuationPlugin		GtranslatorPunctuationPlugin;

struct _GtranslatorPunctuationPlugin
{
	GtranslatorPlugin parent_instance;
};

/*
 * Class definition
 */
typedef struct _GtranslatorPunctuationPluginClass	GtranslatorPunctuationPluginClass;

struct _GtranslatorPunctuationPluginClass
{
	GtranslatorPluginClass parent_class;
};

/*
 * Public methods
 */
GType	gtranslator_punctuation_plugin_get_type	(void) G_GNUC_CONST;

/* All the plugins must implement this function */
G_MODULE_EXPORT GType register_gtranslator_plugin (GTypeModule *module);

G_END_DECLS

#endif /* __GTR_PUNCTUATION_PLUGIN_H__ */
