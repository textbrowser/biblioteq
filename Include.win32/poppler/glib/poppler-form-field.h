/* poppler-form-field.h: glib interface to poppler
 *
 * Copyright (C) 2007 Carlos Garcia Campos <carlosgc@gnome.org>
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
 * Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __POPPLER_FORM_FIELD_H__
#define __POPPLER_FORM_FIELD_H__

#include <glib-object.h>
#include "poppler.h"

G_BEGIN_DECLS

#define POPPLER_TYPE_FORM_FIELD (poppler_form_field_get_type())
#define POPPLER_FORM_FIELD(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), POPPLER_TYPE_FORM_FIELD, PopplerFormField))
#define POPPLER_IS_FORM_FIELD(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), POPPLER_TYPE_FORM_FIELD))

typedef enum
{
    POPPLER_FORM_FIELD_UNKNOWN,
    POPPLER_FORM_FIELD_BUTTON,
    POPPLER_FORM_FIELD_TEXT,
    POPPLER_FORM_FIELD_CHOICE,
    POPPLER_FORM_FIELD_SIGNATURE
} PopplerFormFieldType;

typedef enum
{
    POPPLER_FORM_BUTTON_PUSH,
    POPPLER_FORM_BUTTON_CHECK,
    POPPLER_FORM_BUTTON_RADIO
} PopplerFormButtonType;

typedef enum
{
    POPPLER_FORM_TEXT_NORMAL,
    POPPLER_FORM_TEXT_MULTILINE,
    POPPLER_FORM_TEXT_FILE_SELECT
} PopplerFormTextType;

typedef enum
{
    POPPLER_FORM_CHOICE_COMBO,
    POPPLER_FORM_CHOICE_LIST
} PopplerFormChoiceType;

/**
 * PopplerAdditionalActionType:
 * @POPPLER_ADDITIONAL_ACTION_FIELD_MODIFIED: The action to be performed when the user modifies the field.
 * @POPPLER_ADDITIONAL_ACTION_FORMAT_FIELD: The action to be performed before the field is formatted to
 * display its value.
 * @POPPLER_ADDITIONAL_ACTION_VALIDATE_FIELD: The action to be performed when the field value changes.
 * @POPPLER_ADDITIONAL_ACTION_CALCULATE_FIELD: The action to be performed when the field needs to be
 * recalculated.
 *
 * Form field additional action types to be passed to @poppler_form_field_get_additional_action
 *
 * Since: 0.72
 */
typedef enum
{
    POPPLER_ADDITIONAL_ACTION_FIELD_MODIFIED,
    POPPLER_ADDITIONAL_ACTION_FORMAT_FIELD,
    POPPLER_ADDITIONAL_ACTION_VALIDATE_FIELD,
    POPPLER_ADDITIONAL_ACTION_CALCULATE_FIELD
} PopplerAdditionalActionType;

POPPLER_PUBLIC
GType poppler_form_field_get_type(void) G_GNUC_CONST;

POPPLER_PUBLIC
PopplerFormFieldType poppler_form_field_get_field_type(PopplerFormField *field);
POPPLER_PUBLIC
gint poppler_form_field_get_id(PopplerFormField *field);
POPPLER_PUBLIC
gdouble poppler_form_field_get_font_size(PopplerFormField *field);
POPPLER_PUBLIC
gboolean poppler_form_field_is_read_only(PopplerFormField *field);
POPPLER_PUBLIC
gchar *poppler_form_field_get_partial_name(PopplerFormField *field);
POPPLER_PUBLIC
gchar *poppler_form_field_get_mapping_name(PopplerFormField *field);
POPPLER_PUBLIC
gchar *poppler_form_field_get_name(PopplerFormField *field);
POPPLER_PUBLIC
PopplerAction *poppler_form_field_get_action(PopplerFormField *field);
POPPLER_PUBLIC
PopplerAction *poppler_form_field_get_additional_action(PopplerFormField *field, PopplerAdditionalActionType type);
POPPLER_PUBLIC
gchar *poppler_form_field_get_alternate_ui_name(PopplerFormField *field);

/* Button Field */
POPPLER_PUBLIC
PopplerFormButtonType poppler_form_field_button_get_button_type(PopplerFormField *field);
POPPLER_PUBLIC
gboolean poppler_form_field_button_get_state(PopplerFormField *field);
POPPLER_PUBLIC
void poppler_form_field_button_set_state(PopplerFormField *field, gboolean state);

/* Text Field */
POPPLER_PUBLIC
PopplerFormTextType poppler_form_field_text_get_text_type(PopplerFormField *field);
POPPLER_PUBLIC
gchar *poppler_form_field_text_get_text(PopplerFormField *field);
POPPLER_PUBLIC
void poppler_form_field_text_set_text(PopplerFormField *field, const gchar *text);
POPPLER_PUBLIC
gint poppler_form_field_text_get_max_len(PopplerFormField *field);
POPPLER_PUBLIC
gboolean poppler_form_field_text_do_spell_check(PopplerFormField *field);
POPPLER_PUBLIC
gboolean poppler_form_field_text_do_scroll(PopplerFormField *field);
POPPLER_PUBLIC
gboolean poppler_form_field_text_is_rich_text(PopplerFormField *field);
POPPLER_PUBLIC
gboolean poppler_form_field_text_is_password(PopplerFormField *field);

/* Choice Field */
POPPLER_PUBLIC
PopplerFormChoiceType poppler_form_field_choice_get_choice_type(PopplerFormField *field);
POPPLER_PUBLIC
gboolean poppler_form_field_choice_is_editable(PopplerFormField *field);
POPPLER_PUBLIC
gboolean poppler_form_field_choice_can_select_multiple(PopplerFormField *field);
POPPLER_PUBLIC
gboolean poppler_form_field_choice_do_spell_check(PopplerFormField *field);
POPPLER_PUBLIC
gboolean poppler_form_field_choice_commit_on_change(PopplerFormField *field);
POPPLER_PUBLIC
gint poppler_form_field_choice_get_n_items(PopplerFormField *field);
POPPLER_PUBLIC
gchar *poppler_form_field_choice_get_item(PopplerFormField *field, gint index);
POPPLER_PUBLIC
gboolean poppler_form_field_choice_is_item_selected(PopplerFormField *field, gint index);
POPPLER_PUBLIC
void poppler_form_field_choice_select_item(PopplerFormField *field, gint index);
POPPLER_PUBLIC
void poppler_form_field_choice_unselect_all(PopplerFormField *field);
POPPLER_PUBLIC
void poppler_form_field_choice_toggle_item(PopplerFormField *field, gint index);
POPPLER_PUBLIC
void poppler_form_field_choice_set_text(PopplerFormField *field, const gchar *text);
POPPLER_PUBLIC
gchar *poppler_form_field_choice_get_text(PopplerFormField *field);

G_END_DECLS

#endif /* __POPPLER_FORM_FIELD_H__ */
