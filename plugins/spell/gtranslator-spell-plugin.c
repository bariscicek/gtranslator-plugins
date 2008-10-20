/*
 * gtranslator-spell-plugin.c
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

/* This is a modified version of gedit-spell-plugin.c */
/* included in gedit distribution in GNOME
 *  * Copyright (c) 2002-2006 Paolo Maggi.
 *   */



#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h> /* For strlen */

#include <glib/gi18n.h>
#include <glib/gutils.h>
#include <gmodule.h>
#include <gconf/gconf-client.h>
#include <gtranslator/profile.h>
#include <gtranslator/po.h>
#include <gtranslator/tab.h>
#include <gtranslator/application.h>

#include "gtranslator-spell-plugin.h"
#include "gtranslator-spell-checker.h"
#include "gtranslator-spell-checker-dialog.h"
#include "gtranslator-automatic-spell-checker.h"


#define WINDOW_DATA_KEY "GtranslatorSpellPluginWindowData"

#define GTR_BASE_KEY "/apps/gtranslator/plugins/spell"
#define AUTOSPELL_TOGGLE_KEY GTR_BASE_KEY "/auto_spell"
#define PREFERED_BACKEND_KEY GTR_BASE_KEY "/prefered_beckend"

#define GTR_SPELL_PLUGIN_GET_PRIVATE(object)( \
						   G_TYPE_INSTANCE_GET_PRIVATE ((object), \
					       GTR_TYPE_SPELL_PLUGIN, \
					       GtranslatorSpellPluginPrivate))

GTR_PLUGIN_REGISTER_TYPE(GtranslatorSpellPlugin, gtranslator_spell_plugin)

static void	auto_spell_cb	(GtkAction *action, GtranslatorWindow *window);
static void	check_spelling_cb	(GtkAction *action, GtranslatorWindow *window);
static gboolean go_to_next_message ();

static GtkTextMark *current_mark = NULL;

struct _GtranslatorSpellPluginPrivate
{
	GConfClient		*gconf_client;
};

typedef struct
{
	GtkActionGroup *action_group;
	guint           ui_id;
	guint           message_cid;
} WindowData;

/* UI actions. */


static const GtkToggleActionEntry toggle_action_entries[] =
{
	{ "AutoSpell",
	  NULL,
	  N_("_Autocheck Spelling"),
	  NULL,
	  N_("Automatically spell-check the current entry"),
	  G_CALLBACK (auto_spell_cb),
	  FALSE
	}
};

static const GtkActionEntry action_entries[] =
{
	{ "CheckSpelling",
	  GTK_STOCK_SPELL_CHECK,
	  N_("_Check Spelling..."),
	  "<shift>F7",
	  N_("Check spelling of current translation file"),
	  G_CALLBACK (check_spelling_cb)
	}
};
			
	

static GQuark spell_checker_id = 0;

static void
gtranslator_spell_plugin_init (GtranslatorSpellPlugin *plugin)
{
	plugin->priv = GTR_SPELL_PLUGIN_GET_PRIVATE (plugin);
	
	plugin->priv->gconf_client = gconf_client_get_default ();
	
	gconf_client_add_dir (plugin->priv->gconf_client,
						  GTR_BASE_KEY,
						  GCONF_CLIENT_PRELOAD_ONELEVEL,
						  NULL);
}

static void
gtranslator_spell_plugin_finalize (GObject *object)
{

	G_OBJECT_CLASS (gtranslator_spell_plugin_parent_class)->finalize (object);
}

static void
gtranslator_spell_plugin_set_gconf_key (GtranslatorSpellPlugin *plugin, gchar *key, gchar *value) 
{ 
	/*if (g_ascii_isdigit (value[0])) {
		gconf_client_set_float (plugin->priv->gconf_client, key, g_strtod (value, NULL));
	} else { */
		gconf_client_set_string (plugin->priv->gconf_client, key, value, NULL);
	/*}*/
}

static gchar *
gtranslator_spell_plugin_get_gconf_key (GtranslatorSpellPlugin *plugin, gchar *key) 
{
	gchar *value; 
	
	value = gconf_client_get_string (plugin->priv->gconf_client, key, NULL);
	
	return value;
}

