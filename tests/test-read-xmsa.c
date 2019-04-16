#include <config.h>
#include "libxmimsim-test.h"
#include "xmi_msim.h"
#include "xmi_aux.h"
#include <glib.h>
#include <math.h>
#include <unistd.h>


int main(int argc, char *argv[]) {
	xmi_archive *archive = NULL;
	xmi_archive *archive_copy = NULL;
	int i,j;

	//init test
	g_assert(test_init() == 1);

	//download file
	g_assert(test_download_file(TEST_XMSA_URL_1) == 1);

	//read the file
	g_assert_nonnull(archive = xmi_archive_read_from_xml_file(TEST_XMSA_1, NULL));

	//some testing of the input and output
	for (i = 0 ; i <= archive->nsteps1 ; i++) {
		for (j = 0 ; j <= archive->nsteps2 ; j++) {
			xmi_output *output_copy = NULL;
			xmi_output_copy(archive->output[i][j], &output_copy);
			g_assert(xmi_input_validate(output_copy->input) == 0);
			g_assert(xmi_input_compare(archive->input[i][j], output_copy->input) == 0);
			xmi_output_free(output_copy);
		}
	}

	//make a copy
	archive_copy = xmi_archive_ref(archive);
	g_assert_cmpint(archive->ref_count, ==, 2);
	g_assert_cmpint(archive_copy->ref_count, ==, 2);
	g_assert_true(xmi_archive_equals(archive, archive_copy));
	xmi_archive_unref(archive);
	g_assert_cmpint(archive->ref_count, ==, 1);
	g_assert_cmpint(archive_copy->ref_count, ==, 1);
	xmi_archive_unref(archive_copy);

	//download file
	g_assert(test_download_file(TEST_XMSA_URL_2) == 1);

	//read the file
	g_assert_nonnull(archive = xmi_archive_read_from_xml_file(TEST_XMSA_2, NULL));

	//some testing of the input and output
	for (i = 0 ; i <= archive->nsteps1 ; i++) {
		for (j = 0 ; j <= archive->nsteps2 ; j++) {
			xmi_output *output_copy = NULL;
			xmi_output_copy(archive->output[i][j], &output_copy);
			g_assert(xmi_input_validate(output_copy->input) == 0);
			g_assert(xmi_input_compare(archive->input[i][j], output_copy->input) == 0);
			xmi_output_free(output_copy);
		}
	}

	//make a copy
	archive_copy = xmi_archive_ref(archive);
	g_assert_cmpint(archive->ref_count, ==, 2);
	g_assert_cmpint(archive_copy->ref_count, ==, 2);
	g_assert_true(xmi_archive_equals(archive, archive_copy));
	xmi_archive_unref(archive);
	g_assert_cmpint(archive->ref_count, ==, 1);
	g_assert_cmpint(archive_copy->ref_count, ==, 1);
	xmi_archive_unref(archive_copy);

	// now some tests that are supposed to fail
	GError *error = NULL;
	g_assert_null(archive = xmi_archive_read_from_xml_file("non-existent-file.xmsa", &error));
	g_assert_true(g_error_matches(error, XMI_MSIM_ERROR, XMI_MSIM_ERROR_XML));
	fprintf(stdout, "message: %s\n", error->message);
	g_clear_error(&error);

	// bad xpath1 test: enable when implemented
	/*g_assert(replace_xml_tag(TEST_XMSA_1, TEST_XMSA_COPY_1, "/xmimsim-archive/xpath1", "hsdhodhoosda") == 1);
	g_assert(xmi_read_archive_xml(TEST_XMSA_COPY_1, &archive, &error) == 0);
	g_assert_true(g_error_matches(error, XMI_MSIM_ERROR, XMI_MSIM_ERROR_XML));
	fprintf(stdout, "message: %s\n", error->message);
	g_clear_error(&error);
	unlink(TEST_XMSA_COPY_1);*/

	g_assert(remove_xml_tags(TEST_XMSA_1, TEST_XMSA_COPY_1, "/xmimsim-archive/end_value1") == 1);
	g_assert_null(archive = xmi_archive_read_from_xml_file(TEST_XMSA_COPY_1, &error));
	g_assert_true(g_error_matches(error, XMI_MSIM_ERROR, XMI_MSIM_ERROR_XML));
	fprintf(stdout, "message: %s\n", error->message);
	g_clear_error(&error);
	unlink(TEST_XMSA_COPY_1);
	return 0;
}

