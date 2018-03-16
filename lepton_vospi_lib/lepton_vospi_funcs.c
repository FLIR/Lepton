#ifdef __KERNEL__

#include <linux/kernel.h>
#include <linux/module.h>

#else

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#endif

#include "lepton_vospi_funcs.h"

/*
 * Given a pointer to a pre-allocated lepton_vospi_info struct, the lepton
 * version number, and a telemetry enabled flag, fill in the struct members.
 */
int init_lepton_info(lepton_vospi_info *lep_info, lepton_version lep_version, int telemetry_enabled) {
	int ok = 0;  // 0 means no errors

    // check for typecasting from bad version values
	if ((lep_version < LEPTON_VERSION_2X) || (lep_version >= LEPTON_VERSION_INVALID)) {
		return 1;
	}
	memset(lep_info, 0, sizeof(*lep_info));
	lep_info->lep_version = lep_version;
	/*
	 * The frame_size and expected_next_subframe_index members depend on the
	 * lepton version.
	 */
	if (lep_version == LEPTON_VERSION_2X) {
		lep_info->pixel_width = LEPTON_SUBFRAME_LINE_PIXEL_WIDTH;
		lep_info->pixel_height = LEPTON_SUBFRAME_DATA_LINE_HEIGHT;
		lep_info->frame_size = LEPTON2_FRAME_SIZE; // size of pixel data (including line #, CRC words) in bytes
		lep_info->next_subframe_index = 0; // only 1 subframe per frame, no need for counter
	}
	else {
		lep_info->pixel_width = LEPTON_SUBFRAME_LINE_PIXEL_WIDTH * 2;
		lep_info->pixel_height = LEPTON_SUBFRAME_DATA_LINE_HEIGHT * 2;
		lep_info->frame_size = LEPTON3_FRAME_SIZE; // size of pixel data (including line #, CRC words) in bytes
		lep_info->next_subframe_index = 1; // 4 subframes per frame, start counter at 1
	}
	lep_info->line_count = lep_info->pixel_height;
	lep_info->subframe_data_byte_size = LEPTON_SUBFRAME_SIZE; // SPI xfer bytes per subframe, without telemetry
	lep_info->total_data_byte_size = lep_info->frame_size; // size of 1 frame's worth of data in bytes
	lep_info->telemetry_enabled = 0;
	/*
	 * The total_data_byte_size and subframe_data_byte_size members depend on
	 * the lepton version and whether telemetry is enabled.
	 */
	if (telemetry_enabled) {
		lep_info->telemetry_enabled = 1;
		if (lep_version == LEPTON_VERSION_2X) {
			// each subframe contains 3 additional lines of telemetry (SPI xfer size)
			lep_info->subframe_data_byte_size += LEPTON2_TELEMETRY_SUBFRAME_SIZE;
			lep_info->line_count += LEPTON2_TELEMETRY_LINE_HEIGHT;
			// each frame consists of 1 subframe + 3 lines of telemetry (overall buffer size)
			lep_info->total_data_byte_size += LEPTON2_TELEMETRY_SUBFRAME_SIZE;
		}
		else {
			// each subframe contains 1 additional line of telemetry (SPI xfer size)
			lep_info->subframe_data_byte_size += LEPTON3_TELEMETRY_SUBFRAME_SIZE;
			// each frame contains 4 additional lines (only 3 are telemetry)
			lep_info->line_count += LEPTON3_TELEMETRY_FRAME_LINE_HEIGHT;
			// each frame consists of 4 subframes + 4 lines of telemetry (overall buffer size)
			lep_info->total_data_byte_size += (LEPTON3_TELEMETRY_FRAME_LINE_HEIGHT * LEPTON_SUBFRAME_LINE_BYTE_WIDTH);
		}
	}
	return ok;
}

unsigned char *get_line_from_subframe(unsigned short *subframe_data, int line_no) {
	return (unsigned char *)&(subframe_data[LEPTON_SUBFRAME_LINE_WORD_COUNT*line_no]);
}

int is_subframe_line_counter_valid(lepton_vospi_info *lep_info, unsigned short *subframe_data) {
	int valid = 0;
	int last_line_no = LEPTON_SUBFRAME_DATA_LINE_HEIGHT-1;
	unsigned char *last_line = NULL;

	if (lep_info->telemetry_enabled) {
		if (lep_info->lep_version == LEPTON_VERSION_2X) {
			last_line_no += LEPTON2_TELEMETRY_SUBFRAME_LINE_HEIGHT;
		}
		else {
			last_line_no += LEPTON3_TELEMETRY_SUBFRAME_LINE_HEIGHT;
		}
	}
	last_line = get_line_from_subframe(subframe_data, last_line_no);
	if (last_line[1] == (LEPTON_SUBFRAME_DATA_LINE_HEIGHT-1)) {
		valid = 1;
	}
	return valid;
}

/*
 * Given a pointer to 16-bit sub-frame data from a Lepton 3.x, collect the
 * embedded sub-frame index.
 */
int get_subframe_index_from_subframe(unsigned short *subframe_data) {
	unsigned char *subframe_byte_base = NULL;
	int subframe_index = 0;

	subframe_byte_base = get_line_from_subframe(subframe_data, LEPTON3_SUBFRAME_INDEX_LINE1);
	subframe_index = (subframe_byte_base[LEPTON3_SUBFRAME_INDEX_BYTE] & LEPTON3_SUBFRAME_INDEX_LINE1_BYTE1_MASK) >> 4;
	subframe_byte_base = get_line_from_subframe(subframe_data, LEPTON3_SUBFRAME_INDEX_LINE2);
	subframe_index |= (subframe_byte_base[LEPTON3_SUBFRAME_INDEX_BYTE] & LEPTON3_SUBFRAME_INDEX_LINE2_BYTE1_MASK) >> 1;

	return subframe_index;
}

/*
 * Given a lepton_vospi_info struct pointer and a pointer to 16-bit sub-frame
 * data, find the subframe index (1-based).
 *
 * Always return 0 for Lepton 2.X. Return 0 for Lepton 3.X only for duplicate
 * frames.
 */
int get_subframe_index(lepton_vospi_info *lep_info, unsigned short *subframe_data) {
	int sidx = 0;

	if (lep_info->lep_version != LEPTON_VERSION_2X) {
		sidx = get_subframe_index_from_subframe(subframe_data);
	}
	return sidx;
}

int is_subframe_index_valid(lepton_vospi_info *lep_info, unsigned short *subframe_data, int expected_index) {
	int valid = 0;
	int sidx = 0;

	sidx = get_subframe_index(lep_info, subframe_data);
	if (sidx == expected_index) {
		valid = 1;
	}
	if (expected_index > 0) {
		// only true for Lepton 3.X
		lep_info->next_subframe_index += 1;
		if (lep_info->next_subframe_index > 4) {
		lep_info->next_subframe_index = 1;
		}
	}
	return valid;
}


#ifdef __KERNEL__
EXPORT_SYMBOL(init_lepton_info);
EXPORT_SYMBOL(get_line_from_subframe);
EXPORT_SYMBOL(is_subframe_line_counter_valid);
EXPORT_SYMBOL(is_subframe_index_valid);
#endif
