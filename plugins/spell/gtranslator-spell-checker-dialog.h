/* vim: set sw=8: -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * gtranslator-spell-checker-dialog.h
 * This file is part of gtranslator-plugins
 *
 * Copyright (C) 2002 Paolo Maggi 
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
 
/*
 * Modified by the gtranslator Team, 2002. See the AUTHORS file for a 
 * list of people on the gtranslator-plugins Team.  
 * See the ChangeLog files for a list of changes. 
 */

#ifndef __GTRANSLATOR_SPELL_CHECKER_DIALOG_H__
#define __GTRANSLATOR_SPELL_CHECKER_DIALOG_H__

#include <gtk/gtkwindow.h>
#include "gtranslator-spell-checker.h"

G_BEGIN_DECLS

#define GTRANSLATOR_TYPE_SPELL_CHECKER_DIALOG            (gtranslator_spell_checker_dialog_get_type ())
#define GTRANSLATOR_SPELL_CHECKER_DIALOG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTRANSLATOR_TYPE_SPELL_CHECKER_DIALOG, GtranslatorSpellCheckerDialog))
#define GTRANSLATOR_SPELL_CHECKER_DIALOG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GTRANSLATOR_TYPE_SPELL_CHECKER_DIALOG, GtranslatorSpellCheckerDialog))
#define GTRANSLATOR_IS_SPELL_CHECKER_DIALOG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTRANSLATOR_TYPE_SPELL_CHECKER_DIALOG))
#define GTRANSLATOR_IS_SPELL_CHECKER_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTRANSLATOR_TYPE_SPELL_CHECKER_DIALOG))
#define GTRANSLATOR_SPELL_CHECKER_DIALOG_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GTRANSLATOR_TYPE_SPELL_CHECKER_DIALOG, GtranslatorSpellCheckerDialog))


typedef struct _GtranslatorSpellCheckerDialog GtranslatorSpellCheckerDialog;

typedef struct _GtranslatorSpellCheckerDialogClass GtranslatorSpellCheckerDialogClass;

struct _GtranslatorSpellCheckerDialogClass 
{
	GtkWindowClass parent_class;

	/* Signals */
	void		(*ignore)		(GtranslatorSpellCheckerDialog *dlg,
						 const gchar *word);
	void		(*ignore_all)		(GtranslatorSpellCheckerDialog *dlg,
						 const gchar *word);
	void		(*change)		(GtranslatorSpellCheckerDialog *dlg,
						 const gchar *word, 
						 const gchar *change_to);
	void		(*change_all)		(GtranslatorSpellCheckerDialog *dlg,
						 const gchar *word, 
						 const gchar *change_to);
	void		(*add_word_to_personal)	(GtranslatorSpellCheckerDialog *dlg,
						 const gchar *word);

};

GType        		 gtranslator_spell_checker_dialog_get_type	(void) G_GNUC_CONST;

/* Constructors */
GtkWidget		*gtranslator_spell_checker_dialog_new		(void);
GtkWidget		*gtranslator_spell_checker_dialog_new_from_spell_checker 
								(GtranslatorSpellChecker *spell);

GtranslatorSpellChecker
				 *gtranslator_spell_checker_dialog_get_spell_checker 
								(GtranslatorSpellCheckerDialog *dlg);
GtkTextView
				 *gtranslator_spell_checker_dialog_get_view 
								(GtranslatorSpellCheckerDialog *dlg);
				 
void 			 gtranslator_spell_checker_dialog_set_spell_checker
								(GtranslatorSpellCheckerDialog *dlg,
								 GtranslatorSpellChecker *spell);
void			 gtranslator_spell_checker_dialog_set_misspelled_word 
								(GtranslatorSpellCheckerDialog *dlg, 
								 const gchar* word, 
								 gint len);

void 			 gtranslator_spell_checker_dialog_set_completed 
								(GtranslatorSpellCheckerDialog *dlg);
								
G_END_DECLS

#endif  /* __GTRANSLATOR_SPELL_CHECKER_DIALOG_H__ */

