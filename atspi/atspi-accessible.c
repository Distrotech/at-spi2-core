/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
 * Copyright 2010, 2011 Novell, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "atspi-private.h"
#include <string.h>

static void
atspi_action_interface_init (AtspiAction *action)
{
}

static void
atspi_collection_interface_init (AtspiCollection *component)
{
}

static void
atspi_component_interface_init (AtspiComponent *component)
{
}

static void
atspi_document_interface_init (AtspiDocument *document)
{
}

static void
atspi_editable_text_interface_init (AtspiEditableText *editable_text)
{
}

static void
atspi_hypertext_interface_init (AtspiHypertext *hypertext)
{
}

static void
atspi_image_interface_init (AtspiImage *image)
{
}

static void
atspi_selection_interface_init (AtspiSelection *selection)
{
}

static void
atspi_table_interface_init (AtspiTable *table)
{
}

static void
atspi_text_interface_init (AtspiText *text)
{
}

static void
atspi_value_interface_init (AtspiValue *value)
{
}

G_DEFINE_TYPE_WITH_CODE (AtspiAccessible, atspi_accessible, ATSPI_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_ACTION, atspi_action_interface_init)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_COLLECTION, atspi_collection_interface_init)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_COMPONENT, atspi_component_interface_init)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_DOCUMENT, atspi_document_interface_init)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_EDITABLE_TEXT, atspi_editable_text_interface_init)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_HYPERTEXT, atspi_hypertext_interface_init)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_IMAGE, atspi_image_interface_init)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_SELECTION, atspi_selection_interface_init)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_TABLE, atspi_table_interface_init)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_TEXT, atspi_text_interface_init)
                         G_IMPLEMENT_INTERFACE (ATSPI_TYPE_VALUE, atspi_value_interface_init))

#ifdef DEBUG_REF_COUNTS
static gint accessible_count = 0;
#endif

static void
atspi_accessible_init (AtspiAccessible *accessible)
{
#ifdef DEBUG_REF_COUNTS
  accessible_count++;
  printf("at-spi: init: %d objects\n", accessible_count);
#endif
}

static void
atspi_accessible_dispose (GObject *object)
{
  AtspiAccessible *accessible = ATSPI_ACCESSIBLE (object);
  AtspiEvent e;
  AtspiAccessible *parent;

  /* TODO: Only fire if object not already marked defunct */
  memset (&e, 0, sizeof (e));
  e.type = "object:state-change:defunct";
  e.source = accessible;
  e.detail1 = 1;
  e.detail2 = 0;
  _atspi_send_event (&e);

  if (accessible->states)
  {
    g_object_unref (accessible->states);
    accessible->states = NULL;
  }

  parent = accessible->accessible_parent;
  if (parent && parent->children)
  {
    GList*ls = g_list_find (parent->children, accessible);
    if(ls)
    {
      gboolean replace = (ls == parent->children);
      ls = g_list_remove (ls, accessible);
      if (replace)
        parent->children = ls;
      g_object_unref (object);
    }
  }

  if (parent)
  {
    g_object_unref (parent);
    accessible->accessible_parent = NULL;
  }

  G_OBJECT_CLASS (atspi_accessible_parent_class) ->dispose (object);
}

static void
atspi_accessible_finalize (GObject *object)
{
  AtspiAccessible *accessible = ATSPI_ACCESSIBLE (object);

    g_free (accessible->description);
    g_free (accessible->name);

#ifdef DEBUG_REF_COUNTS
  accessible_count--;
  printf("at-spi: finalize: %d objects\n", accessible_count);
#endif

  G_OBJECT_CLASS (atspi_accessible_parent_class)
    ->finalize (object);
}

static void
atspi_accessible_class_init (AtspiAccessibleClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = atspi_accessible_dispose;
  object_class->finalize = atspi_accessible_finalize;
}

/* TODO: Generate following from spec? */
static const char *role_names [] =
{
  "invalid",
  "accel-label",
  "alert",
  "animation",
  "arrow",
  "calendar",
  "canvas",
  "check-box",
  "check-menu-item",
  "color-chooser",
  "column-header",
  "combo-box",
  "date-editor",
  "desktop-icon",
  "desktop-frame",
  "dial",
  "dialog",
  "directory-pane",
  "drawing-area",
  "file-chooser",
  "filler",
  "font-chooser",
  "frame",
  "glass-pane",
  "html-container",
  "icon",
  "image",
  "internalframe",
  "label",
  "layered-pane",
  "list",
  "list-item",
  "menu",
  "menu-bar",
  "menu-item",
  "option-pane",
  "page-tab",
  "page-tab-list",
  "panel",
  "password-text",
  "popup-menu",
  "progress-bar",
  "push-button",
  "radio-button",
  "radio-menu-item",
  "root-pane",
  "row-header",
  "scroll-bar",
  "scroll-pane",
  "separator",
  "slider",
  "spin-button",
  "split-pane",
  "statusbar",
  "table",
  "table-cell",
  "table-column-header",
  "table-row-header",
  "tear-off-menu-item",
  "terminal",
  "text",
  "toggle-button",
  "tool-bar",
  "tool-tip",
  "tree",
  "tree-table",
  "unknown",
  "viewport",
  "window",
  NULL,
  "header",
  "fooler",
  "paragraph",
  "ruler",
  "application",
  "autocomplete",
  "editbar",
  "embedded",
  "entry",
  "chart",
  "caption",
  "document_frame",
  "heading",
  "page",
  "section",
  "form",
  "redundant object",
  "link",
  "input method window"
};

