/* vim: set sw=8: -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * gtranslator-spell-checker.c
 * This file is part of gtranslator
 *
 * Copyright (C) 2008 Baris Cicek <baris@teamforce.name.tr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */


/* This is a modified version of gedit-spell-checker.c */
/* included in gedit distribution in GNOME
 *  * Copyright (c) 2002 Paolo Maggi.
 *   */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include <enchant/enchant.h>

#include <glib/gi18n.h>
#include <glib/gstrfuncs.h>

#include "gtranslator-spell-checker.h"

/* FIXME - Rename the marshal file - Paolo */

struct _GtranslatorSpellChecker
{
	GObject parent_instance;

	EnchantDict                     *dict;
	EnchantBroker                   *broker;
	
	gchar				*active_lang;
};

/* GObject properties */
enum {
	PROP_0 = 0,
	PROP_LANGUAGE,
	LAST_PROP
};

/* Signals */
enum {
	ADD_WORD_TO_PERSONAL = 0,
	ADD_WORD_TO_SESSION,
	SET_LANGUAGE,
	CLEAR_SESSION,
	LAST_SIGNAL
};

static void	gtranslator_spell_checker_class_init 	(GtranslatorSpellCheckerClass *klass);

static void	gtranslator_spell_checker_init 	(GtranslatorSpellChecker      *spell_checker);
static void 	gtranslator_spell_checker_finalize 	(GObject                *object);

static gboolean is_digit 			(const char             *text,
						 gssize                  length);


static GObjectClass *parent_class = NULL;

static guint signals[LAST_SIGNAL] = { 0 };

GType
gtranslator_spell_checker_get_type (void)
{
	static GType gtranslator_spell_checker_type = 0;

	if(!gtranslator_spell_checker_type)
	{
		static const GTypeInfo gtranslator_spell_checker_info =
		{
			sizeof (GtranslatorSpellCheckerClass),
			NULL, /* base init */
			NULL, /* base finalize */
			(GClassInitFunc) gtranslator_spell_checker_class_init, /* class init */
			NULL, /* class finalize */
			NULL, /* class data */
			sizeof (GtranslatorSpellChecker),
			0,
			(GInstanceInitFunc) gtranslator_spell_checker_init
		};

		gtranslator_spell_checker_type = g_type_register_static (G_TYPE_OBJECT,
							"GtranslatorSpellChecker",
							&gtranslator_spell_checker_info, 0);
	}

	return gtranslator_spell_checker_type;
}

