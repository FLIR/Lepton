#ifndef __BBG_I2C_H__
#define __BBG_I2C_H__
extern int bbg_init_i2c(void);
extern void bbg_close_i2c(void);
extern int bbg_read_byte_data_i2c(LEP_UINT8 *rx_adr, LEP_UINT8 *rx_data, LEP_UINT32 rx_size);
int bbg_write_byte_data_i2c(LEP_UINT8 *tx_data, LEP_UINT32 tx_size);
#endif // __BBG_I2C_H__
