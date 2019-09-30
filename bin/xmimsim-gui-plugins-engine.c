/*
Copyright (C) 2019 Tom Schoonjans and Laszlo Vincze

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// This file is heavily influenced by GeditPluginsEngine

#include <config.h>
#include "xmimsim-gui-plugins-engine.h"
#include <libpeas/peas.h>

#ifdef MAC_INTEGRATION
  #include "xmi_resources_mac.h"
#endif

#ifdef G_OS_WIN32
  #include "xmi_registry_win.h"
#endif

struct _XmiMsimGuiPluginsEngine {
	PeasEngine parent_instance;
};

struct _XmiMsimGuiPluginsEngineClass {
	PeasEngineClass parent_class;
};

G_DEFINE_TYPE(XmiMsimGuiPluginsEngine, xmi_msim_gui_plugins_engine, PEAS_TYPE_ENGINE)

static XmiMsimGuiPluginsEngine *default_engine = NULL;

struct gir_pair {
	gchar *namespace;
	gchar *version;
};

static struct gir_pair gir_pairs[] = {
	{"Peas", "1.0"},
	{"XmiMsim", "1.0"},
	{"XmiMsimGui", "1.0"},
};

static void plugin_load(PeasPluginInfo *info, XmiMsimGuiPluginsEngine *engine) {
	if (!peas_engine_load_plugin(PEAS_ENGINE(engine), info)) {
		g_warning("Could not load plugin %s", peas_plugin_info_get_name(info));
	}
}

static void xmi_msim_gui_plugins_engine_init(XmiMsimGuiPluginsEngine *engine) {
	GError *error = NULL;
	unsigned int i;

	peas_engine_enable_loader(PEAS_ENGINE(engine), "python3");

	/* Require XMI-MSIM's typelibs. */
	// TODO FIXME for Windows/macOS, this will be complicated and will require fooling around with g_irepository_prepend_search_path (https://developer.gnome.org/gi/stable/GIRepository.html#g-irepository-prepend-search-path)
	
	

	for (i = 0 ; i < G_N_ELEMENTS(gir_pairs) ; i++) {

		if (!g_irepository_require(g_irepository_get_default (), gir_pairs[i].namespace, gir_pairs[i].version, 0, &error)) {
			g_warning("Could not load %s repository: %s", gir_pairs[i].namespace, error->message);
			g_error_free(error);
			error = NULL;
			break;
		}
	}

	gchar *sources_dir = NULL;

	// system-wide plugins
#ifdef G_OS_WIN32
	if (xmi_registry_win_query(XMI_REGISTRY_WIN_SOURCES, &sources_dir) == 0) {
		g_critical("Could not get sources location in registry!");
		return;
	}
#elif defined(MAC_INTEGRATION)
	if (xmi_resources_mac_query(XMI_RESOURCES_MAC_SOURCES, &sources_dir) == 0) {
		g_critical("Could not get sources location in resources!");
		return;
	}
#else
	sources_dir = g_strdup(XMIMSIM_SOURCES_DEFAULT);
#endif
	peas_engine_add_search_path(PEAS_ENGINE(engine), sources_dir, NULL);
	g_free(sources_dir);

#ifdef MAC_INTEGRATION
        const gchar *config_dir = xmi_resources_mac_get_user_data_dir();
#else
        const gchar *config_dir = g_get_user_config_dir();
#endif

	sources_dir = g_strdup_printf("%s" G_DIR_SEPARATOR_S "XMI-MSIM" G_DIR_SEPARATOR_S "sources", config_dir);
	peas_engine_add_search_path(PEAS_ENGINE(engine), sources_dir, NULL);

	const GList* plugin_list = peas_engine_get_plugin_list(PEAS_ENGINE(engine));

	g_list_foreach((GList *) plugin_list, (GFunc) plugin_load, engine);
}

static void xmi_msim_gui_plugins_engine_dispose(GObject *object) {
	XmiMsimGuiPluginsEngine *engine = XMI_MSIM_GUI_PLUGINS_ENGINE (object);

	G_OBJECT_CLASS(xmi_msim_gui_plugins_engine_parent_class)->dispose (object);
}

static void xmi_msim_gui_plugins_engine_class_init(XmiMsimGuiPluginsEngineClass *klass) {
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = xmi_msim_gui_plugins_engine_dispose;
}

XmiMsimGuiPluginsEngine* xmi_msim_gui_plugins_engine_get_default(void) {
	if (default_engine == NULL) {
		default_engine = XMI_MSIM_GUI_PLUGINS_ENGINE(g_object_new(XMI_MSIM_GUI_TYPE_PLUGINS_ENGINE, NULL));

		g_object_add_weak_pointer(G_OBJECT(default_engine), (gpointer) &default_engine);
	}

	return default_engine;
}
