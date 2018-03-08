// unit test for lepton_vospi_funcs.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lepton_vospi_funcs.h"

/*
 * Write subframe data as ASCII to an open file handle.
 * Separates packet data with newlines.
 */
void dump_subframe(FILE *dump_file, lepton_vospi_info *lep_info, unsigned short *subframe_data) {
	int line_height = LEPTON_SUBFRAME_DATA_LINE_HEIGHT;
	int i, j;

	fprintf(dump_file, "Generated subframe data:\n");
	if (lep_info->telemetry_enabled) {
		if (lep_info->lep_version == LEPTON_VERSION_2X) {
			line_height += LEPTON2_TELEMETRY_LINE_HEIGHT;
		}
		else {
			line_height += LEPTON3_TELEMETRY_SUBFRAME_LINE_HEIGHT;
		}
	}
	for (i=0; i<line_height; i++) {
		for (j=0; j<LEPTON_SUBFRAME_LINE_WORD_COUNT; j++) {
			fprintf(dump_file, "%04x", subframe_data[i*LEPTON_SUBFRAME_LINE_WORD_COUNT+j]);
			if (j<(LEPTON_SUBFRAME_LINE_WORD_COUNT-1)) {
				fprintf(dump_file, " ");
			}
			else {
				fprintf(dump_file, "\n");
			}
		}
	}
}

/*
 * Write lepton info struct information to an open file handle.
 */
void dump_lepton_info_struct(FILE *dump_file, lepton_vospi_info *lep_info) {
	fprintf(dump_file, "lepton_vospi_info fields:\n");
	fprintf(dump_file, "\tWidth in pixels: %d\n", lep_info->pixel_width);
	fprintf(dump_file, "\tHeight in pixels: %d\n", lep_info->pixel_height);
	fprintf(dump_file, "\tOverall frame pixel data in bytes: %d\n", lep_info->frame_size);
	fprintf(dump_file, "\tSPI transfer size: %d\n", lep_info->subframe_data_byte_size);
	fprintf(dump_file, "\tFrame buffer size: %d\n", lep_info->total_data_byte_size);
	fprintf(dump_file, "\tTelemetry enabled: %d\n", lep_info->telemetry_enabled);
	fprintf(dump_file, "\tNext subframe index: %d\n", lep_info->next_subframe_index);
}

/*
 * Fill an existing subframe with data (can generate bad line numbers or subframe index, or both).
 * Will not generate CRCs.
 */
void gen_subframe_data(lepton_vospi_info *lep_info, unsigned short *subframe_data, int invalid_line_counter,
			    int invalid_subframe_idx) {
	unsigned short *line_data = subframe_data;
	int i;

	// fill in line counters for pixel data (and subframe indices for Lepton 3)
	for (i=0; i<LEPTON_SUBFRAME_DATA_LINE_HEIGHT; i++) {
		// clear one line
		memset(line_data, 0, LEPTON_SUBFRAME_LINE_BYTE_WIDTH);
		// set line counter
		if (!invalid_line_counter) {
			line_data[0] = i;
		}
		// set subframe index fields
		if ((lep_info->lep_version == LEPTON_VERSION_3X) && (!invalid_subframe_idx)) {
			if (i == LEPTON3_SUBFRAME_INDEX_LINE1) {
				int bits = (lep_info->next_subframe_index & 0x7) << 12;

				// fprintf(stderr, "Write index # %d, masked as 0x%x\n", lep_info->next_subframe_index, bits);
				line_data[0] |= ((lep_info->next_subframe_index & 0x7) << 12);
			}
			else if (i == LEPTON3_SUBFRAME_INDEX_LINE2) {
				int bits = (lep_info->next_subframe_index & 0x8) << 9;

				// fprintf(stderr, "Write index # %d, masked as 0x%x\n", lep_info->next_subframe_index, bits);
				line_data[0] |= ((lep_info->next_subframe_index & 0x8) << 9);
			}
		}
		// @@@ pattern generator TBD
		// point to the start of the next line
		if (i < (LEPTON_SUBFRAME_DATA_LINE_HEIGHT-1)) {
			line_data = get_line_from_subframe(subframe_data, i+1);
		}
	}
	// fill in line counters for telemetry, if any
	if (!invalid_line_counter && lep_info->telemetry_enabled) {
		int telem_line_count = LEPTON2_TELEMETRY_LINE_HEIGHT;
		if (lep_info->lep_version == LEPTON_VERSION_3X) {
			telem_line_count = LEPTON3_TELEMETRY_SUBFRAME_LINE_HEIGHT;
		}
		for (i=0; i<telem_line_count; i++) {
			line_data = get_line_from_subframe(subframe_data, LEPTON_SUBFRAME_DATA_LINE_HEIGHT+i);
			line_data[0] = LEPTON_SUBFRAME_DATA_LINE_HEIGHT+i;
		}
	}
}