static GtranslatorSpellChecker *
get_spell_checker_from_tab (GtranslatorTab *tab)
{
	GtranslatorSpellChecker *spell;
	gpointer data;
	gboolean value;
	
	g_return_val_if_fail (tab != NULL, NULL);

	data = g_object_get_qdata (G_OBJECT (tab), spell_checker_id);

	if (data == NULL)
	{
		GtranslatorProfile *profile;

		spell = gtranslator_spell_checker_new ();

		profile = gtranslator_application_get_active_profile (GTR_APP);

		if (profile != NULL)
		{
			const gchar *key;
			
			key = gtranslator_profile_get_language_code (profile);
			
			value = gtranslator_spell_checker_set_language (spell, key);

		}

		g_object_set_qdata_full (G_OBJECT (tab), 
					 spell_checker_id, 
					 spell, 
					 (GDestroyNotify) g_object_unref);

	}
	else
	{
		g_return_val_if_fail (GTRANSLATOR_IS_SPELL_CHECKER (data), NULL);
		spell = GTRANSLATOR_SPELL_CHECKER (data);
	}

	return spell;
}

static void
auto_spell_cb (GtkAction   *action,
	       GtranslatorWindow *window
			  )
{
	
	GtranslatorAutomaticSpellChecker *autospell;
	GtranslatorTab *tab;
	GtkTextBuffer *doc;
	GtranslatorView *view;
	GtranslatorSpellChecker *spell;
	gboolean active;

	tab = gtranslator_window_get_active_tab (window);
	
	spell = get_spell_checker_from_tab (tab);
	
	active = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action));
	
	g_return_if_fail (spell != NULL);
	
	view = gtranslator_window_get_active_view (window);
	
	doc = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
	
	autospell = gtranslator_automatic_spell_checker_get_from_tab (tab);
	
	if (active)
	{
		if (autospell == NULL)
		{
			GtranslatorView *active_view;

			active_view = gtranslator_window_get_active_view (window);
			g_return_if_fail (active_view != NULL);

			autospell = gtranslator_automatic_spell_checker_new (doc, tab, spell);
			gtranslator_automatic_spell_checker_recheck_all (autospell);
		} else { 
			gtranslator_automatic_spell_checker_recheck_all (autospell);
		}
	}
	else
	{
		if (autospell != NULL)
			gtranslator_automatic_spell_checker_free (autospell);
	}
}


