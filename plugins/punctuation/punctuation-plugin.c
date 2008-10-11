/*
 * Copyright (C) 2008  Baris Cicek  <baris@teamforce.name.tr>
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
 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "punctuation-plugin.h"
#include <gtranslator/msg.h>
#include <gtranslator/notebook.h>
#include <gtranslator/window.h>

#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include <string.h>

GTR_PLUGIN_REGISTER_TYPE(GtranslatorPunctuationPlugin, gtranslator_punctuation_plugin)

static void
gtranslator_punctuation_plugin_init (GtranslatorPunctuationPlugin *message_table)
{
}

static void
gtranslator_punctuation_plugin_finalize (GObject *object)
{
	G_OBJECT_CLASS (gtranslator_punctuation_plugin_parent_class)->finalize (object);
}


static gchar *
strip_tags (gchar *text) 
{ 
	gchar *new_string;
	gint len, i = 0;
	gint alloc_size;
	gboolean in_tag = FALSE;
	
	len = alloc_size = strlen (text);
	
	if (len < 2)
		return text;
	
	
	for (i = 0; i < len; i++) {
		if (text[i] >= 128) 
			continue;
		
		if (text[i] == '<') 
			in_tag = TRUE;
		else if (text[i] == '>')
		{
			alloc_size--;
			in_tag = FALSE;
		}
		
		if (in_tag)
			alloc_size--;
	}
	
	// if it's still in_tag or it's whole tag then it's a broken tag don't touch the string
	if (in_tag || alloc_size == 0)
		return text;
		
	
	new_string = g_malloc0 (alloc_size*2);
	in_tag = FALSE;
	
	for (i = 0, alloc_size = 0; i < len; i++) {
		if (text[i] == '<')
		{
			in_tag = TRUE;
			continue;
		} 
		else if (text[i] == '>')
		{
			in_tag = FALSE;
			continue;
		}
		
		if (in_tag)
			continue;
		
		new_string[alloc_size++] = text[i];
		// if it's utf8 char add next byte as well		
		if (text[i] >= 128)
		{ 
			new_string[alloc_size++] = text[i++];
		}
		
	}
	
	g_free (text);
	
	return new_string;
}
	

static void
showed_message_cb (GtranslatorTab *tab,
		   GtranslatorMsg *msg,
		   GtranslatorWindow *window)
{
	gchar *msgid, *msgstr;
	GtranslatorView *view;
	GtkTextBuffer *buffer;
	GtkTextTag *tag;
	GtkTextIter start, end;
	gboolean treedot = FALSE;
	gchar end_of_msgid , end_of_msgstr;
	gboolean mismatch_punct = FALSE;
	
	view = gtranslator_window_get_active_view (window);
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(view));
	gtk_text_buffer_get_bounds (buffer, &start, &end);
	
	msgid = g_strdup(gtranslator_msg_get_msgid (msg));
	msgstr = g_strdup(gtk_text_buffer_get_text (buffer, &start, &end, FALSE));
	
	msgid = strip_tags (msgid);
	msgstr = strip_tags (msgstr);

	if (strlen (msgid) < 2 || strlen (msgstr) < 2)
		return;
	
	end_of_msgid = msgid[strlen (msgid) - 1];
	end_of_msgstr = msgstr[strlen (msgstr) -1];
	
	if (strlen (msgid) > 3) {
		if (end_of_msgid == '.' && msgid[strlen (msgid) - 2] == '.' && msgid [strlen (msgid) - 3] == '.')
			treedot = TRUE;
	}
	
		
	// if the last char is not punctuation return we have nothing to do
	if (!g_ascii_ispunct (end_of_msgid) ||
		end_of_msgid == '\'' ||
		end_of_msgid == '\"' || 
		end_of_msgid == ')' ||
		end_of_msgid == ']' ||
		end_of_msgid == '}')	
		return;

	if ((tag=gtk_text_tag_table_lookup (gtk_text_buffer_get_tag_table (buffer), "red_foreground")) == NULL) 
		tag = gtk_text_buffer_create_tag (buffer, "red_foreground", "foreground", "red", NULL);
	
	if (end_of_msgid != end_of_msgstr)
				mismatch_punct = TRUE;
	// Check if last three chars are dots to see both strings ends with treedot
	else if (treedot) 
	{
		if (strlen (msgstr) < 3)
			mismatch_punct = TRUE;
		else
		{
			if (end_of_msgstr != '.' || msgstr[strlen (msgstr) - 2] != '.' || msgstr[strlen (msgstr) -3] != '.')
				mismatch_punct = TRUE;
		}
	}

	if (mismatch_punct) {
		gtk_text_buffer_apply_tag_by_name (buffer, "red_foreground", &start, &end);
	} else {
		gtk_text_buffer_remove_tag_by_name (buffer, "red_foreground", &start, &end);
	}
	
	g_free (msgid);
	g_free (msgstr);
}


static void
page_added_cb (GtkNotebook *notebook,
	       GtkWidget   *child,
	       guint        page_num,
	       GtranslatorWindow *window)
{	
	
	g_signal_connect (child, 
					 "message-changed",
					  G_CALLBACK(showed_message_cb), 
					  window);
	
	g_signal_connect (child, "showed-message",
			  G_CALLBACK (showed_message_cb), window);
}

static void
impl_activate (GtranslatorPlugin *plugin,
	       GtranslatorWindow *window)
{
	GtranslatorNotebook *notebook;

	GList *tabs = NULL;
	
	g_return_if_fail (GTR_IS_WINDOW (window));
	
	notebook = gtranslator_window_get_notebook (window);

	g_signal_connect (GTK_NOTEBOOK (notebook),
			  "page-added",
			  G_CALLBACK (page_added_cb), window);
			 
	tabs = gtranslator_window_get_all_tabs (window);
	
	if(tabs == NULL)
		return;
	do {
		g_signal_connect (tabs->data, "showed-message",
				  G_CALLBACK (showed_message_cb), window);
	} while((tabs = g_list_next(tabs)));
	
}

static void
impl_deactivate (GtranslatorPlugin *plugin,
				 GtranslatorWindow *window)
{
	GtranslatorNotebook *notebook;
	notebook = gtranslator_window_get_notebook(window);
	
	g_signal_handlers_disconnect_by_func(notebook,
					     page_added_cb,
					     window);
}

static void
gtranslator_punctuation_plugin_class_init (GtranslatorPunctuationPluginClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtranslatorPluginClass *plugin_class = GTR_PLUGIN_CLASS (klass);

	object_class->finalize = gtranslator_punctuation_plugin_finalize;

	plugin_class->activate = impl_activate;
	plugin_class->deactivate = impl_deactivate;
	
}