int check_lepton_info(lepton_vospi_info *lep_info, lepton_version lep_version, int telemetry_enabled) {
	int expected_pixel_width = LEPTON_SUBFRAME_LINE_PIXEL_WIDTH;
	int expected_pixel_height = LEPTON_SUBFRAME_DATA_LINE_HEIGHT;
	int expected_frame_size = LEPTON2_FRAME_SIZE;
	int expected_next_subframe_index = 0;
	int expected_subframe_data_byte_size = LEPTON_SUBFRAME_SIZE;
	int expected_total_data_byte_size = expected_frame_size;
	int info_ok = 1;

	if ((lep_version == LEPTON_VERSION_2X) && (telemetry_enabled)) {
		expected_subframe_data_byte_size += LEPTON2_TELEMETRY_SUBFRAME_SIZE;
		expected_total_data_byte_size += LEPTON2_TELEMETRY_SUBFRAME_SIZE;
	}
	if (lep_version == LEPTON_VERSION_3X) {
		expected_pixel_width *= 2;
		expected_pixel_height *= 2;
		expected_frame_size = LEPTON3_FRAME_SIZE;
		expected_next_subframe_index = 1;
		expected_total_data_byte_size = expected_frame_size;
		if (telemetry_enabled) {
			expected_subframe_data_byte_size += LEPTON3_TELEMETRY_SUBFRAME_SIZE;
		}
	}
	if (lep_info->pixel_width != expected_pixel_width) {
		fprintf(stderr, "Pixel width mismatch: Expected %d, found %d\n", expected_pixel_width,
			lep_info->pixel_width);
		info_ok = 0;
	}
	if (lep_info->pixel_height != expected_pixel_height) {
		fprintf(stderr, "Pixel height mismatch: Expected %d, found %d\n", expected_pixel_height,
			lep_info->pixel_height);
		info_ok = 0;
	}
	if ((lep_info->telemetry_enabled && !telemetry_enabled) ||
		       (!lep_info->telemetry_enabled && telemetry_enabled)) {
		fprintf(stderr, "Telemetry field error, expected %d, got %d\n", telemetry_enabled,
			lep_info->telemetry_enabled);
		info_ok = 0;
	}
	if (lep_info->frame_size != expected_frame_size) {
		fprintf(stderr, "Frame size mismatch: Expected %d, found %d\n", expected_frame_size,
			lep_info->frame_size);
		info_ok = 0;
	}
	if (lep_info->next_subframe_index != expected_next_subframe_index) {
		fprintf(stderr, "Expected next subframe index error: Expected %d, found %d\n",
			expected_next_subframe_index, lep_info->next_subframe_index);
		info_ok = 0;
	}

	return info_ok;
}

int test_init_lepton_info(lepton_vospi_info *lep_info, int debug) {
	int lep_info_ok = 1;

	// test lepton 2, no telemetry
	init_lepton_info(lep_info, LEPTON_VERSION_2X, 0);
	lep_info_ok = check_lepton_info(lep_info, LEPTON_VERSION_2X, 0);
	if (debug) {
		dump_lepton_info_struct(stderr, lep_info);
	}
	if (!lep_info_ok) {
		fprintf(stderr, "%s: Failed lepton2 w/o telemetry validation\n", __func__);
	}
	// test lepton 2 with telemetry
	init_lepton_info(lep_info, LEPTON_VERSION_2X, 1);
	lep_info_ok = check_lepton_info(lep_info, LEPTON_VERSION_2X, 1);
	if (debug) {
		dump_lepton_info_struct(stderr, lep_info);
	}
	if (!lep_info_ok) {
		fprintf(stderr, "%s: Failed lepton2 with telemetry validation\n", __func__);
	}
	// test lepton 3, no telemetry
	init_lepton_info(lep_info, LEPTON_VERSION_3X, 0);
	lep_info_ok = check_lepton_info(lep_info, LEPTON_VERSION_3X, 0);
	if (debug) {
		dump_lepton_info_struct(stderr, lep_info);
	}
	if (!lep_info_ok) {
		fprintf(stderr, "%s: Failed lepton3 w/o telemetry validation\n", __func__);
	}
	// test lepton 3 with telemetry
	init_lepton_info(lep_info, LEPTON_VERSION_3X, 1);
	lep_info_ok = check_lepton_info(lep_info, LEPTON_VERSION_3X, 1);
	if (debug) {
		dump_lepton_info_struct(stderr, lep_info);
	}
	if (!lep_info_ok) {
		fprintf(stderr, "%s: Failed lepton3 with telemetry validation\n", __func__);
	}
test_init_lepton_info_done:
	return lep_info_ok;
}