#define MAX_ROLES (sizeof (role_names) / sizeof (char *))

/**
 * atspi_role_get_name
 * @role: an #AtspiAccessibleRole object to query.
 *
 * Get a localizeable string that indicates the name of an #AtspiAccessibleRole.
 * <em>DEPRECATED.</em>
 *
 * Returns: a localizable string name for an #AtspiAccessibleRole enumerated type.
 **/
gchar *
atspi_role_get_name (AtspiRole role)
{
  if (role < MAX_ROLES && role_names [(int) role])
    {
      return g_strdup (role_names [(int) role]);
    }
  else
    {
      return g_strdup ("");
    }
}

/**
 * atspi_accessible_get_name:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Get the name of an #AtspiAccessible object.
 *
 * Returns: a UTF-8 string indicating the name of the #AtspiAccessible object.
 * or NULL on exception
 **/
gchar *
atspi_accessible_get_name (AtspiAccessible *obj, GError **error)
{
  g_return_val_if_fail (obj != NULL, g_strdup (""));
  if (!(obj->cached_properties & ATSPI_CACHE_NAME))
  {
    if (!_atspi_dbus_get_property (obj, atspi_interface_accessible, "Name", error,
                                   "s", &obj->name))
      return g_strdup ("");
    _atspi_accessible_add_cache (obj, ATSPI_CACHE_NAME);
  }
  return g_strdup (obj->name);
}

/**
 * atspi_accessible_get_description:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Get the description of an #AtspiAccessible object.
 *
 * Returns: a UTF-8 string describing the #AtspiAccessible object.
 * or NULL on exception
 **/
gchar *
atspi_accessible_get_description (AtspiAccessible *obj, GError **error)
{
  g_return_val_if_fail (obj != NULL, g_strdup (""));

  if (!(obj->cached_properties & ATSPI_CACHE_DESCRIPTION))
  {
    if (!_atspi_dbus_get_property (obj, atspi_interface_accessible,
                                   "Description", error, "s",
                                   &obj->description))
      return g_strdup ("");
    _atspi_accessible_add_cache (obj, ATSPI_CACHE_DESCRIPTION);
  }
  return g_strdup (obj->description);
}

const char *str_parent = "Parent";

/**
 * atspi_accessible_get_parent:
 * @obj: a pointer to the #AtspiAccessible object to query.
 *
 * Get an #AtspiAccessible object's parent container.
 *
 * Returns: (transfer full): a pointer to the #AtspiAccessible object which
 *          contains the given #AtspiAccessible instance, or NULL if the @obj
 *          has no parent container.
 *
 **/
AtspiAccessible *
atspi_accessible_get_parent (AtspiAccessible *obj, GError **error)
{
  g_return_val_if_fail (obj != NULL, NULL);

  if (obj->parent.app && !(obj->cached_properties & ATSPI_CACHE_PARENT))
  {
    DBusMessage *message, *reply;
    DBusMessageIter iter, iter_variant;
    message = dbus_message_new_method_call (obj->parent.app->bus_name,
                                            obj->parent.path,
                                            DBUS_INTERFACE_PROPERTIES, "Get");
    if (!message)
      return NULL;
    dbus_message_append_args (message, DBUS_TYPE_STRING, &atspi_interface_accessible,
                               DBUS_TYPE_STRING, &str_parent,
                              DBUS_TYPE_INVALID);
    reply = _atspi_dbus_send_with_reply_and_block (message, error);
    if (!reply)
      return NULL;
    if (strcmp (dbus_message_get_signature (reply), "v") != 0)
    {
      dbus_message_unref (reply);
      return NULL;
    }
    dbus_message_iter_init (reply, &iter);
    dbus_message_iter_recurse (&iter, &iter_variant);
    obj->accessible_parent = _atspi_dbus_return_accessible_from_iter (&iter_variant);
    dbus_message_unref (reply);
    _atspi_accessible_add_cache (obj, ATSPI_CACHE_PARENT);
  }
  if (!obj->accessible_parent)
    return NULL;
  return g_object_ref (obj->accessible_parent);
}

