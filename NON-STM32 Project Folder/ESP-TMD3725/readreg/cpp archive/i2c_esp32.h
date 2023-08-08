#include "Wire.h"

void get_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, size_t len);
void set_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t data);

