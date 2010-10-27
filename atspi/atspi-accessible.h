/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2002 Ximian, Inc.
 *           2002 Sun Microsystems Inc.
 *           
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

#ifndef _ATSPI_ACCESSIBLE_H_
#define _ATSPI_ACCESSIBLE_H_

#include "glib-object.h"

#include "atspi-application.h"
#include "atspi-constants.h"
#include "atspi-stateset.h"

#define ATSPI_TYPE_ACCESSIBLE                        (atspi_accessible_get_type ())
#define ATSPI_ACCESSIBLE(obj)                        (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATSPI_TYPE_ACCESSIBLE, AtspiAccessible))
#define ATSPI_ACCESSIBLE_CLASS(klass)                (G_TYPE_CHECK_CLASS_CAST ((klass), ATSPI_TYPE_ACCESSIBLE, AtspiAccessibleClass))
#define ATSPI_IS_ACCESSIBLE(obj)                     (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATSPI_TYPE_ACCESSIBLE))
#define ATSPI_IS_ACCESSIBLE_CLASS(klass)             (G_TYPE_CHECK_CLASS_TYPE ((klass), ATSPI_TYPE_ACCESSIBLE))
#define ATSPI_ACCESSIBLE_GET_CLASS(obj)              (G_TYPE_INSTANCE_GET_CLASS ((obj), ATSPI_TYPE_ACCESSIBLE, AtspiAccessibleClass))

typedef struct _AtspiAccessible AtspiAccessible;
struct _AtspiAccessible
{
  GObject parent;
  gint ref_count;
  AtspiAccessible *accessible_parent;
  GList *children;
  AtspiApplication *app;
  char *path;
  gint role : 8;
  gint interfaces : 24;
  char *name;
  char *description;
  AtspiStateSet *states;
};

typedef struct _AtspiAccessibleClass AtspiAccessibleClass;
struct _AtspiAccessibleClass
{
  GObjectClass parent_class;
};

GType atspi_accessible_get_type (void); 

AtspiAccessible *
atspi_accessible_new (AtspiApplication *app, const gchar *path);

gchar * atspi_role_get_name (AtspiRole role);

gchar * atspi_accessible_get_name (AtspiAccessible *obj, GError **error);

gchar * atspi_accessible_get_description (AtspiAccessible *obj, GError **error);

AtspiAccessible * atspi_accessible_get_parent (AtspiAccessible *obj, GError **error);

gint atspi_accessible_get_child_count (AtspiAccessible *obj, GError *error);

AtspiAccessible * atspi_accessible_get_child_at_index (AtspiAccessible *obj, gint    child_index, GError **error);

gint atspi_accessible_get_index_in_parent (AtspiAccessible *obj, GError **error);

GArray * atspi_accessible_get_relation_set (AtspiAccessible *obj, GError **error);

AtspiRole atspi_accessible_get_role (AtspiAccessible *obj, GError **error);

gchar * atspi_accessible_get_role_name (AtspiAccessible *obj, GError **error);

gchar * atspi_accessible_get_localized_role_name (AtspiAccessible *obj, GError **error);

AtspiStateSet * atspi_accessible_get_state_set (AtspiAccessible *obj);

GHashTable * atspi_accessible_get_attributes (AtspiAccessible *obj, GError **error);

AtspiApplication * atspi_accessible_get_host_application (AtspiAccessible *obj, GError **error);

#endif	/* _ATSPI_ACCESSIBLE_H_ */