/**
 * atspi_accessible_get_child_count:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Get the number of children contained by an #AtspiAccessible object.
 *
 * Returns: a #long indicating the number of #AtspiAccessible children
 *          contained by an #AtspiAccessible object. or -1 on exception
 *
 **/
gint
atspi_accessible_get_child_count (AtspiAccessible *obj, GError **error)
{
  g_return_val_if_fail (obj != NULL, -1);

  if (!(obj->cached_properties & ATSPI_CACHE_CHILDREN))
  {
    dbus_int32_t ret;
    if (!_atspi_dbus_get_property (obj, atspi_interface_accessible,
                                   "ChildCount", error, "i", &ret))
      return -1;
    return ret;
  }

  return g_list_length (obj->children);
}

/**
 * atspi_accessible_get_child_at_index:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 * @child_index: a #long indicating which child is specified.
 *
 * Get the #AtspiAccessible child of an #AtspiAccessible object at a given index.
 *
 * Returns: (transfer full): a pointer to the #AtspiAccessible child object at
 * index @child_index. or NULL on exception
 **/
AtspiAccessible *
atspi_accessible_get_child_at_index (AtspiAccessible *obj,
                            gint    child_index,
                            GError **error)
{
  AtspiAccessible *child;

  g_return_val_if_fail (obj != NULL, NULL);

  if (!(obj->cached_properties & ATSPI_CACHE_CHILDREN))
  {
    DBusMessage *reply;
    reply = _atspi_dbus_call_partial (obj, atspi_interface_accessible,
                                     "GetChildAtIndex", error, "i",
                                     child_index);
    return _atspi_dbus_return_accessible_from_message (reply);
  }

  child = g_list_nth_data (obj->children, child_index);
  if (!child)
    return NULL;
  return g_object_ref (child);
}

/**
 * atspi_accessible_get_index_in_parent
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Get the index of an #AtspiAccessible object in its containing #AtspiAccessible.
 *
 * Returns: a #glong indicating the index of the #AtspiAccessible object
 *          in its parent (i.e. containing) #AtspiAccessible instance,
 *          or -1 if @obj has no containing parent or on exception.
 **/
gint
atspi_accessible_get_index_in_parent (AtspiAccessible *obj, GError **error)
{
  GList *l;
  gint i = 0;

  g_return_val_if_fail (obj != NULL, -1);
  if (!obj->accessible_parent) return -1;
  if (!(obj->accessible_parent->cached_properties & ATSPI_CACHE_CHILDREN))
  {
    dbus_uint32_t ret = -1;
    _atspi_dbus_call (obj, atspi_interface_accessible,
                      "GetIndexInParent", NULL, "=>u", &ret);
    return ret;
  }

  l = obj->accessible_parent->children;
  while (l)
  {
    if (l->data == obj) return i;
    l = g_list_next (l);
    i++;
  }
  return -1;
}

typedef struct
{
  dbus_uint32_t type;
  GArray *targets;
} Accessibility_Relation;

/**
 * atspi_accessible_get_relation_set:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Get the set of #AtspiRelation objects which describe this #AtspiAccessible object's
 *       relationships with other #AtspiAccessible objects.
 *
 * Returns: (element-type AtspiAccessible*) (transfer full): an array of
 *          #AtspiAccessibleRelation pointers. or NULL on exception
 **/
GArray *
atspi_accessible_get_relation_set (AtspiAccessible *obj, GError **error)
{
  DBusMessage *reply;
  DBusMessageIter iter, iter_array;
  GArray *ret;

  g_return_val_if_fail (obj != NULL, NULL);

  reply = _atspi_dbus_call_partial (obj, atspi_interface_accessible, "GetRelationSet", error, "");
  if (!reply)
    return NULL;
  _ATSPI_DBUS_CHECK_SIG (reply, "a(ua(so))", error, NULL);

  ret = g_array_new (TRUE, TRUE, sizeof (AtspiRelation *));
  dbus_message_iter_init (reply, &iter);
  dbus_message_iter_recurse (&iter, &iter_array);
  while (dbus_message_iter_get_arg_type (&iter_array) != DBUS_TYPE_INVALID)
  {
    AtspiRelation *relation;
    relation = _atspi_relation_new_from_iter (&iter_array);
    ret = g_array_append_val (ret, relation);
    dbus_message_iter_next (&iter_array);
  }
  dbus_message_unref (reply);
  return ret;
}

/**
 * atspi_accessible_get_role:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Get the UI role of an #AtspiAccessible object.
 * A UTF-8 string describing this role can be obtained via atspi_accessible_getRoleName ().
 *
 * Returns: the #AtspiRole of the object.
 *
 **/
