#include <config.h>
#include <math.h>
#include "xmi_gobject.h"
#include "xmi_data_structs.h"
#include "libxmimsim-test.h"
#include <unistd.h>

typedef struct {
	xmi_composition *composition;
} SetupDataComposition;

typedef struct {
	xmi_excitation *excitation;
} SetupDataExcitation;

typedef struct {
	xmi_input *input;
} SetupDataInput;

typedef struct {
	xmi_archive *archive;
} SetupDataArchive;

typedef struct {
	xmi_main_options *main_options;
} SetupDataMainOptions;

static gboolean composition_equal(xmi_composition *A, xmi_composition *B) {
	int i, j;

	if (A->n_layers != B->n_layers) {
		return FALSE;
	}
	else if (A->reference_layer != B->reference_layer) {
		return FALSE;
	}
	else {
		for (i = 0 ; i < A->n_layers ; i++) {
			if (A->layers[i].n_elements != B->layers[i].n_elements) {
				return FALSE;
			}
			else {
				for (j = 0 ; j < A->layers[i].n_elements ; j++) {
					if (A->layers[i].Z[j] != B->layers[i].Z[j]) {
						return FALSE;
					}
					else if (fabs(A->layers[i].weight[j]- B->layers[i].weight[j])/A->layers[i].weight[j] >XMI_COMPARE_THRESHOLD) {
						return FALSE;
					}
				}
				if (fabs(A->layers[i].density - B->layers[i].density)/A->layers[i].density > XMI_COMPARE_THRESHOLD) {
					return FALSE;
				}
				if (fabs(A->layers[i].thickness- B->layers[i].thickness)/A->layers[i].thickness > XMI_COMPARE_THRESHOLD) {
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}

static gboolean excitation_equal(xmi_excitation *A, xmi_excitation *B) {
	int i, j;
#define XMI_IF_COMPARE_EXCITATION_DISCRETE(a) if (fabs(A->discrete[i].a-B->discrete[i].a)/A->discrete[i].a > XMI_COMPARE_THRESHOLD) {\
					return FALSE;\
				}
	if (A->n_discrete > 0 || B->n_discrete > 0) {
		if (A->n_discrete != B->n_discrete) {
			return FALSE;
		}
		else {
			for (i = 0 ; i < A->n_discrete ; i++) {
				XMI_IF_COMPARE_EXCITATION_DISCRETE(energy)
				XMI_IF_COMPARE_EXCITATION_DISCRETE(horizontal_intensity)
				XMI_IF_COMPARE_EXCITATION_DISCRETE(vertical_intensity)
				XMI_IF_COMPARE_EXCITATION_DISCRETE(sigma_x)
				XMI_IF_COMPARE_EXCITATION_DISCRETE(sigma_xp)
				XMI_IF_COMPARE_EXCITATION_DISCRETE(sigma_y)
				XMI_IF_COMPARE_EXCITATION_DISCRETE(sigma_yp)
				if (A->discrete[i].distribution_type != B->discrete[i].distribution_type) {
					return FALSE;
				}
				else if (A->discrete[i].distribution_type != XMI_ENERGY_DISCRETE_DISTRIBUTION_MONOCHROMATIC) {
					XMI_IF_COMPARE_EXCITATION_DISCRETE(scale_parameter)
				}
			}
		}
	}

#define XMI_IF_COMPARE_EXCITATION_CONTINUOUS(a) if (fabs(A->continuous[i].a-B->continuous[i].a)/A->continuous[i].a > XMI_COMPARE_THRESHOLD) {\
					return FALSE;\
				}
	if (A->n_continuous > 0 || B->n_continuous > 0) {
		if (A->n_continuous != B->n_continuous) {
			return FALSE;
		}
		else {
			for (i = 0 ; i < A->n_continuous ; i++) {
				XMI_IF_COMPARE_EXCITATION_CONTINUOUS(energy)
				XMI_IF_COMPARE_EXCITATION_CONTINUOUS(horizontal_intensity)
				XMI_IF_COMPARE_EXCITATION_CONTINUOUS(vertical_intensity)
				XMI_IF_COMPARE_EXCITATION_CONTINUOUS(sigma_x)
				XMI_IF_COMPARE_EXCITATION_CONTINUOUS(sigma_xp)
				XMI_IF_COMPARE_EXCITATION_CONTINUOUS(sigma_y)
				XMI_IF_COMPARE_EXCITATION_CONTINUOUS(sigma_yp)
			}
		}
	}
	return TRUE;
}

static gboolean main_options_equal(xmi_main_options *A, xmi_main_options *B) {
	if (A->use_M_lines != B->use_M_lines)
		return FALSE;
	if (A->use_cascade_radiative != B->use_cascade_radiative)
		return FALSE;
	if (A->use_cascade_auger != B->use_cascade_auger)
		return FALSE;
	if (A->use_variance_reduction != B->use_variance_reduction)
		return FALSE;
	if (A->use_sum_peaks != B->use_sum_peaks)
		return FALSE;
	if (A->use_escape_peaks != B->use_escape_peaks)
		return FALSE;
	if (A->escape_ratios_mode != B->escape_ratios_mode)
		return FALSE;
	if (A->verbose != B->verbose)
		return FALSE;
	if (A->use_poisson != B->use_poisson)
		return FALSE;
	if (A->use_gpu != B->use_gpu)
		return FALSE;
	if (A->omp_num_threads != B->omp_num_threads)
		return FALSE;
	if (A->extra_verbose != B->extra_verbose)
		return FALSE;
	if (A->use_advanced_compton != B->use_advanced_compton)
		return FALSE;
	if (A->use_default_seeds != B->use_default_seeds)
		return FALSE;
	if (g_strcmp0(A->custom_detector_response, B->custom_detector_response) != 0)
		return FALSE;
	return TRUE;	
}

static void setup_data_composition(SetupDataComposition *data, gconstpointer user_data) {
	data->composition = g_malloc(sizeof(xmi_composition));
	data->composition->reference_layer = 1;
	data->composition->n_layers = 1;
	data->composition->layers = g_malloc(sizeof(xmi_layer));
	data->composition->layers[0].n_elements = 2;
	data->composition->layers[0].Z = g_malloc(sizeof(int) * 2);
	data->composition->layers[0].weight = g_malloc(sizeof(double) * 2);
	data->composition->layers[0].Z[0] = 8;
	data->composition->layers[0].Z[1] = 14;
	data->composition->layers[0].weight[0] = 0.4;
	data->composition->layers[0].weight[1] = 0.6;
	data->composition->layers[0].density = 10.0;
	data->composition->layers[0].thickness = 1E-4;
}

static void setup_data_excitation(SetupDataExcitation *data, gconstpointer user_data) {
	data->excitation = g_malloc(sizeof(xmi_excitation));
	data->excitation->n_discrete = 1;
	data->excitation->n_continuous = 0;
	data->excitation->continuous = NULL;
	data->excitation->discrete = g_malloc(sizeof(xmi_energy_discrete));
	data->excitation->discrete[0].energy = 28.0;
	data->excitation->discrete[0].horizontal_intensity= 1E12;
	data->excitation->discrete[0].vertical_intensity= 1E9;
	data->excitation->discrete[0].sigma_x= 0.0;
	data->excitation->discrete[0].sigma_xp= 0.0;
	data->excitation->discrete[0].sigma_y= 0.0;
	data->excitation->discrete[0].sigma_yp= 0.0;
	data->excitation->discrete[0].scale_parameter = 0.0;
	data->excitation->discrete[0].distribution_type = XMI_ENERGY_DISCRETE_DISTRIBUTION_MONOCHROMATIC;
}

static void setup_data_input(SetupDataInput *data, gconstpointer user_data) {
	g_assert_nonnull(data->input = xmi_input_read_from_xml_file(TEST_XMSI, NULL));
}

static void setup_data_archive(SetupDataArchive *data, gconstpointer user_data) {
	g_assert_nonnull(data->archive = xmi_archive_read_from_xml_file(TEST_XMSA_1_OLD, NULL));
}

static void setup_data_main_options(SetupDataMainOptions *data, gconstpointer user_data) {
	data->main_options = xmi_main_options_new();
}

static void teardown_data_composition(SetupDataComposition *data, gconstpointer user_data) {
	xmi_composition_free(data->composition);
}

static void teardown_data_excitation(SetupDataExcitation *data, gconstpointer user_data) {
	xmi_excitation_free(data->excitation);
}

static void teardown_data_input(SetupDataInput *data, gconstpointer user_data) {
	xmi_input_free(data->input);
}

static void teardown_data_archive(SetupDataArchive *data, gconstpointer user_data) {
	if (data->archive)
		xmi_archive_unref(data->archive);
}

static void teardown_data_main_options(SetupDataMainOptions *data, gconstpointer user_data) {
	xmi_main_options_free(data->main_options);
}

static void test_composition_static(SetupDataComposition *data, gconstpointer user_data) {
	GValue value = G_VALUE_INIT;

	g_value_init(&value, XMI_MSIM_TYPE_COMPOSITION);
	g_value_set_static_boxed(&value, data->composition);
	g_assert(data->composition == g_value_get_boxed(&value));
	g_value_unset(&value);
}

static void test_composition_take(SetupDataComposition *data, gconstpointer user_data) {
	GValue value = G_VALUE_INIT;

	g_value_init(&value, XMI_MSIM_TYPE_COMPOSITION);
	g_value_take_boxed(&value, data->composition);
	g_assert(data->composition == g_value_get_boxed(&value));
	g_value_unset(&value);
}

static void test_composition_null(SetupDataComposition *data, gconstpointer user_data) {
	GValue value = G_VALUE_INIT;

	g_assert_null(data->composition);
	g_value_init(&value, XMI_MSIM_TYPE_COMPOSITION);
	g_value_take_boxed(&value, data->composition);
	g_assert_null(g_value_get_boxed(&value));
	g_value_unset(&value);
}

static void test_composition(SetupDataComposition *data, gconstpointer user_data) {
	GValue value = G_VALUE_INIT;

	g_value_init(&value, XMI_MSIM_TYPE_COMPOSITION);
	g_value_set_boxed(&value, data->composition);
	g_assert(data->composition != g_value_get_boxed(&value));
	g_assert_true(composition_equal(data->composition, g_value_get_boxed(&value)));
	g_value_unset(&value);
}

static void test_excitation_static(SetupDataExcitation *data, gconstpointer user_data) {
	GValue value = G_VALUE_INIT;

	g_value_init(&value, XMI_MSIM_TYPE_EXCITATION);
	g_value_set_static_boxed(&value, data->excitation);
	g_assert(data->excitation == g_value_get_boxed(&value));
	g_value_unset(&value);
}

static void test_excitation_take(SetupDataExcitation *data, gconstpointer user_data) {
	GValue value = G_VALUE_INIT;

	g_value_init(&value, XMI_MSIM_TYPE_EXCITATION);
	g_value_take_boxed(&value, data->excitation);
	g_assert(data->excitation == g_value_get_boxed(&value));
	g_value_unset(&value);
}

static void test_excitation_null(SetupDataExcitation *data, gconstpointer user_data) {
	GValue value = G_VALUE_INIT;

	g_assert_null(data->excitation);
	g_value_init(&value, XMI_MSIM_TYPE_EXCITATION);
	g_value_take_boxed(&value, data->excitation);
	g_assert_null(g_value_get_boxed(&value));
	g_value_unset(&value);
}

static void test_excitation(SetupDataExcitation *data, gconstpointer user_data) {
	GValue value = G_VALUE_INIT;

	g_value_init(&value, XMI_MSIM_TYPE_EXCITATION);
	g_value_set_boxed(&value, data->excitation);
	g_assert(data->excitation != g_value_get_boxed(&value));
	g_assert_true(excitation_equal(data->excitation, g_value_get_boxed(&value)));
	g_value_unset(&value);
}

static void test_input_static(SetupDataInput *data, gconstpointer user_data) {
	GValue value = G_VALUE_INIT;

	g_value_init(&value, XMI_MSIM_TYPE_INPUT);
	g_value_set_static_boxed(&value, data->input);
	g_assert(data->input == g_value_get_boxed(&value));
	g_value_unset(&value);
}

static void test_input_take(SetupDataInput *data, gconstpointer user_data) {
	GValue value = G_VALUE_INIT;

	g_value_init(&value, XMI_MSIM_TYPE_INPUT);
	g_value_take_boxed(&value, data->input);
	g_assert(data->input == g_value_get_boxed(&value));
	g_value_unset(&value);
}

static void test_input_null(SetupDataInput *data, gconstpointer user_data) {
	GValue value = G_VALUE_INIT;

	g_assert_null(data->input);
	g_value_init(&value, XMI_MSIM_TYPE_INPUT);
	g_value_take_boxed(&value, data->input);
	g_assert_null(g_value_get_boxed(&value));
	g_value_unset(&value);
}

static void test_input(SetupDataInput *data, gconstpointer user_data) {
	GValue value = G_VALUE_INIT;

	g_value_init(&value, XMI_MSIM_TYPE_INPUT);
	g_value_set_boxed(&value, data->input);
	g_assert(data->input != g_value_get_boxed(&value));
	g_assert(xmi_input_compare(data->input, g_value_get_boxed(&value)) == 0);
	g_value_unset(&value);
}

static void test_archive_static(SetupDataArchive *data, gconstpointer user_data) {
	GValue value = G_VALUE_INIT;

	g_value_init(&value, XMI_MSIM_TYPE_ARCHIVE);
	g_value_set_static_boxed(&value, data->archive);
	g_assert(data->archive == g_value_get_boxed(&value));
	g_value_unset(&value);
}

static void test_archive_take(SetupDataArchive *data, gconstpointer user_data) {
	GValue value = G_VALUE_INIT;

	g_value_init(&value, XMI_MSIM_TYPE_ARCHIVE);
	g_value_take_boxed(&value, data->archive);
	g_assert(data->archive == g_value_get_boxed(&value));
	g_value_unset(&value);
}

static void test_archive_null(SetupDataArchive *data, gconstpointer user_data) {
	GValue value = G_VALUE_INIT;

	g_assert_null(data->archive);
	g_value_init(&value, XMI_MSIM_TYPE_ARCHIVE);
	g_value_take_boxed(&value, data->archive);
	g_assert_null(g_value_get_boxed(&value));
	g_value_unset(&value);
}

static void test_archive(SetupDataArchive *data, gconstpointer user_data) {
	GValue value = G_VALUE_INIT;

	g_value_init(&value, XMI_MSIM_TYPE_ARCHIVE);
	g_value_set_boxed(&value, data->archive);
	g_assert(data->archive == g_value_get_boxed(&value));
	g_assert_true(xmi_archive_equals(data->archive, g_value_get_boxed(&value)));
	g_value_unset(&value);
}

static void test_main_options_static(SetupDataMainOptions *data, gconstpointer user_data) {
	GValue value = G_VALUE_INIT;

	g_value_init(&value, XMI_MSIM_TYPE_MAIN_OPTIONS);
	g_value_set_static_boxed(&value, data->main_options);
	g_assert(data->main_options == g_value_get_boxed(&value));
	g_value_unset(&value);
}

static void test_main_options_take(SetupDataMainOptions *data, gconstpointer user_data) {
	GValue value = G_VALUE_INIT;

	g_value_init(&value, XMI_MSIM_TYPE_MAIN_OPTIONS);
	g_value_take_boxed(&value, data->main_options);
	g_assert(data->main_options == g_value_get_boxed(&value));
	g_value_unset(&value);
}

static void test_main_options_null(SetupDataMainOptions *data, gconstpointer user_data) {
	GValue value = G_VALUE_INIT;

	g_assert_null(data->main_options);
	g_value_init(&value, XMI_MSIM_TYPE_MAIN_OPTIONS);
	g_value_take_boxed(&value, data->main_options);
	g_assert_null(g_value_get_boxed(&value));
	g_value_unset(&value);
}

static void test_main_options(SetupDataMainOptions *data, gconstpointer user_data) {
	GValue value = G_VALUE_INIT;

	g_value_init(&value, XMI_MSIM_TYPE_MAIN_OPTIONS);
	g_value_set_boxed(&value, data->main_options);
	g_assert(data->main_options != g_value_get_boxed(&value));
	g_assert(main_options_equal(data->main_options, g_value_get_boxed(&value)));
	g_value_unset(&value);
}

int main(int argc, char *argv[]) {
	//init test
	g_assert_cmpint(test_init(), ==, 1);
	g_test_init(&argc, &argv, NULL);

	//download files
	g_assert_cmpint(test_download_file(TEST_XMSI_URL), ==, 1);
	g_assert_cmpint(test_download_file(TEST_XMSA_URL_1_OLD), ==, 1);

	g_test_add("/gobject/composition-static",
			SetupDataComposition,
			NULL,
			setup_data_composition,
			test_composition_static,
			teardown_data_composition
			);
	g_test_add("/gobject/composition-take",
			SetupDataComposition,
			NULL,
			setup_data_composition,
			test_composition_take,
			NULL
			);
	g_test_add("/gobject/composition-null",
			SetupDataComposition,
			NULL,
			NULL,
			test_composition_null,
			teardown_data_composition
			);
	g_test_add("/gobject/composition",
			SetupDataComposition,
			NULL,
			setup_data_composition,
			test_composition,
			teardown_data_composition
			);
	g_test_add("/gobject/excitation-static",
			SetupDataExcitation,
			NULL,
			setup_data_excitation,
			test_excitation_static,
			teardown_data_excitation
			);
	g_test_add("/gobject/excitation-take",
			SetupDataExcitation,
			NULL,
			setup_data_excitation,
			test_excitation_take,
			NULL
			);
	g_test_add("/gobject/excitation-null",
			SetupDataExcitation,
			NULL,
			NULL,
			test_excitation_null,
			teardown_data_excitation
			);
	g_test_add("/gobject/excitation",
			SetupDataExcitation,
			NULL,
			setup_data_excitation,
			test_excitation,
			teardown_data_excitation
			);
	g_test_add("/gobject/input-static",
			SetupDataInput,
			NULL,
			setup_data_input,
			test_input_static,
			teardown_data_input
			);
	g_test_add("/gobject/input-take",
			SetupDataInput,
			NULL,
			setup_data_input,
			test_input_take,
			NULL
			);
	g_test_add("/gobject/input-null",
			SetupDataInput,
			NULL,
			NULL,
			test_input_null,
			teardown_data_input
			);
	g_test_add("/gobject/input",
			SetupDataInput,
			NULL,
			setup_data_input,
			test_input,
			teardown_data_input
			);
	g_test_add("/gobject/archive-static",
			SetupDataArchive,
			NULL,
			setup_data_archive,
			test_archive_static,
			teardown_data_archive
			);
	g_test_add("/gobject/archive-take",
			SetupDataArchive,
			NULL,
			setup_data_archive,
			test_archive_take,
			NULL
			);
	g_test_add("/gobject/archive-null",
			SetupDataArchive,
			NULL,
			NULL,
			test_archive_null,
			teardown_data_archive
			);
	g_test_add("/gobject/archive",
			SetupDataArchive,
			NULL,
			setup_data_archive,
			test_archive,
			teardown_data_archive
			);
	g_test_add("/gobject/main_options-static",
			SetupDataMainOptions,
			NULL,
			setup_data_main_options,
			test_main_options_static,
			teardown_data_main_options
			);
	g_test_add("/gobject/main_options-take",
			SetupDataMainOptions,
			NULL,
			setup_data_main_options,
			test_main_options_take,
			NULL
			);
	g_test_add("/gobject/main_options-null",
			SetupDataMainOptions,
			NULL,
			NULL,
			test_main_options_null,
			teardown_data_main_options
			);
	g_test_add("/gobject/main_options",
			SetupDataMainOptions,
			NULL,
			setup_data_main_options,
			test_main_options,
			teardown_data_main_options
			);

	int rv = g_test_run();

	unlink(TEST_XMSI);

	return rv;
}