static void
gtranslator_spell_checker_set_property (GObject *object,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *pspec)
{
	/*
	GtranslatorSpellChecker *spell = GTRANSLATOR_SPELL_CHECKER (object);
	*/

	switch (prop_id)
	{
		case PROP_LANGUAGE:
			/* TODO */
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

static void
gtranslator_spell_checker_get_property (GObject *object,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *pspec)
{
	/*
	GtranslatorSpellChecker *spell = GTRANSLATOR_SPELL_CHECKER (object);
	*/

	switch (prop_id)
	{
		case PROP_LANGUAGE:
			/* TODO */
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

static void
gtranslator_spell_checker_class_init (GtranslatorSpellCheckerClass * klass)
{
	GObjectClass *object_class;

	object_class = G_OBJECT_CLASS (klass);

	parent_class = g_type_class_peek_parent (klass);

	object_class->set_property = gtranslator_spell_checker_set_property;
	object_class->get_property = gtranslator_spell_checker_get_property;

	object_class->finalize = gtranslator_spell_checker_finalize;

	g_object_class_install_property (object_class,
					 PROP_LANGUAGE,
					 g_param_spec_pointer ("language",
						 	      "Language",
							      "The language used by the spell checker",
							      G_PARAM_READWRITE));

}

static void
gtranslator_spell_checker_init (GtranslatorSpellChecker *spell_checker)
{
	spell_checker->broker = enchant_broker_init ();
	spell_checker->dict = NULL;
}

GtranslatorSpellChecker *
gtranslator_spell_checker_new	(void)
{
	GtranslatorSpellChecker *spell;

	spell = GTRANSLATOR_SPELL_CHECKER (g_object_new (GTRANSLATOR_TYPE_SPELL_CHECKER,NULL));
	
	g_return_val_if_fail (spell != NULL, NULL);

	return spell;
}

static void
gtranslator_spell_checker_finalize (GObject *object)
{
	GtranslatorSpellChecker *spell_checker;

	g_return_if_fail (GTRANSLATOR_IS_SPELL_CHECKER (object));

	spell_checker = GTRANSLATOR_SPELL_CHECKER (object);


	if (spell_checker->broker != NULL)
		enchant_broker_free (spell_checker->broker);
}

static gboolean
lazy_init (GtranslatorSpellChecker               *spell,
	   gchar *language)
{
	if (spell->dict != NULL)
		return TRUE;

	g_return_val_if_fail (spell->broker != NULL, FALSE);



	/* Second try to get a default language */
	spell->active_lang = g_strdup(language);
	g_warning ("Setting active language as %s", language);
	spell->dict = enchant_broker_request_dict (spell->broker, 
						  language);	
	
	if (spell->dict == NULL)
	{
		spell->active_lang = NULL;
		
		if (language != NULL)
			g_warning ("Spell checker plugin: cannot select a default language.");

		return FALSE;
	}
	return TRUE;
}


gboolean
gtranslator_spell_checker_check_word (GtranslatorSpellChecker *spell,
				const gchar       *word,
				gssize             len)
{
	gint enchant_result;
	gboolean res = FALSE;

	g_return_val_if_fail (GTRANSLATOR_IS_SPELL_CHECKER (spell), FALSE);
	g_return_val_if_fail (word != NULL, FALSE);

	if (!lazy_init (spell, spell->active_lang))
		return FALSE;

	if (len < 0)
		len = strlen (word);

	if (strcmp (word, "gtranslator") == 0)
		return TRUE;

	if (is_digit (word, len))
		return TRUE;

	g_return_val_if_fail (spell->dict != NULL, FALSE);
	enchant_result = enchant_dict_check (spell->dict, word, len);

	switch (enchant_result)
	{
		case -1:
			/* error */
			res = FALSE;

			g_warning ("Spell checker plugin: error checking word '%s' (%s).",
				   word, enchant_dict_get_error (spell->dict));

			break;
		case 1:
			/* it is not in the directory */
			res = FALSE;
			break;
		case 0:
			/* is is in the directory */
			res = TRUE;
			break;
		default:
			g_return_val_if_reached (FALSE);
	}

	return res;
}


/* return NULL on error or if no suggestions are found */
GSList *
gtranslator_spell_checker_get_suggestions (GtranslatorSpellChecker *spell,
				     const gchar       *word,
				     gssize             len)
{
	gchar **suggestions;
	size_t n_suggestions = 0;
	GSList *suggestions_list = NULL;
	gint i;

	g_return_val_if_fail (GTRANSLATOR_IS_SPELL_CHECKER (spell), NULL);
	g_return_val_if_fail (word != NULL, NULL);

	if (!lazy_init (spell, spell->active_lang))
		return NULL;

	g_return_val_if_fail (spell->dict != NULL, NULL);

	if (len < 0)
		len = strlen (word);

	suggestions = enchant_dict_suggest (spell->dict, word, len, &n_suggestions);

	if (n_suggestions == 0)
		return NULL;

	g_return_val_if_fail (suggestions != NULL, NULL);

	for (i = 0; i < (gint)n_suggestions; i++)
	{
		suggestions_list = g_slist_prepend (suggestions_list,
						    suggestions[i]);
	}

	/* The single suggestions will be freed by the caller */
	g_free (suggestions);

	suggestions_list = g_slist_reverse (suggestions_list);

	return suggestions_list;
}

gboolean
gtranslator_spell_checker_add_word_to_personal (GtranslatorSpellChecker *spell,
					  const gchar       *word,
					  gssize             len)
{
	g_return_val_if_fail (GTRANSLATOR_IS_SPELL_CHECKER (spell), FALSE);
	g_return_val_if_fail (word != NULL, FALSE);

	if (!lazy_init (spell, spell->active_lang))
		return FALSE;

	g_return_val_if_fail (spell->dict != NULL, FALSE);

	if (len < 0)
		len = strlen (word);

	enchant_dict_add_to_pwl (spell->dict, word, len);

	g_signal_emit (G_OBJECT (spell), signals[ADD_WORD_TO_PERSONAL], 0, word, len);

	return TRUE;
}

gboolean
gtranslator_spell_checker_add_word_to_session (GtranslatorSpellChecker *spell,
					 const gchar       *word,
					 gssize             len)
{
	g_return_val_if_fail (GTRANSLATOR_IS_SPELL_CHECKER (spell), FALSE);
	g_return_val_if_fail (word != NULL, FALSE);

	if (!lazy_init (spell, NULL))
		return FALSE;

	g_return_val_if_fail (spell->dict != NULL, FALSE);

	if (len < 0)
		len = strlen (word);

	enchant_dict_add_to_session (spell->dict, word, len);

	g_signal_emit (G_OBJECT (spell), signals[ADD_WORD_TO_SESSION], 0, word, len);

	return TRUE;
}

gboolean
gtranslator_spell_checker_clear_session (GtranslatorSpellChecker *spell)
{
	g_return_val_if_fail (GTRANSLATOR_IS_SPELL_CHECKER (spell), FALSE);

	/* free and re-request dictionary */

	if (!lazy_init (spell, spell->active_lang))
		return FALSE;

	g_signal_emit (G_OBJECT (spell), signals[CLEAR_SESSION], 0);

	return TRUE;
}

/*
 * Informs dictionary, that word 'word' will be replaced/corrected by word
 * 'replacement'
 */
gboolean
gtranslator_spell_checker_set_correction (GtranslatorSpellChecker *spell,
				    const gchar       *word,
				    gssize             w_len,
				    const gchar       *replacement,
				    gssize             r_len)
{
	g_return_val_if_fail (GTRANSLATOR_IS_SPELL_CHECKER (spell), FALSE);
	g_return_val_if_fail (word != NULL, FALSE);
	g_return_val_if_fail (replacement != NULL, FALSE);

	if (!lazy_init (spell, NULL))
		return FALSE;

	g_return_val_if_fail (spell->dict != NULL, FALSE);

	if (w_len < 0)
		w_len = strlen (word);

	if (r_len < 0)
		r_len = strlen (replacement);

	enchant_dict_store_replacement (spell->dict,
					word,
					w_len,
					replacement,
					r_len);

	return TRUE;
}

static gboolean
is_digit (const char *text, gssize length)
{
	gunichar c;
	const gchar *p;
 	const gchar *end;

	g_return_val_if_fail (text != NULL, FALSE);

	if (length < 0)
		length = strlen (text);

	p = text;
	end = text + length;

	while (p != end) {
		const gchar *next;
		next = g_utf8_next_char (p);

		c = g_utf8_get_char (p);

		if (!g_unichar_isdigit (c) && c != '.' && c != ',')
			return FALSE;

		p = next;
	}

	return TRUE;
}

gboolean
gtranslator_spell_checker_set_language (GtranslatorSpellChecker               *spell,
				  const gchar *language)
{
	gboolean ret;

	g_return_val_if_fail (GTRANSLATOR_IS_SPELL_CHECKER (spell), FALSE);

	if (spell->dict != NULL)
	{
		enchant_broker_free_dict (spell->broker, spell->dict);
		spell->dict = NULL;
	}

	ret = lazy_init (spell, language);

	if (ret) {
	//	g_signal_emit (G_OBJECT (spell), signals[SET_LANGUAGE], 0, language);
	}
	else
		g_warning ("Spell checker plugin: cannot use language %s.",
		           language);

	return ret;
}