AtspiRole
atspi_accessible_get_role (AtspiAccessible *obj, GError **error)
{
  g_return_val_if_fail (obj != NULL, ATSPI_ROLE_INVALID);

  if (!(obj->cached_properties & ATSPI_CACHE_ROLE))
  {
    dbus_uint32_t role;
    /* TODO: Make this a property */
    if (_atspi_dbus_call (obj, atspi_interface_accessible, "GetRole", error, "=>u", &role))
    {
      obj->role = role;
    _atspi_accessible_add_cache (obj, ATSPI_CACHE_ROLE);
    }
  }
  return obj->role;
}

/**
 * atspi_accessible_get_role_name:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Get a UTF-8 string describing the role this object plays in the UI.
 * This method will return useful values for roles that fall outside the
 * enumeration used in atspi_accessible_getRole ().
 *
 * Returns: a UTF-8 string specifying the role of this #AtspiAccessible object.
 *
 **/
gchar *
atspi_accessible_get_role_name (AtspiAccessible *obj, GError **error)
{
  char *retval = NULL;

  g_return_val_if_fail (obj != NULL, NULL);

  _atspi_dbus_call (obj, atspi_interface_accessible, "GetRoleName", error, "=>s", &retval);

  if (!retval)
    retval = g_strdup ("");

  return retval;
}

/**
 * atspi_accessible_get_localized_role_name:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Get a UTF-8 string describing the (localized) role this object plays in the UI.
 * This method will return useful values for roles that fall outside the
 * enumeration used in atspi_accessible_getRole ().
 *
 * Returns: a UTF-8 string specifying the role of this #AtspiAccessible object.
 *
 **/
gchar *
atspi_accessible_get_localized_role_name (AtspiAccessible *obj, GError **error)
{
  char *retval = NULL;

  g_return_val_if_fail (obj != NULL, NULL);

  _atspi_dbus_call (obj, atspi_interface_accessible, "GetLocalizedRoleName", error, "=>s", &retval);

  if (!retval)
    return g_strdup ("");

  return retval;
}

static AtspiStateSet *
defunct_set ()
{
  AtspiStateSet *set = atspi_state_set_new (NULL);
  atspi_state_set_add (set, ATSPI_STATE_DEFUNCT);
  return set;
}

/**
 * atspi_accessible_get_state_set:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Gets the current state of an object.
 *
 * Returns: (transfer full): a pointer to an #AtspiStateSet representing the
 *          object's current state.
 **/
AtspiStateSet *
atspi_accessible_get_state_set (AtspiAccessible *obj)
{
  if (!obj->parent.app || !obj->parent.app->bus)
    return defunct_set ();


  if (!(obj->cached_properties & ATSPI_CACHE_STATES))
  {
    DBusMessage *reply;
    DBusMessageIter iter;
    reply = _atspi_dbus_call_partial (obj, atspi_interface_accessible,
                                      "GetState", NULL, "");
    _ATSPI_DBUS_CHECK_SIG (reply, "au", NULL, defunct_set ());
    dbus_message_iter_init (reply, &iter);
    _atspi_dbus_set_state (obj, &iter);
    dbus_message_unref (reply);
    _atspi_accessible_add_cache (obj, ATSPI_CACHE_STATES);
  }

  return g_object_ref (obj->states);
}

/**
 * atspi_accessible_get_attributes:
 * @obj: The #AtspiAccessible being queried.
 *
 * Get the #AttributeSet representing any assigned 
 * name-value pair attributes or annotations for this object.
 * For typographic, textual, or textually-semantic attributes, see
 * atspi_text_get_attributes instead.
 *
 * Returns: (element-type gchar* gchar*) (transfer full): The name-value-pair
 *          attributes assigned to this object.
 */
GHashTable *
atspi_accessible_get_attributes (AtspiAccessible *obj, GError **error)
{
  DBusMessage *message;

    g_return_val_if_fail (obj != NULL, NULL);

  message = _atspi_dbus_call_partial (obj, atspi_interface_accessible, "GetAttributes", error, "");
  return _atspi_dbus_return_hash_from_message (message);
}

/**
 * atspi_accessible_get_attributes_as_array:
 * @obj: The #AtspiAccessible being queried.
 *
 * Get the #AttributeSet representing any assigned 
 * name-value pair attributes or annotations for this object.
 * For typographic, textual, or textually-semantic attributes, see
 * atspi_text_get_attributes_as_array instead.
 *
 * Returns: (element-type gchar*) (transfer full): The name-value-pair
 *          attributes assigned to this object.
 */
GArray *
atspi_accessible_get_attributes_as_array (AtspiAccessible *obj, GError **error)
{
  DBusMessage *message;

    g_return_val_if_fail (obj != NULL, NULL);

  message = _atspi_dbus_call_partial (obj, atspi_interface_accessible, "GetAttributes", error, "");
  return _atspi_dbus_return_attribute_array_from_message (message);
}

