#ifndef __LEP_I2C_H__
#define __LEP_I2C_H__
extern int i2cdev_init(void);
extern void i2cdev_close(void);
extern int i2cdev_read_byte_data(LEP_UINT8 *rx_adr, LEP_UINT8 *rx_data, LEP_UINT32 rx_size);
int i2cdev_write_byte_data(LEP_UINT8 *tx_data, LEP_UINT32 tx_size);
#endif // __LEP_I2C_H__
