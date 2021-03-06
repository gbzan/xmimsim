/*
Copyright (C) 2016-2017 Tom Schoonjans and Laszlo Vincze

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

#include <config.h>
#include "xmimsim-gui-source-radionuclide.h"
#include "xmimsim-gui-prefs.h"
#include "xmimsim-gui-utils.h"
#include "xmi_aux.h"
#include <xraylib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <libpeas/peas.h>

enum {
	ACTIVITY_UNIT_mCi = 0,
	ACTIVITY_UNIT_Ci,
	ACTIVITY_UNIT_GBq,
	ACTIVITY_UNIT_Bq,
};

struct _XmiMsimGuiSourceRadionuclidePrivate
{
  	// all our widgets
	GtkWidget *radioNuclideW;
	GtkWidget *activityW;
	GtkWidget *activityUnitW;
	GtkWidget *nuclideSolidAngleW;
	gboolean dispose_called;
};

typedef struct {
	int radioNuclide;
	int activityUnit;
	double activity;
	double nuclide_solid_angle;
} xmi_nuclide_parameters;

static const gchar *activity_units[4] = {"mCi", "Ci", "GBq", "Bq"};

G_DEFINE_TYPE_WITH_PRIVATE(XmiMsimGuiSourceRadionuclide, xmi_msim_gui_source_radionuclide, XMI_MSIM_GUI_TYPE_SOURCE_ABSTRACT)

G_MODULE_EXPORT void peas_register_radionuclides_types(PeasObjectModule *module);

G_MODULE_EXPORT void peas_register_radionuclides_types(PeasObjectModule *module) {
	g_debug("Entering tube peas_register_radionuclides_types");

	peas_object_module_register_extension_type(module, XMI_MSIM_GUI_TYPE_SOURCE_ABSTRACT, XMI_MSIM_GUI_TYPE_SOURCE_RADIONUCLIDE);
}

static void xmi_msim_gui_source_radionuclide_real_generate(XmiMsimGuiSourceAbstract *source);

static const gchar *xmi_msim_gui_source_radionuclide_real_get_name(XmiMsimGuiSourceAbstract *source);

static const gchar *xmi_msim_gui_source_radionuclide_real_get_about_text(XmiMsimGuiSourceAbstract *source);

static void xmi_msim_gui_source_radionuclide_dispose(GObject *object);

static void xmi_msim_gui_source_radionuclide_finalize(GObject *object);

static void slits_button_clicked_cb(XmiMsimGuiSourceRadionuclide *source);

static void xmi_msim_gui_source_radionuclide_class_init(XmiMsimGuiSourceRadionuclideClass *klass) {

	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->dispose = xmi_msim_gui_source_radionuclide_dispose;
	object_class->finalize = xmi_msim_gui_source_radionuclide_finalize;

	XmiMsimGuiSourceAbstractClass *parent_klass = XMI_MSIM_GUI_SOURCE_ABSTRACT_CLASS(klass);

	parent_klass->generate = xmi_msim_gui_source_radionuclide_real_generate;
	//parent_klass->save = xmi_msim_gui_source_radionuclide_real_save; // do not override
	parent_klass->get_source_name = xmi_msim_gui_source_radionuclide_real_get_name;
	parent_klass->get_about_text = xmi_msim_gui_source_radionuclide_real_get_about_text;
}

static xmi_nuclide_parameters* get_parameters(XmiMsimGuiSourceRadionuclide *source, GError **error) {
	xmi_nuclide_parameters *xnp = (xmi_nuclide_parameters *) g_malloc(sizeof(xmi_nuclide_parameters));

	xnp->radioNuclide = gtk_combo_box_get_active(GTK_COMBO_BOX(source->priv->radioNuclideW));
	xnp->activityUnit = gtk_combo_box_get_active(GTK_COMBO_BOX(source->priv->activityUnitW));
	
	const gchar *text = gtk_entry_get_text(GTK_ENTRY(source->priv->activityW));
	gchar *endPtr;
	xnp->activity = strtod(text, &endPtr);
	if (strlen(text) == 0 || text + strlen(text) != endPtr || xnp->activity <= 0.0) {
		g_set_error(error, XMI_MSIM_GUI_SOURCE_RADIONUCLIDE_ERROR, XMI_MSIM_GUI_SOURCE_RADIONUCLIDE_ERROR_INVALID_DATA, "Invalid activity: must be greater than zero");
		g_free(xnp);
		return NULL;
	}
	text = gtk_entry_get_text(GTK_ENTRY(source->priv->nuclideSolidAngleW));
	xnp->nuclide_solid_angle = strtod(text, &endPtr);
	if (strlen(text) == 0 || text + strlen(text) != endPtr || xnp->nuclide_solid_angle <= 0.0 || xnp->nuclide_solid_angle >= 2.0*M_PI) {
		g_set_error(error, XMI_MSIM_GUI_SOURCE_RADIONUCLIDE_ERROR, XMI_MSIM_GUI_SOURCE_RADIONUCLIDE_ERROR_INVALID_DATA, "Invalid solid angle: must be greater than zero and less than 2\317\200");
		g_free(xnp);
		return NULL;
	}

	return xnp;
}

static void set_parameters(XmiMsimGuiSourceRadionuclide *source, xmi_nuclide_parameters *xnp) {
	gchar *buf;

	gtk_combo_box_set_active(GTK_COMBO_BOX(source->priv->radioNuclideW), xnp->radioNuclide);
	gtk_combo_box_set_active(GTK_COMBO_BOX(source->priv->activityUnitW), xnp->activityUnit);

	buf = g_strdup_printf("%g", xnp->activity);
	gtk_entry_set_text(GTK_ENTRY(source->priv->activityW), buf);
	g_free(buf);

	buf = g_strdup_printf("%g", xnp->nuclide_solid_angle);
	gtk_entry_set_text(GTK_ENTRY(source->priv->nuclideSolidAngleW), buf);
	g_free(buf);
}

static void set_preferences(xmi_nuclide_parameters *xnp) {
	gchar *prefs_file;
	GKeyFile *keyfile;

	prefs_file = xmimsim_gui_get_preferences_filename();

	keyfile = g_key_file_new();

	if (!g_key_file_load_from_file(keyfile, prefs_file, (GKeyFileFlags) (G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS), NULL)) {
		if (!xmimsim_gui_create_prefs_file(keyfile, prefs_file)) {
			g_error("Could not create preferences file %s with default settings!", prefs_file);
			return;
		}
	}
	g_key_file_set_double(keyfile, "Radionuclide last used", "Activity", xnp->activity);
	g_key_file_set_double(keyfile, "Radionuclide last used", "Solid angle", xnp->nuclide_solid_angle);
	g_key_file_set_string(keyfile, "Radionuclide last used", "Unit", activity_units[xnp->activityUnit]);
	int nNuclides;
	gchar **nuclides = GetRadioNuclideDataList(&nNuclides, NULL);
	if (xnp->radioNuclide < 0 || xnp->radioNuclide >= nNuclides) {
		g_warning("Invalid radioNuclide %i detected\n", xnp->radioNuclide);
	}
	else {
		g_key_file_set_string(keyfile, "Radionuclide last used", "Radionuclide", nuclides[xnp->radioNuclide]);
	}
	int i;
	for (i = 0 ; i < nNuclides ; i++)
		g_free(nuclides[i]);
	g_free(nuclides);

	//save file
	gchar *prefs_file_contents = g_key_file_to_data(keyfile, NULL, NULL);
	GError *error = NULL;
	if(!g_file_set_contents(prefs_file, prefs_file_contents, -1, &error)) {
		g_error("Could not write to %s: %s\n", prefs_file, error->message);
	}
	g_free(prefs_file_contents);
	g_free(prefs_file);
	g_key_file_free(keyfile);

	return;
}

static xmi_nuclide_parameters* get_preferences() {
	xmi_nuclide_parameters *xnp = (xmi_nuclide_parameters *) g_malloc(sizeof(xmi_nuclide_parameters));

	gchar *prefs_file;
	GKeyFile *keyfile;

	prefs_file = xmimsim_gui_get_preferences_filename();

	keyfile = g_key_file_new();

	if (!g_key_file_load_from_file(keyfile, prefs_file, (GKeyFileFlags) (G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS), NULL)) {
		if (!xmimsim_gui_create_prefs_file(keyfile, prefs_file))
			g_error("Could not create preferences file %s with default settings!", prefs_file);
	}
	
	gboolean update_file = FALSE;
	GError *error = NULL;

	xnp->activity= g_key_file_get_double(keyfile, "Radionuclide last used", "Activity", &error);
	if (error != NULL) {
		g_warning("Radionuclide last used Activity not found in preferences file\n");
		g_key_file_set_double(keyfile, "Radionuclide last used", "Activity", 100.0);
		xnp->activity = 100.0;
		error = NULL;
		update_file = TRUE;
	}
	gchar *unit = g_key_file_get_string(keyfile, "Radionuclide last used", "Unit", &error);
	if (error != NULL) {
		g_warning("Radionuclide last used Unit not found in preferences file\n");
		g_key_file_set_string(keyfile, "Radionuclide last used", "Unit", activity_units[ACTIVITY_UNIT_mCi]);
		xnp->activityUnit = ACTIVITY_UNIT_mCi;
		error = NULL;
		update_file = TRUE;
	}
	else {
		gboolean matched = FALSE;
		int i;
		for (i = 0 ; i < 4 ; i++) {
			if (strcmp(activity_units[i], unit) == 0) {
				xnp->activityUnit = i;
				matched = TRUE;
				break;
			}
		}
		if (!matched) {
			g_error("Could not find a match for Radionuclide last used Unit in preferences file\n");
			xnp->activityUnit = ACTIVITY_UNIT_mCi;
			g_key_file_set_string(keyfile, "Radionuclide last used", "Unit", activity_units[ACTIVITY_UNIT_mCi]);
			update_file = TRUE;
		}
		g_free(unit);
	}

	xnp->nuclide_solid_angle = g_key_file_get_double(keyfile, "Radionuclide last used", "Solid angle", &error);
	if (error != NULL) {
		g_warning("Radionuclide last used Solid angle not found in preferences file\n");
		g_key_file_set_double(keyfile, "Radionuclide last used", "Solid angle", 1.0);
		xnp->nuclide_solid_angle = 1.0;
		error = NULL;
		update_file = TRUE;
	}

	int nNuclides;
	gchar **nuclides = GetRadioNuclideDataList(&nNuclides, NULL);
	gchar *nuclide = g_key_file_get_string(keyfile, "Radionuclide last used", "Radionuclide", &error);

	if (error != NULL) {
		g_warning("Radionuclide last used Radionuclide not found in preferences file\n");
		g_key_file_set_string(keyfile, "Radionuclide last used", "Radionuclide", nuclides[0]);
		xnp->radioNuclide = 0;
		error = NULL;
		update_file = TRUE;
	}
	else {
		gboolean matched = FALSE;
		int i;
		for (i = 0 ; i < nNuclides ; i++) {
			if (strcmp(nuclides[i], nuclide) == 0) {
				xnp->radioNuclide= i;
				matched = TRUE;
				break;
			}
		}
		if (!matched) {
			g_error("Could not find a match for Radionuclide last used Radionuclide in preferences file\n");
			g_key_file_set_string(keyfile, "Radionuclide last used", "Radionuclide", nuclides[0]);
			xnp->radioNuclide = 0;
			update_file = TRUE;
		}
		g_free(nuclide);
	}
	int i;
	for (i = 0 ; i < nNuclides ; i++)
		g_free(nuclides[i]);
	g_free(nuclides);

	if (update_file) {
		//save file
		gchar *prefs_file_contents = g_key_file_to_data(keyfile, NULL, NULL);
		if(!g_file_set_contents(prefs_file, prefs_file_contents, -1, &error))
			g_error("Could not write to %s: %s\n", prefs_file, error->message);
		g_free(prefs_file_contents);
	}
	g_free(prefs_file);
	g_key_file_free(keyfile);
	return xnp;
}

static void xmi_msim_gui_source_radionuclide_init(XmiMsimGuiSourceRadionuclide *source) {
	source->priv = xmi_msim_gui_source_radionuclide_get_instance_private(source);
	source->priv->dispose_called = FALSE;

	// construct the widgets, and set them to their values as we go along...
	GtkWidget *mainVBox = GTK_WIDGET(source);

	GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_box_set_homogeneous(GTK_BOX(hbox), FALSE);
	gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new("Radionuclide"), FALSE, FALSE, 2);
	source->priv->radioNuclideW = gtk_combo_box_text_new();

	gchar **nuclides;
	int nNuclides, i;
	nuclides = GetRadioNuclideDataList(&nNuclides, NULL);
	for (i = 0 ; i < nNuclides ; i++) {
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(source->priv->radioNuclideW), nuclides[i]);
		g_free(nuclides[i]);
	}
	g_free(nuclides);
	gtk_box_pack_end(GTK_BOX(hbox), source->priv->radioNuclideW, FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(mainVBox), hbox, FALSE, FALSE, 2);
	gtk_widget_show_all(hbox);


	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_box_set_homogeneous(GTK_BOX(hbox), FALSE);
	gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new("Activity"), FALSE, FALSE, 2);
	source->priv->activityW = gtk_entry_new();

	source->priv->activityUnitW = gtk_combo_box_text_new();
	for (i = 0 ; i < 4 ; i++) {
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(source->priv->activityUnitW), activity_units[i]);
	}
	gtk_box_pack_end(GTK_BOX(hbox), source->priv->activityUnitW, FALSE, FALSE, 2);
	gtk_box_pack_end(GTK_BOX(hbox), source->priv->activityW, FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(mainVBox), hbox, FALSE, FALSE, 2);
	gtk_widget_show_all(hbox);

	source->priv->nuclideSolidAngleW = gtk_entry_new();
	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_box_set_homogeneous(GTK_BOX(hbox), FALSE);
	GtkWidget *label = gtk_label_new("Source solid angle (sr)");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 2);
	GtkWidget *slitsButton = gtk_button_new_with_label("Get from slits");
	gtk_box_pack_end(GTK_BOX(hbox), slitsButton, FALSE, FALSE, 2);
	gtk_box_pack_end(GTK_BOX(hbox), source->priv->nuclideSolidAngleW, FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(mainVBox), hbox, FALSE, FALSE, 2);
	gtk_widget_show_all(hbox);

	// load the preferences
	xmi_nuclide_parameters *xnp = get_preferences();
	
	set_parameters(source, xnp);

	g_free(xnp);

	// signal handlers
	g_signal_connect_swapped(G_OBJECT(slitsButton), "clicked", G_CALLBACK(slits_button_clicked_cb), (gpointer) source);

}

static void slits_button_clicked_cb(XmiMsimGuiSourceRadionuclide *source) {
	//calculate solid angle based on slits
	xmi_input *current = NULL;
	g_object_get(source, "xmi-input-current", &current, NULL);
	double solid_angle = xmi_msim_gui_utils_get_solid_angle_from_slits(current->geometry);
	xmi_input_free(current);

	gchar *buf = g_strdup_printf("%g", solid_angle);
	gtk_entry_set_text(GTK_ENTRY(source->priv->nuclideSolidAngleW), buf);
	g_free(buf);
	return;
}

static void xmi_msim_gui_source_radionuclide_real_generate(XmiMsimGuiSourceAbstract *source) {
	GError *error = NULL;

	// read the parameters
	xmi_nuclide_parameters *xnp = get_parameters(XMI_MSIM_GUI_SOURCE_RADIONUCLIDE(source), &error);

	if (xnp == NULL) {
		g_signal_emit_by_name((gpointer) source, "after-generate", error);
		g_error_free(error);
		return;
	}

	const double nuclide_solid_angle_fraction = 1.0/(4.0*M_PI);
	double activity = xnp->activity * nuclide_solid_angle_fraction * xnp->nuclide_solid_angle;

	if (xnp->activityUnit == ACTIVITY_UNIT_mCi) {
		activity *= 3.7E7;
	}
	else if (xnp->activityUnit == ACTIVITY_UNIT_Ci) {
		activity *= 3.7E10;
	}
	else if (xnp->activityUnit == ACTIVITY_UNIT_GBq) {
		activity *= 1E9;
	}
	else if (xnp->activityUnit == ACTIVITY_UNIT_Bq) {
		//do nothing
	}

	struct radioNuclideData *rnd = GetRadioNuclideDataByIndex(xnp->radioNuclide, NULL);

	int i;

	xmi_excitation *excitation_nuclide = (xmi_excitation *) g_malloc(sizeof(xmi_excitation));
	excitation_nuclide->n_continuous = 0;
	excitation_nuclide->continuous = NULL;
	excitation_nuclide->n_discrete= 0;
	excitation_nuclide->discrete= NULL;

	double plot_xmax = 0.0;

	for (i = 0 ; i < rnd->nXrays ; i++) {
		double energy = LineEnergy(rnd->Z_xray, rnd->XrayLines[i], NULL);
		if (energy < 1.0 || energy > 200.0)
			continue;

		if (energy > plot_xmax)
			plot_xmax = energy;

		excitation_nuclide->discrete = (xmi_energy_discrete *) g_realloc(excitation_nuclide->discrete, sizeof(xmi_energy_discrete)*++excitation_nuclide->n_discrete);
		excitation_nuclide->discrete[excitation_nuclide->n_discrete-1].energy = energy;
		excitation_nuclide->discrete[excitation_nuclide->n_discrete-1].horizontal_intensity =
		excitation_nuclide->discrete[excitation_nuclide->n_discrete-1].vertical_intensity =
		rnd->XrayIntensities[i] * activity/2.0;
		excitation_nuclide->discrete[excitation_nuclide->n_discrete-1].sigma_x =
		excitation_nuclide->discrete[excitation_nuclide->n_discrete-1].sigma_xp =
		excitation_nuclide->discrete[excitation_nuclide->n_discrete-1].sigma_y =
		excitation_nuclide->discrete[excitation_nuclide->n_discrete-1].sigma_yp =
		0.0;
		excitation_nuclide->discrete[excitation_nuclide->n_discrete-1].distribution_type = XMI_ENERGY_DISCRETE_DISTRIBUTION_MONOCHROMATIC;
		excitation_nuclide->discrete[excitation_nuclide->n_discrete-1].scale_parameter= 0.0;
	}
	for (i = 0 ; i < rnd->nGammas ; i++) {
		double energy = rnd->GammaEnergies[i];
		if (energy < 1.0 || energy > 200.0)
			continue;

		if (energy > plot_xmax)
			plot_xmax = energy;

		excitation_nuclide->discrete = (xmi_energy_discrete *) g_realloc(excitation_nuclide->discrete, sizeof(xmi_energy_discrete)*++excitation_nuclide->n_discrete);
		excitation_nuclide->discrete[excitation_nuclide->n_discrete-1].energy = energy;
		excitation_nuclide->discrete[excitation_nuclide->n_discrete-1].horizontal_intensity =
		excitation_nuclide->discrete[excitation_nuclide->n_discrete-1].vertical_intensity =
		rnd->GammaIntensities[i]*activity/2.0;
		excitation_nuclide->discrete[excitation_nuclide->n_discrete-1].sigma_x =
		excitation_nuclide->discrete[excitation_nuclide->n_discrete-1].sigma_xp =
		excitation_nuclide->discrete[excitation_nuclide->n_discrete-1].sigma_y =
		excitation_nuclide->discrete[excitation_nuclide->n_discrete-1].sigma_yp =
		0.0;
		excitation_nuclide->discrete[excitation_nuclide->n_discrete-1].distribution_type = XMI_ENERGY_DISCRETE_DISTRIBUTION_MONOCHROMATIC;
		excitation_nuclide->discrete[excitation_nuclide->n_discrete-1].scale_parameter= 0.0;
	}
	FreeRadioNuclideData(rnd);


	GArray *x = g_array_sized_new(FALSE, FALSE, sizeof(double), 1000);
	GArray *y = g_array_sized_new(FALSE, FALSE, sizeof(double), 1000);

	for (i = 0 ; i < 1000 ; i++) {
		double energy = i * plot_xmax/999.0;
		double intensity = 0.0;
		g_array_append_val(x, energy);
		g_array_append_val(y, intensity);
	}
	for (i = 0 ; i < excitation_nuclide->n_discrete ; i++) {
		int channel = (int) floor(excitation_nuclide->discrete[i].energy * 999.0/plot_xmax);
		double *intensity = &g_array_index(y, double, channel); 
		*intensity += excitation_nuclide->discrete[i].horizontal_intensity*2.0;
	}

	// find the smallest value greater than zero (1E-1)
	double ymax = xmi_maxval_double((double *) y->data, y->len);
	if (ymax < 1.0) {
		g_set_error(&error, XMI_MSIM_GUI_SOURCE_RADIONUCLIDE_ERROR, XMI_MSIM_GUI_SOURCE_RADIONUCLIDE_ERROR_MAXIMUM, "Maximum value is too low: %f\nConsider changing the parameters", ymax);
		g_free(xnp);
		g_array_free(x, TRUE);
		g_array_free(y, TRUE);
		g_signal_emit_by_name((gpointer) source, "after-generate", error);
		g_error_free(error);
		return;
	}
	double new_min = ymax;
	for (i = 0 ; i < y->len ; i++) {
		if (g_array_index(y, double, i) < new_min && g_array_index(y, double, i) > 1E-1)
			new_min = g_array_index(y, double, i);
	}

	for (i = 0 ; i < y->len ; i++) {
		double *intensity = &g_array_index(y, double, i);
		if (*intensity < new_min)
			*intensity = new_min;
	}

	// update member variables
	g_object_set(source, "raw-data", excitation_nuclide, "x", x, "y", y, NULL);

	xmi_excitation_free(excitation_nuclide);
	g_array_unref(x);
	g_array_unref(y);

	g_free(xnp);
	g_signal_emit_by_name((gpointer) source, "after-generate", NULL);

	return;
}

static const gchar *xmi_msim_gui_source_radionuclide_real_get_name(XmiMsimGuiSourceAbstract *source) {
	static const gchar name[] = "Radionuclides";

	return name;
}

static const gchar *xmi_msim_gui_source_radionuclide_real_get_about_text(XmiMsimGuiSourceAbstract *source) {
	static const gchar about_text[] =
		"The X-ray and gamma spectra of the provided radionuclides "
		"have been obtained using the <a href='http://github.com/tschoonj/xraylib/wiki/The-xraylib-API-list-of-all-functions'>xraylib API</a> for radionuclides. "
		"Follow the references in the xraylib documentation in "
		"order to find the origin of the datasets.";
		
	return about_text;
}

static void xmi_msim_gui_source_radionuclide_dispose(GObject *object) {
	XmiMsimGuiSourceRadionuclide *source = XMI_MSIM_GUI_SOURCE_RADIONUCLIDE(object);

	if (source->priv->dispose_called == FALSE) {
		// save current input in preferences if valid
		// this can only occur the first time the dispose method is called though!
		xmi_nuclide_parameters *xnp = get_parameters(source, NULL);
		if (xnp != NULL) {
			set_preferences(xnp);
			g_free(xnp);
		}
		source->priv->dispose_called = TRUE;
	}
	G_OBJECT_CLASS(xmi_msim_gui_source_radionuclide_parent_class)->dispose(object);
}

static void xmi_msim_gui_source_radionuclide_finalize(GObject *object) {

	G_OBJECT_CLASS(xmi_msim_gui_source_radionuclide_parent_class)->finalize(object);
}

GQuark xmi_msim_gui_source_radionuclide_error_quark(void) {
	return g_quark_from_string("xmi-msim-gui-source-radionuclide-error-quark");
}

static void xmi_msim_gui_source_radionuclide_class_finalize(XmiMsimGuiSourceRadionuclideClass *klass) {

}

