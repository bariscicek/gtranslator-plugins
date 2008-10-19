/* vim: set sw=8: -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * gtranslator-spell-checker.h
 * This file is part of gtranslator
 *
 * Copyright (C) 2008 Baris Cicek
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

/* This is a modified version of gedit-spell-checker.h */
/* included in gedit distribution in GNOME
 *  * Copyright (c) 2002 Paolo Maggi.
 *   */



#ifndef __GTRANSLATOR_SPELL_CHECKER_H__
#define __GTRANSLATOR_SPELL_CHECKER_H__

#include <glib.h>
#include <glib-object.h>


G_BEGIN_DECLS

#define GTRANSLATOR_TYPE_SPELL_CHECKER            (gtranslator_spell_checker_get_type ())
#define GTRANSLATOR_SPELL_CHECKER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTRANSLATOR_TYPE_SPELL_CHECKER, GtranslatorSpellChecker))
#define GTRANSLATOR_SPELL_CHECKER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GTRANSLATOR_TYPE_SPELL_CHECKER, GtranslatorSpellChecker))
#define GTRANSLATOR_IS_SPELL_CHECKER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTRANSLATOR_TYPE_SPELL_CHECKER))
#define GTRANSLATOR_IS_SPELL_CHECKER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTRANSLATOR_TYPE_SPELL_CHECKER))
#define GTRANSLATOR_SPELL_CHECKER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GTRANSLATOR_TYPE_SPELL_CHECKER, GtranslatorSpellChecker))

typedef struct _GtranslatorSpellChecker GtranslatorSpellChecker;

typedef struct _GtranslatorSpellCheckerClass GtranslatorSpellCheckerClass;

struct _GtranslatorSpellCheckerClass
{
	GObjectClass parent_class;

	/* Signals */
	void (*add_word_to_personal) (GtranslatorSpellChecker               *spell,
				      const gchar                     *word,
				      gint                             len);

	void (*add_word_to_session)  (GtranslatorSpellChecker               *spell,
				      const gchar                     *word,
				      gint                             len);


	void (*clear_session)	     (GtranslatorSpellChecker               *spell);
};


GType        		 gtranslator_spell_checker_get_type		(void) G_GNUC_CONST;

/* Constructors */
GtranslatorSpellChecker	*gtranslator_spell_checker_new		(void);

gboolean		 gtranslator_spell_checker_check_word 	(GtranslatorSpellChecker               *spell,
								 const gchar                     *word,
								 gssize                           len);

GSList 			*gtranslator_spell_checker_get_suggestions 	(GtranslatorSpellChecker               *spell,
								 const gchar                     *word,
								 gssize                           len);

gboolean		 gtranslator_spell_checker_add_word_to_personal
								(GtranslatorSpellChecker               *spell,
								 const gchar                     *word,
								 gssize                           len);

gboolean		 gtranslator_spell_checker_add_word_to_session
								(GtranslatorSpellChecker               *spell,
								 const gchar                     *word,
								 gssize                           len);

gboolean		 gtranslator_spell_checker_clear_session 	(GtranslatorSpellChecker               *spell);

gboolean		 gtranslator_spell_checker_set_correction 	(GtranslatorSpellChecker               *spell,
								 const gchar                     *word,
								 gssize                           w_len,
								 const gchar                     *replacement,
								 gssize                           r_len);

gboolean			gtranslator_spell_checker_set_language (GtranslatorSpellChecker               *spell,
									const gchar *language);
G_END_DECLS

#endif  /* __GTRANSLATOR_SPELL_CHECKER_H__ */

