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
#define LEPTON3_TELEMETRY_SUBFRAME_SIZE (LEPTON3_TELEMETRY_SUBFRAME_LINE_HEIGHT * LEPTON_SUBFRAME_LINE_BYTE_WIDTH)

// constants for collecting the subframe index from Lepton 3.x subframes
#define LEPTON3_SUBFRAME_INDEX_LINE1 20
#define LEPTON3_SUBFRAME_INDEX_BYTE 0
#define LEPTON3_SUBFRAME_INDEX_LINE1_BYTE1_MASK 0x70
#define LEPTON3_SUBFRAME_INDEX_LINE2 21
#define LEPTON3_SUBFRAME_INDEX_LINE2_BYTE1_MASK 0x10

typedef enum {LEPTON_VERSION_2X=2, LEPTON_VERSION_3X, LEPTON_VERSION_INVALID} lepton_version;

typedef enum {TELEMETRY_OFF, TELEMETRY_AT_START, TELEMETRY_AT_END, TELEMETRY_INVALID_LOCATION} telemetry_location;

/* Parameters to be calculated for driver code */
struct subframe_parameters {
	int line_count;		// number of lines (including telemetry) in a subframe
	unsigned int subframe_data_byte_size;	// init_lepton_info() fills this in;
						//  use for SPI DMA transfer length
};

/* Parameters to be calculated for user-space code */
struct image_parameters {
	int pixel_width;			// frame width in pixels
	int pixel_height;			// frame height in pixels
};

typedef struct _lepton_vospi_info_t {
	lepton_version lep_version;		// 2 or 3, passed into
						//  init_lepton_info()
	telemetry_location telemetry_loc;		// off, start, or end; passed into
						//  init_lepton_info()
	unsigned int next_subframe_index;	// filled in by
						//  init_lepton_info(), used
						//  internally.
	struct subframe_parameters subframe_params;
	struct image_parameters image_params;
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
 * Given a lepton_vospi_info struct pointer, and a pointer to 16-bit sub-frame
 * data, check whether the subframe index (1-based) is valid.
 *
 * The lep_info struct's next_subframe_index member may be changed by
 * this function.
 */
int is_subframe_index_valid(lepton_vospi_info *lep_info, unsigned short *subframe_data);

/*
 * Extract pixel data from one subframe collected over SPI.
 *
 * For Lepton 3.x, the same pixel_data struct should be used for 4 subframes.
 * Marks the frame as 'done' when all subframes have been received.
 * lep_info: The struct holding lepton VOSPI parameters.
 * received_frame: The raw unprocessed data received from the kernel driver.
 * pixel_data: A pre-allocated buffer (sized for pixel width * pixel height)
 * done: Set to 1 if the pixel_data buffer is ready.
 * Returns 0 normally, >0 for the number of line counter errors encountered.
 */
int extract_pixel_data(lepton_vospi_info *lep_info, unsigned short *received_frame, unsigned short *pixel_data,
					  int *done);