/**
 * atspi_accessible_get_application:
 * @obj: The #AtspiAccessible being queried.
 *
 * Get the containing #AtspiApplication for an object.
 *
 * Returns: (transfer full): the containing AtspiApplication instance for
 *          this object.
 */
AtspiAccessible *
atspi_accessible_get_application (AtspiAccessible *obj, GError **error)
{
  AtspiAccessible *parent;

  g_object_ref (obj);
  for (;;)
  {
    parent = atspi_accessible_get_parent (obj, NULL);
    if (!parent && obj->parent.app &&
        atspi_accessible_get_role (obj, NULL) != ATSPI_ROLE_APPLICATION)
    {
      AtspiAccessible *root = g_object_ref (obj->parent.app->root);
      if (root)
      {
        g_object_unref (obj);
        if (atspi_accessible_get_role (root, NULL) == ATSPI_ROLE_DESKTOP_FRAME)
        {
          g_object_unref (root);
          return NULL;
        }
        return root;
      }
    }
    if (!parent || parent == obj ||
        atspi_accessible_get_role (parent, NULL) == ATSPI_ROLE_DESKTOP_FRAME)
    return obj;
    g_object_unref (obj);
    obj = parent;
  }
}

/* Application-specific methods */

/**
 * atspi_accessible_get_toolkit_name:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Get the toolkit for a #AtspiAccessible object.
 * Only works on application root objects.
 *
 * Returns: a UTF-8 string indicating the toolkit name for the #AtspiAccessible object.
 * or NULL on exception
 **/
gchar *
atspi_accessible_get_toolkit_name (AtspiAccessible *obj, GError **error)
{
  gchar *ret = NULL;

  g_return_val_if_fail (obj != NULL, NULL);

  if (!_atspi_dbus_get_property (obj, atspi_interface_application, "ToolkitName", error, "s", &ret))
      return NULL;
  return ret;
}

/**
 * atspi_accessible_get_toolkit_version:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Get the toolkit version for a #AtspiAccessible object.
 * Only works on application root objects.
 *
 * Returns: a UTF-8 string indicating the toolkit ersion for the #AtspiAccessible object.
 * or NULL on exception
 **/
gchar *
atspi_accessible_get_toolkit_version (AtspiAccessible *obj, GError **error)
{
  gchar *ret = NULL;

  g_return_val_if_fail (obj != NULL, NULL);

  if (!_atspi_dbus_get_property (obj, atspi_interface_application, "ToolkitVersion", error, "s", &ret))
      return NULL;
  return ret;
}

/**
 * atspi_accessible_get_toolkit_version:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Get the application id for a #AtspiAccessible object.
 * Only works on application root objects.
 *
 * Returns: a gint indicating the id for the #AtspiAccessible object.
 * or -1 on exception
 **/
gint
atspi_accessible_get_id (AtspiAccessible *obj, GError **error)
{
  gint ret = -1;

  g_return_val_if_fail (obj != NULL, -1);

  if (!_atspi_dbus_get_property (obj, atspi_interface_application, "Id", error, "i", &ret))
      return -1;
  return ret;
}


/* Interface query methods */

static gboolean
_atspi_accessible_is_a (AtspiAccessible *accessible,
		      const char *interface_name)
{
  int n;

  if (accessible == NULL)
    {
      return FALSE;
    }

  if (!(accessible->cached_properties & ATSPI_CACHE_INTERFACES))
  {
    DBusMessage *reply;
    DBusMessageIter iter;
    reply = _atspi_dbus_call_partial (accessible, atspi_interface_accessible,
                                      "GetInterfaces", NULL, "");
    _ATSPI_DBUS_CHECK_SIG (reply, "as", NULL, FALSE);
    dbus_message_iter_init (reply, &iter);
    _atspi_dbus_set_interfaces (accessible, &iter);
    dbus_message_unref (reply);
    _atspi_accessible_add_cache (accessible, ATSPI_CACHE_INTERFACES);
  }

  n = _atspi_get_iface_num (interface_name);
  if (n == -1) return FALSE;
  return (gboolean) ((accessible->interfaces & (1 << n))? TRUE: FALSE);
}

/**
 * atspi_accessible_is_action:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements #AtspiAction.
 *
 * Returns: #TRUE if @obj implements the #AtspiAction interface,
 *          #FALSE otherwise.
 **/
gboolean
atspi_accessible_is_action (AtspiAccessible *obj)
{
  return _atspi_accessible_is_a (obj,
			      atspi_interface_action);
}

/**
 * atspi_accessible_is_application:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements #AtspiApplication.
 *
 * Returns: #TRUE if @obj implements the #AtspiApplication interface,
 *          #FALSE otherwise.
 **/
