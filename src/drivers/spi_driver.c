#include "drivers/spi_driver.h"

static void SPI_MasterInit(void) {
    /* Set MOSI and SCK output, all others input */
    DDRB |= (1<<DD_MOSI)|(1<<DD_SCK)|(1 << PB4);
    PORTB |= (1 << PB4);
    /* Enable SPI, Master, set clock rate fck/16 */
    SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

void spi_init(const spi_ss_pin_t slaves[], int num_slaves) {
    SPI_MasterInit();

    for(int i = 0; i < num_slaves; ++i) {
        set_bit(*slaves[i].port, slaves[i].bit);
        set_bit(*slaves[i].ddr, slaves[i].bit);
    }
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

void spi_w(uint8_t data[], int data_len, spi_ss_pin_t slave) {
    clear_bit(*slave.port, slave.bit);

    for(int i = 0; i < data_len; ++i) {
        SPI_MasterTransmit(data[i]      );
    }

    set_bit(*slave.port, slave.bit);
}