/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
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

#include <stdlib.h>
#include <config.h>
#include <string.h>
#include <glib.h>
#include <stdio.h>
#include <dlfcn.h>

#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>

#include "paths.h"
#include "registry.h"
#include "deviceeventcontroller.h"
#include "libregistry-internals.h"

#define CORBA_GCONF_KEY  "/desktop/gnome/interface/at-spi-corba"

static gboolean need_to_quit ();

static GMainLoop *mainloop;
static gchar *dbus_name = NULL;
static gboolean use_gnome_session = FALSE;

static GOptionEntry optentries[] =
{
  {"dbus-name", 0, 0, G_OPTION_ARG_STRING, &dbus_name, "Well-known name to register with D-Bus", NULL},
  {"use-gnome-session", 0, 0, G_OPTION_ARG_NONE, &use_gnome_session, "Should register with gnome session manager", NULL},
  {NULL}
};

static DBusGConnection *bus_connection = NULL;
static DBusGProxy      *sm_proxy = NULL;
static char            *client_id = NULL;
static DBusGProxy      *client_proxy = NULL;

#define SM_DBUS_NAME      "org.gnome.SessionManager"
#define SM_DBUS_PATH      "/org/gnome/SessionManager"
#define SM_DBUS_INTERFACE "org.gnome.SessionManager"

#define SM_CLIENT_DBUS_INTERFACE "org.gnome.SessionManager.ClientPrivate"

static void registry_session_init (const char *previous_client_id, const char *exe);

static gboolean
session_manager_connect (void)
{

        if (bus_connection == NULL) {
                GError *error;

                error = NULL;
                bus_connection = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
                if (bus_connection == NULL) {
                        g_message ("Failed to connect to the session bus: %s",
                                   error->message);
                        g_error_free (error);
                        exit (1);
                }
        }

        sm_proxy = dbus_g_proxy_new_for_name (bus_connection,
                                              SM_DBUS_NAME,
                                              SM_DBUS_PATH,
                                              SM_DBUS_INTERFACE);
        return (sm_proxy != NULL);
}

static void
stop_cb (gpointer data)
{
        g_main_loop_quit (mainloop);
}

static gboolean
end_session_response (gboolean is_okay, const gchar *reason)
{
        gboolean ret;
        GError *error = NULL;

        ret = dbus_g_proxy_call (client_proxy, "EndSessionResponse",
                                 &error,
                                 G_TYPE_BOOLEAN, is_okay,
                                 G_TYPE_STRING, reason,
                                 G_TYPE_INVALID,
                                 G_TYPE_INVALID);

        if (!ret) {
                g_warning ("Failed to send session response %s", error->message);
                g_error_free (error);
        }

        return ret;
}

static void
query_end_session_cb (guint flags, gpointer data)
{
        end_session_response (TRUE, NULL);
}

static void
end_session_cb (guint flags, gpointer data)
{
        end_session_response (TRUE, NULL);
        g_main_loop_quit (mainloop);
}
static gboolean
register_client (void)
{
        GError     *error;
        gboolean    res;
        const char *startup_id;
        const char *app_id;

        startup_id = g_getenv ("DESKTOP_AUTOSTART_ID");
        app_id = "at-spi-registryd.desktop";

        error = NULL;
        res = dbus_g_proxy_call (sm_proxy,
                                 "RegisterClient",
                                 &error,
                                 G_TYPE_STRING, app_id,
                                 G_TYPE_STRING, startup_id,
                                 G_TYPE_INVALID,
                                 DBUS_TYPE_G_OBJECT_PATH, &client_id,
                                 G_TYPE_INVALID);
        if (! res) {
                g_warning ("Failed to register client: %s", error->message);
                g_error_free (error);
                return FALSE;
        }

        client_proxy = dbus_g_proxy_new_for_name (bus_connection,
                                                  SM_DBUS_NAME,
                                                  client_id,
                                                  SM_CLIENT_DBUS_INTERFACE);

        dbus_g_proxy_add_signal (client_proxy, "Stop", G_TYPE_INVALID);
        dbus_g_proxy_connect_signal (client_proxy, "Stop",
                                     G_CALLBACK (stop_cb), NULL, NULL);

        dbus_g_proxy_add_signal (client_proxy, "QueryEndSession", G_TYPE_UINT, G_TYPE_INVALID);
        dbus_g_proxy_connect_signal (client_proxy, "QueryEndSession",
                                     G_CALLBACK (query_end_session_cb), NULL, NULL);

        dbus_g_proxy_add_signal (client_proxy, "EndSession", G_TYPE_UINT, G_TYPE_INVALID);
        dbus_g_proxy_connect_signal (client_proxy, "EndSession",
                                     G_CALLBACK (end_session_cb), NULL, NULL);

        g_unsetenv ("DESKTOP_AUTOSTART_ID");

        return TRUE;
}