gboolean
atspi_accessible_is_application (AtspiAccessible *obj)
{
  return _atspi_accessible_is_a (obj,
			      atspi_interface_application);
}

/**                      
 * atspi_accessible_is_collection:                                                                                                                                                                          * @obj: a pointer to the #AtspiAccessible instance to query.                                                                                                                                          
 *                          
 * Query whether the specified #AtspiAccessible implements #AtspiCollection.    
 * Returns: #TRUE if @obj implements the #AtspiCollection interface,                                                                                                               
 *          #FALSE otherwise.
 **/
gboolean
atspi_accessible_is_collection (AtspiAccessible *obj)
{
     return _atspi_accessible_is_a (obj,
			      atspi_interface_collection);
}

/**
 * atspi_accessible_is_component:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements #AtspiComponent.
 *
 * Returns: #TRUE if @obj implements the #AtspiComponent interface,
 *          #FALSE otherwise.
 **/
gboolean
atspi_accessible_is_component (AtspiAccessible *obj)
{
  return _atspi_accessible_is_a (obj,
			      atspi_interface_component);
}

/**
 * atspi_accessible_is_document:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements #AtspiDocument.
 *
 * Returns: #TRUE if @obj implements the #AtspiDocument interface,
 *          #FALSE otherwise.
 **/
gboolean
atspi_accessible_is_document (AtspiAccessible *obj)
{
  return _atspi_accessible_is_a (obj,
			      atspi_interface_document);
}

/**
 * atspi_accessible_is_editable_text:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements #AtspiEditableText.
 *
 * Returns: #TRUE if @obj implements the #AtspiEditableText interface,
 *          #FALSE otherwise.
 **/
gboolean
atspi_accessible_is_editable_text (AtspiAccessible *obj)
{
  return _atspi_accessible_is_a (obj,
			      atspi_interface_editable_text);
}
                                                                                                                                                                        
/**
 * atspi_accessible_is_hypertext:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements #AtspiHypertext.
 *
 * Returns: #TRUE if @obj implements the #AtspiHypertext interface,
 *          #FALSE otherwise.
 **/
gboolean
atspi_accessible_is_hypertext (AtspiAccessible *obj)
{
  return _atspi_accessible_is_a (obj,
			      atspi_interface_hypertext);
}

/**
 * atspi_accessible_is_hyperlink:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements #AtspiHyperlink.
 *
 * Returns: #TRUE if @obj implements the #AtspiHypertext interface,
 *          #FALSE otherwise.
 **/
gboolean
atspi_accessible_is_hyperlink (AtspiAccessible *obj)
{
  return _atspi_accessible_is_a (obj,
			      atspi_interface_hyperlink);
}

/**
 * atspi_accessible_is_image:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements #AtspiImage.
 *
 * Returns: #TRUE if @obj implements the #AtspiImage interface,
 *          #FALSE otherwise.
**/
gboolean
atspi_accessible_is_image (AtspiAccessible *obj)
{
  return _atspi_accessible_is_a (obj,
			      atspi_interface_image);
}

/**
 * atspi_accessible_is_selection:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements #AtspiSelection.
 *
 * Returns: #TRUE if @obj implements the #AtspiSelection interface,
 *          #FALSE otherwise.
**/
gboolean
atspi_accessible_is_selection (AtspiAccessible *obj)
{
  return _atspi_accessible_is_a (obj,
			      atspi_interface_selection);
}

/**
 * atspi_accessible_is_table:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements #AtspiTable.
 *
 * Returns: #TRUE if @obj implements the #AtspiTable interface,
 *          #FALSE otherwise.
**/
gboolean
atspi_accessible_is_table (AtspiAccessible *obj)
{
  return _atspi_accessible_is_a (obj,
			      atspi_interface_table);
}

/**
 * atspi_accessible_is_streamable_content:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements
 *          #AtspiStreamableContent.
 *
 * Returns: #TRUE if @obj implements the #AtspiStreamableContent interface,
 *          #FALSE otherwise.
**/
gboolean
atspi_accessible_is_streamable_content (AtspiAccessible *obj)
{
#if 0
  return _atspi_accessible_is_a (obj,
			      atspi_interface_streamable_content);
#else
  g_warning ("Streamable content not implemented");
  return FALSE;
#endif
}

/**
 * atspi_accessible_is_text:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements #AtspiText.
 *
 * Returns: #TRUE if @obj implements the #AtspiText interface,
 *          #FALSE otherwise.
**/
gboolean
atspi_accessible_is_text (AtspiAccessible *obj)
{
  return _atspi_accessible_is_a (obj,
			      atspi_interface_text);
}

