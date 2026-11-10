#include <stdint.h>

#include "sam.h"

#include "drivers/adc_driver.h"


void adc_init() {
    // Enable the peripheral clock for the ADC
    //Since the ADC has a ID of 37, we subtract 32 to get the excact bit position in the PMC_PCER1 register
    PMC->PMC_PCER1 |= (1 << (ID_ADC - 32)); 

    // Reset the ADC
    ADC->ADC_CR = ADC_CR_SWRST;

    // Set the ADC mode register
    ADC->ADC_MR = ADC_MR_TRGEN_DIS | // Disable hardware trigger
                  ADC_MR_LOWRES_BITS_12 | // 12-bit resolution
                  ADC_MR_SLEEP_NORMAL | // Normal mode
                  ADC_MR_FWUP_OFF | // Fast wake-up disabled
                  ADC_MR_FREERUN_OFF | // Free run mode disabled
                  ADC_MR_PRESCAL(5) | //This results in a ADC frequency of 7 MHz
                  ADC_MR_STARTUP_SUT64 ; //wait 64 periods of ADC clock before starting the first conversion
                  

    // Enable the desired channels (e.g., channel 7 for A0)
    ADC->ADC_CHER = (1 << 0); // Enable channel 7

    // Start the first conversion
    ADC->ADC_CR = ADC_CR_START;
}

uint16_t adc_read() {
    // Wait for the conversion to complete
    while (!(ADC->ADC_ISR & ADC_ISR_EOC0)); // Wait for end of conversion on channel 7

    // Read the converted value
    uint16_t value = ADC->ADC_CDR[0]; // Read the data register for channel 7

    // Start the next conversion
    ADC->ADC_CR = ADC_CR_START;

    return value;
}

uint16_t adc_read_average(uint8_t samples) {
    uint32_t sum = 0;
    for (uint8_t i = 0; i < samples; i++) {
        sum += adc_read();
    }
    return (uint16_t)(sum / samples);
}

uint16_t adc_state() {
    // Read the converted average value
  uint16_t value = adc_read_average(10);
  if (value < 1900) {
    return 0; // Low state}
  } else { 
    return 1; // High state}
   
    }
    
}