int test_is_subframe_index_valid(lepton_vospi_info *lep_info, unsigned short *subframe_buf) {
	int index_valid_ok = 1;
	int i;

	// Lepton 2 generator should always set index to 0
	// @@ test lepton 2, no telemetry
	init_lepton_info(lep_info, LEPTON_VERSION_2X, 0);
	gen_subframe_data(lep_info, subframe_buf, 0, 0);
	if (!is_subframe_index_valid(lep_info, subframe_buf, 0)) {
		fprintf(stderr, "%s: is_subframe_index_valid() failed for correct subframe data\n", __func__);
		index_valid_ok = 0;
	}
	// dump_subframe(stderr, &lep_info, subframe_buf);
	// @@ test lepton 2 with telemetry
	init_lepton_info(lep_info, LEPTON_VERSION_2X, 1);
	gen_subframe_data(lep_info, subframe_buf, 0, 0);
	if (!is_subframe_index_valid(lep_info, subframe_buf, 0)) {
		fprintf(stderr, "%s: is_subframe_index_valid() failed for correct subframe data\n", __func__);
		index_valid_ok = 0;
	}
	// dump_subframe(stderr, &lep_info, subframe_buf);
	// @@ test lepton 3, no telemetry
	init_lepton_info(lep_info, LEPTON_VERSION_3X, 0);
	for (i=1; i<=4; i++) {
		gen_subframe_data(lep_info, subframe_buf, 0, 0);
		if (!is_subframe_index_valid(lep_info, subframe_buf, i)) {
			fprintf(stderr, "%s: is_subframe_index_valid() failed for correct subframe data\n", __func__);
			index_valid_ok = 0;
		}
	}
	// dump_subframe(stderr, &lep_info, subframe_buf);
	// @@ test lepton 3 with telemetry
	init_lepton_info(lep_info, LEPTON_VERSION_3X, 1);
	for (i=1; i<=4; i++) {
		gen_subframe_data(lep_info, subframe_buf, 0, 0);
		if (!is_subframe_index_valid(lep_info, subframe_buf, i)) {
			fprintf(stderr, "%s: is_subframe_index_valid() failed for correct subframe data\n", __func__);
			index_valid_ok = 0;
		}
	}
	// dump_subframe(stderr, &lep_info, subframe_buf);

test_is_subframe_index_valid_done:
	return index_valid_ok;
}

// @@@ debug arg
// @@@ debug output location
int main(int argc, char **argv) {
	unsigned short *subframe_buf = NULL;
	int largest_subframe_byte_size = ((LEPTON_SUBFRAME_LINE_BYTE_WIDTH*LEPTON_SUBFRAME_DATA_LINE_HEIGHT) +
					(LEPTON_SUBFRAME_LINE_BYTE_WIDTH*LEPTON2_TELEMETRY_LINE_HEIGHT));
	lepton_vospi_info lep_info;
	int debug = 0;

	fprintf(stderr, "test_init_lepton_info:\t\t\t");
	if (!test_init_lepton_info(&lep_info, debug)) {
		fprintf(stderr, "F\n");
	}
	else {
		fprintf(stderr, "P\n");
	}
	// allocate for the largest possible subframe transfer size (subframe data + Lepton 2 telemetry)
	subframe_buf = malloc(largest_subframe_byte_size);
	if (!subframe_buf) {
		fprintf(stderr, "Unable to allocate %d subframe bytes!\n", largest_subframe_byte_size);
		exit(1);
	}
	fprintf(stderr, "test_is_subframe_index_valid:\t\t");
	if (!test_is_subframe_index_valid(&lep_info, subframe_buf)) {
		fprintf(stderr, "F\n");
	}
	else {
		fprintf(stderr, "P\n");
	}
main_done:
	free(subframe_buf);
	return 0;
}