/**
 * atspi_accessible_is_value:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Query whether the specified #AtspiAccessible implements #AtspiValue.
 *
 * Returns: #TRUE if @obj implements the #AtspiValue interface,
 *          #FALSE otherwise.
**/
gboolean
atspi_accessible_is_value (AtspiAccessible *obj)
{
  return _atspi_accessible_is_a (obj,
			      atspi_interface_value);
}

/**
 * atspi_accessible_get_action:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Get the #AtspiAction interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiAction interface
 *          instance, or NULL if @obj does not implement #AtspiAction.
 **/
AtspiAction *
atspi_accessible_get_action (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_action) ?
          g_object_ref (ATSPI_ACTION (accessible)) : NULL);  
}

/**
 * atspi_accessible_get_collection:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Get the #AtspiCollection interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiCollection interface
 *          instance, or NULL if @obj does not implement #AtspiCollection.
 **/
AtspiCollection *
atspi_accessible_get_collection (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_collection) ?
          g_object_ref (ATSPI_COLLECTION (accessible)) : NULL);  
}

/**
 * atspi_accessible_get_component:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Get the #AtspiComponent interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiComponent interface
 *          instance, or NULL if @obj does not implement #AtspiComponent.
 **/
AtspiComponent *
atspi_accessible_get_component (AtspiAccessible *obj)
{
  return (_atspi_accessible_is_a (obj, atspi_interface_component) ?
          g_object_ref (ATSPI_COMPONENT (obj)) : NULL);
}

/**
 * atspi_accessible_get_document:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Get the #AtspiDocument interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiDocument interface
 *          instance, or NULL if @obj does not implement #AtspiDocument.
 **/
AtspiDocument *
atspi_accessible_get_document (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_document) ?
          g_object_ref (ATSPI_DOCUMENT (accessible)) : NULL);  
}

/**
 * atspi_accessible_get_editable_text:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Get the #AtspiEditableText interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiEditableText interface
 *          instance, or NULL if @obj does not implement #AtspiEditableText.
 **/
AtspiEditableText *
atspi_accessible_get_editable_text (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_editable_text) ?
          g_object_ref (ATSPI_EDITABLE_TEXT (accessible)) : NULL);  
}

/**
 * atspi_accessible_get_hyperlink:
 * @obj: a pointer to the #AtspiAccessible object on which to operate.
 *
 * Get the #AtspiHyperlink associated with the given #AtspiAccessible, if
 * supported.
 *
 * Returns: (transfer full): the #AtspiHyperlink object associated with
 *          the given #AtspiAccessible, or NULL if not supported.
 **/
AtspiHyperlink *
atspi_accessible_get_hyperlink (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_hyperlink) ?
          atspi_hyperlink_new (accessible->parent.app, accessible->parent.path) : NULL);
}

/**
 * atspi_accessible_get_hypertext:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Get the #AtspiHypertext interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiHypertext interface
 *          instance, or NULL if @obj does not implement #AtspiHypertext.
 **/
AtspiHypertext *
atspi_accessible_get_hypertext (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_hypertext) ?
          g_object_ref (ATSPI_HYPERTEXT (accessible)) : NULL);  
}

/**
 * atspi_accessible_get_image:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Get the #AtspiImage interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiImage interface instance, or
 *          NULL if @obj does not implement #AtspiImage.
 **/
AtspiImage *
atspi_accessible_get_image (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_image) ?
          g_object_ref (ATSPI_IMAGE (accessible)) : NULL);  
}

/**
 * atspi_accessible_get_selection:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Get the #AtspiSelection interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiSelection interface
 *          instance, or NULL if @obj does not implement #AtspiSelection.
 **/
AtspiSelection *
atspi_accessible_get_selection (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_selection) ?
          g_object_ref (ATSPI_SELECTION (accessible)) : NULL);  
}

#if 0
/**
 * atspi_accessible_get_streamable_content:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Get the #AtspiStreamableContent interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiStreamableContent interface
 *          instance, or NULL if @obj does not implement #AtspiStreamableContent.
 **/
AtspiStreamableContent *
atspi_accessible_get_streamable_content (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_streamable_content) ?
          accessible : NULL);  
}
#endif

/**
 * atspi_accessible_get_table:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Get the #AtspiTable interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiTable interface instance, or
 *          NULL if @obj does not implement #AtspiTable.
 **/
AtspiTable *
atspi_accessible_get_table (AtspiAccessible *obj)
{
  return (_atspi_accessible_is_a (obj, atspi_interface_table) ?
          g_object_ref (ATSPI_TABLE (obj)) : NULL);  
}

/**
 * atspi_accessible_get_text:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Get the #AtspiTable interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiText interface instance, or
 *          NULL if @obj does not implement #AtspiText.
 **/
