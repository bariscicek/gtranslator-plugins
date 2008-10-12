/*
 * gtranslator-spell-plugin.h
 * 
 * Copyright (C) 2008 Baris Cicek <baris@teamforce.name.tr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

/* This is a modified version of gedit-spell-plugin.h */
/* included in gedit distribution in GNOME
 *  * Copyright (c) 2002-2005 Paolo Maggi.
 *   */


#ifndef __GTR_SPELL_PLUGIN_H__
#define __GTR_SPELL_PLUGIN_H__

#include <glib.h>
#include <glib-object.h>
#include <gconf/gconf-client.h>
#include <gtranslator/plugin.h>

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTR_TYPE_SPELL_PLUGIN		(gtranslator_spell_plugin_get_type ())
#define GTR_SPELL_PLUGIN(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), GTR_TYPE_SPELL_PLUGIN, GtranslatorSpellPlugin))
#define GTR_SPELL_PLUGIN_CLASS(k)	(G_TYPE_CHECK_CLASS_CAST((k), GTR_TYPE_SPELL_PLUGIN, GtranslatorSpellPluginClass))
#define GTR_IS_SPELL_PLUGIN(o)	(G_TYPE_CHECK_INSTANCE_TYPE ((o), GTR_TYPE_SPELL_PLUGIN))
#define GTR_IS_SPELL_PLUGIN_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), GTR_TYPE_SPELL_PLUGIN))
#define GTR_SPELL_PLUGIN_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), GTR_TYPE_SPELL_PLUGIN, GtranslatorSpellPluginClass))

/* Private structure type */
typedef struct _GtranslatorSpellPluginPrivate	GtranslatorSpellPluginPrivate;

/*
 * Main object structure
 */
typedef struct _GtranslatorSpellPlugin	GtranslatorSpellPlugin;

struct _GtranslatorSpellPlugin
{
	GtranslatorPlugin parent_instance;
	GtranslatorSpellPluginPrivate *priv;
};

/*
 * Class definition
 */
typedef struct _GtranslatorSpellPluginClass	GtranslatorSpellPluginClass;

struct _GtranslatorSpellPluginClass
{
	GtranslatorPluginClass parent_class;
};

/*
 * Public methods
 */
GType	gtranslator_spell_plugin_get_type		(void) G_GNUC_CONST;

/* All the plugins must implement this function */
G_MODULE_EXPORT GType register_gtranslator_plugin (GTypeModule *module);

G_END_DECLS

#endif /* __GTR_SPELL_PLUGIN_H__ */
