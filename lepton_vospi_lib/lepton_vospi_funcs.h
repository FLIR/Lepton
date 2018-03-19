/*
 * Constants for telemetry, sub-frame and frame sizes, for calculating buffer
 * sizes.
 */
#define LEPTON_SUBFRAME_LINE_WORD_COUNT 82
#define LEPTON_SUBFRAME_LINE_BYTE_WIDTH (LEPTON_SUBFRAME_LINE_WORD_COUNT*2)
#define LEPTON_SUBFRAME_LINE_PIXEL_WIDTH 80
#define LEPTON_SUBFRAME_DATA_LINE_HEIGHT 60
#define LEPTON_SUBFRAME_SIZE (LEPTON_SUBFRAME_LINE_BYTE_WIDTH * LEPTON_SUBFRAME_DATA_LINE_HEIGHT)
#define LEPTON2_SUBFRAME_COUNT 1
#define LEPTON2_FRAME_SIZE (LEPTON_SUBFRAME_SIZE * LEPTON2_SUBFRAME_COUNT)
#define LEPTON3_SUBFRAME_COUNT 4
#define LEPTON3_FRAME_SIZE (LEPTON_SUBFRAME_SIZE * LEPTON3_SUBFRAME_COUNT)
#define LEPTON2_TELEMETRY_LINE_HEIGHT 3
#define LEPTON2_TELEMETRY_SUBFRAME_LINE_HEIGHT LEPTON2_TELEMETRY_LINE_HEIGHT // each (full) frame has 3 extra lines
#define LEPTON2_TELEMETRY_FRAME_LINE_HEIGHT LEPTON2_TELEMETRY_SUBFRAME_LINE_HEIGHT
#define LEPTON2_TELEMETRY_SUBFRAME_SIZE (LEPTON2_TELEMETRY_SUBFRAME_LINE_HEIGHT * LEPTON_SUBFRAME_LINE_BYTE_WIDTH)
#define LEPTON3_TELEMETRY_SUBFRAME_LINE_HEIGHT 1  // each subframe has 1 extra line
#define LEPTON3_TELEMETRY_FRAME_LINE_HEIGHT 4  // each full frame has 4 extra lines
#define LEPTON3_TELEMETRY_SUBFRAME_SIZE (LEPTON3_TELEMETRY_SUBFRAME_LINE_HEIGHT * LEPTON_SUBFRAME_LINE_BYTE_WIDTH)

// constants for collecting the subframe index from Lepton 3.x subframes
#define LEPTON3_SUBFRAME_INDEX_LINE1 20
#define LEPTON3_SUBFRAME_INDEX_BYTE 1
#define LEPTON3_SUBFRAME_INDEX_LINE1_BYTE1_MASK 0x70
#define LEPTON3_SUBFRAME_INDEX_LINE2 21
#define LEPTON3_SUBFRAME_INDEX_LINE2_BYTE1_MASK 0x10

typedef enum {LEPTON_VERSION_2X=2, LEPTON_VERSION_3X, LEPTON_VERSION_INVALID} lepton_version;

typedef enum {TELEMETRY_OFF, TELEMETRY_AT_START, TELEMETRY_AT_END, TELEMETRY_INVALID_LOCATION} telemetry_location;

typedef struct _lepton_vospi_info_t {
	lepton_version lep_version;		// 2 or 3, passed into
						//  init_lepton_info()
	int pixel_width;			// frame width in pixels
	int pixel_height;			// frame height in pixels
	int line_count;		// number of lines (including telemetry) in an entire frame
	telemetry_location telemetry_loc;		// off, start, or end; passed into
						//  init_lepton_info()
	unsigned int next_subframe_index;	// filled in by
						//  init_lepton_info(), used
						//  internally.
	unsigned int subframe_data_byte_size;	// init_lepton_info() fills this in; use
						//  for SPI DMA transfer length
	unsigned int frame_size;		// init_lepton_info() fills this in; use
						// for allocating actual image data
	unsigned int total_data_byte_size;	// init_lepton_info() fills this in; use
						// this size for allocating buffers
} lepton_vospi_info;

/*
 * Given a pointer to a pre-allocated lepton_vospi_info struct, the lepton
 * version number, and a telemetry enabled flag, fill in the struct members.
 */
int init_lepton_info(lepton_vospi_info *lep_info, lepton_version lep_version, telemetry_location telemetry_loc);

/*
 * Given a pointer to 16-bit sub-frame data, return a pointer to the first 16-
 * bit value on the given line.
 */
unsigned char *get_line_from_subframe(unsigned short *subframe_data, int line_no);

/*
 * Given a pointer to 16-bit sub-frame data, determine whether the last line's
 * line number is correct.
 */
int is_subframe_line_counter_valid(lepton_vospi_info *lep_info, unsigned short *subframe_data);

/*
 * Given a lepton_vospi_info struct pointer, a pointer to 16-bit sub-frame
 * data, and an expected next index, check whether the subframe index (1-based)
 * is valid.
 *
 * The lep_info struct's next_subframe_index member may be changed by
 * this function.
 */
int is_subframe_index_valid(lepton_vospi_info *lep_info, unsigned short *subframe_data, int expected_index);

/*
 * Extract pixel data from one entire frame collected over SPI.
 */
// int extract_pixel_data(lepton_vospi_info *lep_info, unsigned short *received frame, unsigned short *pixel_data);