AtspiText *
atspi_accessible_get_text (AtspiAccessible *obj)
{
  return (_atspi_accessible_is_a (obj, atspi_interface_text) ?
          g_object_ref (ATSPI_TEXT (obj)) : NULL);
}

/**
 * atspi_accessible_get_value:
 * @obj: a pointer to the #AtspiAccessible instance to query.
 *
 * Get the #AtspiTable interface for an #AtspiAccessible.
 *
 * Returns: (transfer full): a pointer to an #AtspiValue interface instance, or
 *          NULL if @obj does not implement #AtspiValue.
 **/
AtspiValue *
atspi_accessible_get_value (AtspiAccessible *accessible)
{
  return (_atspi_accessible_is_a (accessible, atspi_interface_value) ?
          g_object_ref (ATSPI_VALUE (accessible)) : NULL);  
}

static void
append_const_val (GArray *array, const gchar *val)
{
  gchar *dup = g_strdup (val);

  if (dup)
    g_array_append_val (array, dup);
}

/**
 * atspi_accessible_get_interfaces:
 *
 * #obj: The #AtspiAccessible to query.
 *
 * Returns: (element-type gchar*) (transfer full): A #GArray of strings
 *          describing the interfaces supported by the object.  Interfaces are
 *          denoted in short-hand (ie, "Component", "Text", etc.)
 **/
GArray *
atspi_accessible_get_interfaces (AtspiAccessible *obj)
{
  GArray *ret = g_array_new (TRUE, TRUE, sizeof (gchar *));

  g_return_val_if_fail (obj != NULL, NULL);

  append_const_val (ret, "Accessible");
  if (atspi_accessible_is_action (obj))
    append_const_val (ret, "Action");
  if (atspi_accessible_is_collection (obj))
    append_const_val (ret, "Collection");
  if (atspi_accessible_is_component (obj))
    append_const_val (ret, "Component");
  if (atspi_accessible_is_document (obj))
    append_const_val (ret, "Document");
  if (atspi_accessible_is_editable_text (obj))
    append_const_val (ret, "EditableText");
  if (atspi_accessible_is_hypertext (obj))
    append_const_val (ret, "Hypertext");
  if (atspi_accessible_is_hyperlink (obj))
    append_const_val (ret, "Hyperlink");
  if (atspi_accessible_is_image (obj))
    append_const_val (ret, "Image");
  if (atspi_accessible_is_selection (obj))
    append_const_val (ret, "Selection");
  if (atspi_accessible_is_table (obj))
    append_const_val (ret, "Table");
  if (atspi_accessible_is_text (obj))
    append_const_val (ret, "Text");
  if (atspi_accessible_is_value (obj))
    append_const_val (ret, "Value");

  return ret;
}

AtspiAccessible *
atspi_accessible_new (AtspiApplication *app, const gchar *path)
{
  AtspiAccessible *accessible;
  
  accessible = g_object_new (ATSPI_TYPE_ACCESSIBLE, NULL);
  g_return_val_if_fail (accessible != NULL, NULL);

  accessible->parent.app = g_object_ref (app);
  accessible->parent.path = g_strdup (path);

  return accessible;
}

/**
 * atspi_accessible_set_cache_mask:
 *
 * @accessible: The #AtspiAccessible to operate on.  Must be the desktop or
 *             the root of an application.
 * @mask: An #AtspiCache specifying a bit mask of the types of data to cache.
 *
 * Sets the type of data to cache for accessibles.
 * If this is not set for an application or is reset to ATSPI_CACHE_UNDEFINED,
 * then the desktop's cache flag will be used.
 * If the desktop's cache flag is also undefined, then all possible data will
 * be cached.
 * This function is intended to work around bugs in toolkits where the proper
 * events are not raised / to aid in testing for such bugs.
 *
 * Note: This function has no effect on data that has already been cached.
 **/
void
atspi_accessible_set_cache_mask (AtspiAccessible *accessible, AtspiCache mask)
{
  g_return_if_fail (accessible != NULL);
  g_return_if_fail (accessible->parent.app != NULL);
  g_return_if_fail (accessible == accessible->parent.app->root);
  accessible->parent.app->cache = mask;
}

void
_atspi_accessible_add_cache (AtspiAccessible *accessible, AtspiCache flag)
{
  AtspiCache mask = accessible->parent.app->cache;

  if (mask == ATSPI_CACHE_UNDEFINED &&
      accessible->parent.app->root &&
      accessible->parent.app->root->accessible_parent)
  {
    AtspiAccessible *desktop = atspi_get_desktop (0);
    mask = desktop->parent.app->cache;
    g_object_unref (desktop);
  }

  if (mask == ATSPI_CACHE_UNDEFINED)
    mask = ATSPI_CACHE_ALL;

  accessible->cached_properties |= flag & mask;
}
