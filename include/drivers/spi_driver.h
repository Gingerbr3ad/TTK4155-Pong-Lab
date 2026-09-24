#include "system_definitions.h"

#define DD_MOSI 5
#define DD_MISO 6
#define DD_SCK 7

typedef struct {
    volatile uint8_t *ddr;
    volatile uint8_t *port;
    int bit;
} spi_ss_pin;

void spi_init(spi_ss_pin slaves[], int num_slaves);
char * spi_wr(char data[], int data_len, spi_ss_pin slave, char * return_char_buff);
void spi_w(char data[], int data_len, spi_ss_pin slave);