/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/

typedef GObject *(*gconf_client_get_default_t) ();
typedef gboolean (*gconf_client_get_bool_t)(GObject *, const char *, void *);

int
main (int argc, char **argv)
{
  SpiRegistry *registry;
  SpiDEController *dec;

  DBusConnection *bus = NULL;

  GOptionContext *opt;

  GError *err = NULL;
  DBusError error;
  int ret;

  if (need_to_quit ())
    return 0;

  g_type_init();

  /*Parse command options*/
  opt = g_option_context_new(NULL);
  g_option_context_add_main_entries(opt, optentries, NULL);

  if (!g_option_context_parse(opt, &argc, &argv, &err))
      g_error("Option parsing failed: %s\n", err->message);

  if (dbus_name == NULL)
      dbus_name = SPI_DBUS_NAME_REGISTRY;

  dbus_error_init (&error);
  bus = _libregistry_get_a11y_bus ();
  if (!bus)
  {
    return 0;
  }

  mainloop = g_main_loop_new (NULL, FALSE);
  dbus_connection_setup_with_g_main(bus, NULL);

  ret = dbus_bus_request_name(bus, dbus_name, DBUS_NAME_FLAG_DO_NOT_QUEUE, &error);
  if (ret == DBUS_REQUEST_NAME_REPLY_EXISTS)
    {
      g_error("Could not obtain D-Bus name - %s\n", dbus_name);
    }
  else
    {
      g_print ("SpiRegistry daemon is running with well-known name - %s\n", dbus_name);
    }

  registry = spi_registry_new (bus);
  dec = spi_registry_dec_new (registry, bus);

  if (use_gnome_session)
    {
      if (!session_manager_connect ())
          g_warning ("Unable to connect to session manager");

      if (!register_client ())
          g_warning ("Unable to register client with session manager");
    }

  g_main_loop_run (mainloop);
  return 0;
}

static gboolean
need_to_quit ()
{
  void *gconf = NULL;
  gconf_client_get_default_t gconf_client_get_default = NULL;
  gconf_client_get_bool_t gconf_client_get_bool = NULL;
  GObject *gconf_client;	/* really a GConfClient */
  gboolean ret;

  g_type_init ();

  gconf = dlopen ("libgconf-2.so", RTLD_LAZY);
  if (gconf)
    {
      gconf_client_get_default = dlsym (gconf, "gconf_client_get_default");
      gconf_client_get_bool = dlsym (gconf, "gconf_client_get_bool");
  }

  if (!gconf_client_get_default || !gconf_client_get_bool)
    {
      if (gconf)
        dlclose (gconf);
      return FALSE;
    }

  /* If we've been relocated, we will exit if the at-spi-corba gconf key
 * has been set.  If we have not been relocated, we will only run if the
 * at-spi-dbus gconf key has been set.
   */
  gconf_client = gconf_client_get_default ();
  ret = gconf_client_get_bool (gconf_client, CORBA_GCONF_KEY, NULL);
  g_object_unref (gconf_client);

  return ret;
}