static gboolean
go_to_next_message ()
{
	GtranslatorWindow *window;
	GtranslatorTab *tab;
	GtranslatorPo *po;
	
	window = gtranslator_application_get_active_window (GTR_APP);
	g_return_val_if_fail (window != NULL, FALSE);
	
	tab = gtranslator_window_get_active_tab (window);
	g_return_val_if_fail (tab != NULL, FALSE);
	
	po = gtranslator_tab_get_po (tab);
	g_return_val_if_fail (po != NULL, FALSE);
	
	if (gtranslator_po_get_messages_count (po) != gtranslator_po_get_message_position (po))
	{
		GtranslatorView *view;
		GtkTextBuffer *buffer;
		GtkTextIter iter;
		
		gtranslator_tab_go_to_next (tab);
		set_sensitive_according_to_message (window, po);
		/* Move current_mark to start */
		view = gtranslator_window_get_active_view (window);
		buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
		g_return_val_if_fail (buffer != NULL, FALSE);
		
		gtk_text_buffer_get_start_iter (buffer, &iter);
		gtk_text_buffer_move_mark (buffer, 
								   current_mark,
								   &iter);
		
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

gchar *
get_next_word (GtranslatorSpellCheckerDialog *dlg,
			   GtranslatorView *view)
{
	GtkTextIter current_iter;
	GtkTextIter start_iter, end_iter;
	GtkTextIter old_iter;
	GtkTextBuffer *buffer;
	GtkTextView *dlg_view;
	GtkTextBuffer *dlg_buffer;
	gchar *buffer_text;
	
	g_return_val_if_fail (view != NULL, NULL);
	g_return_val_if_fail (dlg != NULL, NULL);

	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
	g_return_val_if_fail (buffer != NULL, NULL);
	
	dlg_view = gtranslator_spell_checker_dialog_get_view (dlg);
	g_return_val_if_fail (dlg_view != NULL, NULL);
	
	dlg_buffer = gtk_text_view_get_buffer (dlg_view);
	g_return_val_if_fail (dlg_buffer != NULL, NULL);
	
	gtk_text_buffer_get_iter_at_mark (buffer,
									  &current_iter,
									  current_mark);
	gtk_text_buffer_get_bounds (buffer,
								&start_iter,
								&end_iter);
	
	/* Get buffer text for dlg message preview */
	buffer_text = gtk_text_buffer_get_text (buffer,
											&start_iter,
											&end_iter,
											FALSE);
	
	gtk_text_iter_backward_word_start (&end_iter);
	gtk_text_iter_forward_word_end (&end_iter);
	
	if (gtk_text_iter_compare (&current_iter, &end_iter) == 0)
	{
		gchar *new_word;
		if (!go_to_next_message ())
			return NULL;
		
		return get_next_word (dlg, view);
	}
	
	old_iter = current_iter;
	gtk_text_iter_forward_word_end (&current_iter);
	if (gtk_text_iter_compare (&current_iter, &old_iter) == 0)
	{
		gtk_text_buffer_move_mark (buffer,
								   current_mark,
								   &current_iter);
		return NULL;
	}
	old_iter = current_iter;
	gtk_text_iter_backward_word_start (&current_iter);
	gtk_text_buffer_move_mark (buffer,
							   current_mark,
							   &old_iter);
	/* Select the word */
	gtk_text_buffer_select_range (buffer,
								  &current_iter,
								  &old_iter);
	
	/* Update dlg message_preview */
	gtk_text_buffer_begin_user_action (dlg_buffer);
	gtk_text_buffer_set_text (dlg_buffer,
							  buffer_text,
							  -1);
	/* Remove all tags on dlg_buffer and add current word buffer */
	gtk_text_buffer_get_bounds (dlg_buffer,
								&start_iter,
								&end_iter);
	gtk_text_buffer_remove_all_tags (dlg_buffer,
									&start_iter,
									&end_iter);
	gtk_text_buffer_get_iter_at_offset (dlg_buffer,
										&start_iter,
										gtk_text_iter_get_offset (&current_iter));
	gtk_text_buffer_get_iter_at_offset (dlg_buffer,
										&end_iter,
										gtk_text_iter_get_offset (&old_iter));
	gtk_text_buffer_apply_tag_by_name (dlg_buffer,
									   "high_light",
									   &start_iter,
									   &end_iter);
	gtk_text_view_scroll_to_iter (dlg_view, 
								  &start_iter,
								  0.0,
								  TRUE,
								  0.0,
								  0.5);
	
	g_free (buffer_text);
							  
	return gtk_text_buffer_get_slice (buffer,
									  &current_iter,
									  &old_iter,
									  TRUE);
}
	

gchar *
get_next_misspelled_word (GtranslatorSpellCheckerDialog *dlg,
						  GtranslatorView *view)
{
	gchar *word;
	GtranslatorSpellChecker *spell;
	
	g_return_val_if_fail (view != NULL, NULL);

	word = get_next_word (dlg, view);
	spell = gtranslator_spell_checker_dialog_get_spell_checker (GTRANSLATOR_SPELL_CHECKER_DIALOG (dlg));
	g_return_val_if_fail (spell != NULL, NULL);
	
	while (gtranslator_spell_checker_check_word (spell, word, -1)) 
	{
		g_free (word);
		word = get_next_word (dlg, view);
	}
	
	if (word == NULL)
		gtranslator_spell_checker_dialog_set_completed (dlg);
	
	return word;
	
}

static void
ignore_cb (GtranslatorSpellCheckerDialog *dlg,
		   const gchar *w,
		   GtranslatorView *view)
{
	gchar *word = NULL;
	GtranslatorSpellChecker *spell;

	g_return_if_fail (w != NULL);
	g_return_if_fail (view != NULL);
	
	spell = gtranslator_spell_checker_dialog_get_spell_checker (dlg);
	
	word = get_next_misspelled_word (dlg, view);
	
	
	gtranslator_spell_checker_dialog_set_misspelled_word (GTRANSLATOR_SPELL_CHECKER_DIALOG (dlg),
							word,
							-1);
	g_free (word);
		
}

static void
ignore_all_cb (GtranslatorSpellCheckerDialog *dlg,
			   const gchar *w,
			   GtranslatorView *view)
{
	gchar *word = NULL;
	GtranslatorSpellChecker *spell;
	
	g_return_if_fail (w != NULL);
	g_return_if_fail (view != NULL);
	
	spell = gtranslator_spell_checker_dialog_get_spell_checker (dlg);
	
	/* Add word to current session */
	gtranslator_spell_checker_add_word_to_session (spell,
												   w,
												   -1);
	
	word = get_next_misspelled_word (dlg, view);
	
	gtranslator_spell_checker_dialog_set_misspelled_word (GTRANSLATOR_SPELL_CHECKER_DIALOG (dlg),
														  word,
														  -1);
	g_free (word);
}

static void
check_spelling_cb (GtkAction	*action,
				   GtranslatorWindow	*window)
{
	GtranslatorView *view;
	GtranslatorTab *tab;
	GtkTextBuffer *buffer;
	GtranslatorSpellChecker *spell;
	GtkWidget *dlg;
	gint start, end;
	GtkTextIter sel_start, sel_end;
	gchar *word;
	gboolean sel = FALSE;
	
	view = gtranslator_window_get_active_view (window);
	g_return_if_fail (view != NULL);
	
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
	g_return_if_fail (buffer != NULL);
	
	tab = gtranslator_window_get_active_tab (window);
	g_return_if_fail (tab != NULL);
	
	spell = get_spell_checker_from_tab (tab);
	g_return_if_fail (spell != NULL);
		
	dlg = gtranslator_spell_checker_dialog_new_from_spell_checker (spell);
	gtk_window_set_modal (GTK_WINDOW (dlg), TRUE);
	gtk_window_set_transient_for (GTK_WINDOW (dlg), GTK_WINDOW (window));
	
	word = get_next_misspelled_word (GTRANSLATOR_SPELL_CHECKER_DIALOG(dlg), 
									 view);
	gtranslator_spell_checker_dialog_set_misspelled_word (GTRANSLATOR_SPELL_CHECKER_DIALOG (dlg),
							word,
							-1);
	g_signal_connect (dlg, 
					  "ignore", 
					  G_CALLBACK (ignore_cb), 
					  view);
	g_signal_connect (dlg, 
					  "ignore_all", 
					  G_CALLBACK (ignore_all_cb), 
					  view);
	
	gtk_widget_show (dlg);
}

static void
free_window_data (WindowData *data)
{
	g_return_if_fail (data != NULL);

	g_object_unref (data->action_group);
	g_free (data);
}


static void
update_ui_real (GtranslatorWindow *window,
		WindowData *data)
{
	GtranslatorTab *tab;
	GtranslatorView *view;
	gboolean autospell;
	GtkAction *action;

	tab = gtranslator_window_get_active_tab (window);
	
	view = gtranslator_window_get_active_view (window);
	
	autospell = (tab != NULL &&
	             gtranslator_automatic_spell_checker_get_from_tab (tab) != NULL);
	action = gtk_action_group_get_action (data->action_group, "AutoSpell");
	gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), autospell);

	gtk_action_group_set_sensitive (data->action_group,
					(view != NULL) &&
					gtk_text_view_get_editable (GTK_TEXT_VIEW (view)));
}


