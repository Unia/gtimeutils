/* GTimerApplication
 *
 * Copyright (C) 2015 Jente Hidskes <hjdskes@gmail.com>
 *
 * Author: Jente Hidskes <hjdskes@gmail.com>
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include "gtimer-application.h"
#include "gtimer-window.h"

G_DEFINE_TYPE (GTimerApplication, gtimer_application, GTK_TYPE_APPLICATION);

static void
gtimer_application_action_about (GSimpleAction *action,
				 GVariant      *parameter,
				 gpointer       user_data)
{
	GtkWindow *window;

	window = gtk_application_get_active_window (GTK_APPLICATION (user_data));
	g_return_if_fail (GTIMER_IS_WINDOW (window));

	gtimer_window_show_about_dialog (GTIMER_WINDOW (window));
}

static void
gtimer_application_action_quit (GSimpleAction *action,
				GVariant      *parameter,
				gpointer       user_data)
{
	GList *windows;

	windows = gtk_application_get_windows (GTK_APPLICATION (user_data));

	g_list_foreach (windows, (GFunc) gtimer_window_close, NULL);
}

// FIXME: menu not showing
static GActionEntry app_entries[] = {
	{ "about", gtimer_application_action_about, NULL, NULL, NULL },
	{ "quit",  gtimer_application_action_quit,  NULL, NULL, NULL },
};

static void
gtimer_application_init_app_menu (GtkApplication *application)
{
	GMenu *menu;

	g_action_map_add_action_entries (G_ACTION_MAP (application),
					 app_entries, G_N_ELEMENTS (app_entries),
					 application);

	menu = g_menu_new ();
	g_menu_append (menu, _("About"), "app.about");
	g_menu_append (menu, _("Quit"), "app.quit");
	gtk_application_set_app_menu (application, G_MENU_MODEL (menu));
	g_object_unref (menu);
}

static void
gtimer_application_init_style (void)
{
	GtkCssProvider *css_provider;
	GFile *file = NULL;

	css_provider = gtk_css_provider_new ();
	file = g_file_new_for_uri ("resource:///org/unia/gtimeutils/gnome-clocks.css");
	if (gtk_css_provider_load_from_file (css_provider, file, NULL)) {
		GdkScreen *screen;

		screen = gdk_screen_get_default ();
		gtk_style_context_add_provider_for_screen (screen,
							   GTK_STYLE_PROVIDER (css_provider),
							   GTK_STYLE_PROVIDER_PRIORITY_FALLBACK);
	}

	g_object_unref (css_provider);
	g_object_unref (file);
}

static void
gtimer_application_startup (GApplication *application)
{
	G_APPLICATION_CLASS (gtimer_application_parent_class)->startup (application);

	gtk_window_set_default_icon_name ("gnome-clocks");
	g_set_application_name (_("Timer"));

	gtimer_application_init_app_menu (GTK_APPLICATION (application));
	gtimer_application_init_style ();
}

static void
gtimer_application_activate (GApplication *application)
{
	GTimerWindow *window;

	window = gtimer_window_new (GTIMER_APPLICATION (application));
	gtk_window_present_with_time (GTK_WINDOW (window), GDK_CURRENT_TIME);
}

static void
gtimer_application_class_init (GTimerApplicationClass *gtimer_application_class)
{
	GApplicationClass *application_class = G_APPLICATION_CLASS (gtimer_application_class);

	application_class->startup = gtimer_application_startup;
	application_class->activate = gtimer_application_activate;
}

static void
gtimer_application_init (GTimerApplication *application)
{
}

GTimerApplication *
gtimer_application_new (void)
{
	GObject *application;

	application = g_object_new (GTIMER_TYPE_APPLICATION,
				    "application-id", "org.unia.gtimeutils.gtimer",
				    "flags", G_APPLICATION_FLAGS_NONE,
				    NULL);

	return GTIMER_APPLICATION (application);
}

