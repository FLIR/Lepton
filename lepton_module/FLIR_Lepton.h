#define LEPTON_MODULE_NAME "lepton"
#define VERSION "1.0"
#define lepton_major 240
#define lepton_minor 0

#define LEPTON_LINE_WIDTH 62  	// 16-bit values per line
#define LEPTON_SUBFRAME_HEIGHT 80  // lines per subframe
#define FRAME_WIDTH_LEPTON_2 LEPTON_LINE_WIDTH
#define FRAME_WIDTH_LEPTON_3 (LEPTON_LINE_WIDTH * 2)
#define FRAME_HEIGHT_LEPTON_2 LEPTON_SUBFRAME_HEIGHT
#define FRAME_HEIGHT_LEPTON_3 (LEPTON_SUBFRAME_HEIGHT*2)
/*
 * The lepton will choke on capturing the next frame if the SPI transfers
 * from the previous frame were still occurring during a short period
 * before the VSYNC gets sent. Tune this parameter for situations in
 * which SPI transfers complete in time, but are marginal (evidenced
 * by error packets arriving in all subsequent frames). This situation
 * can only be resolved by resetting the lepton.
 */
#define ONE_MS_IN_NS 1000000
// the last SPI transfer needs to complete this many ns before VSYNC
#define MINIMUM_SPI_TRANSFER_QUIET_TIME (2 * ONE_MS_IN_NS)
// how many discards in a row can be received before giving up
#define MAX_CONSEC_DISCARD_COUNT 120