static void
showed_message_cb (GtranslatorTab *tab,
				   GtranslatorMsg *msg,
		   		GtranslatorWindow *window)
{
	GtkTextBuffer *buffer;
	GtranslatorView *view;
	GtkTextIter iter;
	
	GtranslatorAutomaticSpellChecker *autospell;
	
	autospell = gtranslator_automatic_spell_checker_get_from_tab (tab);
	view = gtranslator_tab_get_active_view (tab);
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
	gtk_text_buffer_get_start_iter (buffer, &iter);
	
	if (current_mark == NULL) 
		current_mark = gtk_text_buffer_create_mark (buffer, NULL, &iter, TRUE);
	else
		gtk_text_buffer_move_mark (buffer,
								   current_mark,
								   &iter);
	
	if (autospell != NULL) 
	{
		gtranslator_automatic_spell_checker_recheck_all (autospell);
	}
	
}
	
			   

static void
page_added_cb (GtkNotebook *notebook,
	       GtkWidget   *child,
	       guint        page_num,
	       GtranslatorWindow *window)
{	
 
	g_signal_connect (child, "showed-message",
			  G_CALLBACK (showed_message_cb), window); 
	
}

static void
impl_activate (GtranslatorPlugin *plugin,
			   GtranslatorWindow *window) 
{ 
	
	GtkUIManager *manager;
	WindowData *data;

	data = g_new (WindowData, 1);

	manager = gtranslator_window_get_ui_manager (window);
	
	data->action_group = gtk_action_group_new ("GtranslatorSpellPluginActions");
	gtk_action_group_set_translation_domain (data->action_group, 
						 GETTEXT_PACKAGE);
	gtk_action_group_add_actions (data->action_group,
						 action_entries,
						 G_N_ELEMENTS (action_entries),
						 window);
	gtk_action_group_add_toggle_actions (data->action_group, 
					     toggle_action_entries,
					     G_N_ELEMENTS (toggle_action_entries),
					     window);

	gtk_ui_manager_insert_action_group (manager, data->action_group, -1);

	data->ui_id = gtk_ui_manager_new_merge_id (manager);

	data->message_cid = gtk_statusbar_get_context_id
			(GTK_STATUSBAR (gtranslator_window_get_statusbar (window)), 
			 "spell_plugin_message");

	g_object_set_data_full (G_OBJECT (window),
				WINDOW_DATA_KEY, 
				data,
				(GDestroyNotify) free_window_data);

	gtk_ui_manager_add_ui (manager, 
			       data->ui_id, 
			       "/MainMenu/EditMenu/EditOps_1/",
			       "AutoSpell", 
			       "AutoSpell",
			       GTK_UI_MANAGER_MENUITEM, 
			       FALSE);

	gtk_ui_manager_add_ui (manager,
				   data->ui_id,
				   "/MainMenu/EditMenu/EditOps_1/",
				   "CheckSpelling",
				   "CheckSpelling",
				   GTK_UI_MANAGER_MENUITEM,
				   FALSE);

	GtranslatorNotebook *notebook;

	notebook = gtranslator_window_get_notebook (window);
	
	
	g_signal_connect (GTK_NOTEBOOK (notebook),
			  "page-added",
			  G_CALLBACK (page_added_cb), window);
		 
/*	
	tabs = gtranslator_window_get_all_tabs (window);
				
				
	if(tabs == NULL)
		return;
	do{
		g_signal_connect (tabs->data, "showed-message",
				  G_CALLBACK (auto_spell_cb), window);
	}while((tabs = g_list_next(tabs)));

*/			
	update_ui_real (window, data);			
}


