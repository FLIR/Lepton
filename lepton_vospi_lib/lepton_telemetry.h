typedef struct __attribute__((packed)) _telemetry_data_row1 {
	unsigned short telemetry_revision[1];     // w  0
	unsigned short time_ms[2];                // w  1  2
	unsigned short status_bits[2];            // w  3  4
	unsigned short serial_number[8];          // w  5  6  7  8  9 10 11 12
	unsigned short software_rev[4];           // w 13 14 15 16
	unsigned short reserved_1[3];             // w 17 18 19
	unsigned short frame_counter[2];          // w 20 21
	unsigned short frame_mean[1];             // w 22
	unsigned short fpa_temp_counts[1];        // w 23
	unsigned short fpa_temp_100k[1];          // w 24
	unsigned short housing_temp_counts[1];    // w 25
	unsigned short housing_temp_100k[1];      // w 26
	unsigned short reserved_2[2];             // w 27 28
	unsigned short fpa_temp_ffc_100k[1];      // w 29
	unsigned short time_ffc_ms[2];            // w 30 31
	unsigned short housing_temp_ffc_100k[1];  // w 32
	unsigned short reserved_3[1];             // w 33
	unsigned short agc_roi[4];                // w 34 35 36 37
	unsigned short agc_clip_limit_h[1];       // w 38
	unsigned short agc_clip_limit_l[1];       // w 39
	unsigned short reserved_4[34];            // w 40-73
	unsigned short log2_ffc_frames[1];        // w 74
	unsigned short reserved_5[5];             // w 75 76 77 78 79
} telemetry_data_row1;

typedef struct __attribute__((packed)) _telemetry_data_row2 {
	unsigned short telemetry_revision[1];     // w  0
	unsigned short time_ms[2];                // w  1  2
	unsigned short status_bits[2];            // w  3  4
	unsigned short reserved_1[8];             // w  5  6  7  8  9 10 11 12
	unsigned short software_rev[4];           // w 13 14 15 16
	unsigned short reserved_2[3];             // w 17 18 19
	unsigned short frame_counter[2];          // w 20 21
	unsigned short frame_mean[1];             // w 22
	unsigned short fpa_temp_counts[1];        // w 23
	unsigned short fpa_temp_100k[1];          // w 24
	unsigned short reserved_3[4];             // w 25 26 27 28
	unsigned short fpa_temp_ffc_100k[1];      // w 29
	unsigned short time_ffc_ms[2];            // w 30 31
	unsigned short reserved_4[2];             // w 32 33
	unsigned short agc_roi[4];                // w 34 35 36 37
	unsigned short agc_clip_limit_h[1];       // w 38
	unsigned short agc_clip_limit_l[1];       // w 39
	unsigned short reserved_5[32];            // w 40-71
	unsigned short video_output_format[2];    // w 72 73
	unsigned short reserved_6[7];             // w 74 75 76 77 78 79
} telemetry_data_row2;

