#include "drivers/spi_driver.h"

/*##################### SS PINS #####################*/
const spi_ss_pin_t oled_ss = OLED_SS;
const spi_ss_pin_t io_ss = IO_SS;
const spi_ss_pin_t slaves[2] = {OLED_SS, IO_SS};
/*###################################################*/

static void SPI_MasterInit(void) {
    /* Set MOSI and SCK output, all others input */
    DDRB |= (1<<DD_MOSI)|(1<<DD_SCK)|(1 << PB4);
    PORTB |= (1 << PB4);
    clear_bit(DDRB, DD_MISO);
    /* Enable SPI, Master, set clock rate fck/2 */
    SPCR = (1<<SPE)|(1<<MSTR);
    SPSR = (1<<SPI2X);
}

void spi_init() {
    for(uint8_t i = 0; i < sizeof(slaves)/sizeof(slaves[0]); ++i) {
        set_bit(*slaves[i].port, slaves[i].bit);
        set_bit(*slaves[i].ddr, slaves[i].bit);
    }
    SPI_MasterInit();
}

static uint8_t SPI_MasterTransmit(uint8_t cData) {
    /* Start transmission */
    SPDR = cData;
    /* Wait for transmission complete */
    while(!(SPSR & (1<<SPIF))) {/* Wait */};

    return SPDR;
}

uint8_t * spi_wr(uint8_t data[], int data_len, spi_ss_pin_t slave, uint8_t * return_buff) {
    clear_bit(*slave.port, slave.bit);

    for(int i = 0; i < data_len; ++i) {
        return_buff[i] = SPI_MasterTransmit(data[i]);
    }

    set_bit(*slave.port, slave.bit);

    return return_buff;
}

uint8_t * spi_transmit(uint8_t data[], int data_len, uint8_t * return_buff) {
    for(int i = 0; i < data_len; ++i) {
        return_buff[i] = SPI_MasterTransmit(data[i]);
    }
    return return_buff;
}

void spi_w(uint8_t data[], int data_len, spi_ss_pin_t slave) {
    clear_bit(*slave.port, slave.bit);

    for(int i = 0; i < data_len; ++i) {
        SPI_MasterTransmit(data[i]);
    }

    set_bit(*slave.port, slave.bit);
}

void select_slave(spi_ss_pin_t slave) {
    clear_bit(*slave.port, slave.bit);
}
void deselect_slave(spi_ss_pin_t slave) {
    set_bit(*slave.port, slave.bit);
}