static void
impl_deactivate	(GtranslatorPlugin *plugin,
		 GtranslatorWindow *window)
{
	GtkUIManager *manager;
	WindowData *data;

	manager = gtranslator_window_get_ui_manager (window);

	data = (WindowData *) g_object_get_data (G_OBJECT (window), WINDOW_DATA_KEY);
	g_return_if_fail (data != NULL);

	gtk_ui_manager_remove_ui (manager, data->ui_id);
	gtk_ui_manager_remove_action_group (manager, data->action_group);

	g_object_set_data (G_OBJECT (window), WINDOW_DATA_KEY, NULL);
}


static void
impl_update_ui (GtranslatorPlugin *plugin,
		GtranslatorWindow *window)
{
	WindowData *data;

	data = (WindowData *) g_object_get_data (G_OBJECT (window), WINDOW_DATA_KEY);
	g_return_if_fail (data != NULL);

	update_ui_real (window, data);
}

static void
gtranslator_spell_plugin_class_init (GtranslatorSpellPluginClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtranslatorPluginClass *plugin_class = GTR_PLUGIN_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtranslatorSpellPluginPrivate));

	
	if (spell_checker_id == 0)
		spell_checker_id = g_quark_from_static_string ("GtranslatorSpellCheckerID");
	
	object_class->finalize = gtranslator_spell_plugin_finalize;
    plugin_class->activate = impl_activate;
	plugin_class->deactivate = impl_deactivate;
	plugin_class->update_ui = impl_update_ui;


}
