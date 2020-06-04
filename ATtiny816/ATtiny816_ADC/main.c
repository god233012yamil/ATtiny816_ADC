#include <atmel_start.h>

// Defines
#define R1	200000// 200K
#define R2	200000// 200K

// Variables Declaration section.
uint8_t adc_state;
adc_result_t result;
uint16_t voltage;

// Prototype Function Declarations section
void measure_voltage(ADC_MUXPOS_t adc_channel);

// Function Definitions section

/*
* \brief		Function to measure the voltage on the passed ADC input pin.
*				It must run on main loop because it uses the poll method.			
*
* \param[in]	adc_channel: ADC input pin. 
*
* \return		Nothing
*/
void measure_voltage(ADC_MUXPOS_t adc_channel) {
	switch(adc_state) {
		case 0:
			// Select the ADC channel to start a single conversion on,
			ADC_0_start_conversion(adc_channel);
			// Change state.
			adc_state = 1;
			break;
		case 1:
			// Check if conversion is done and change to next state.
			if(ADC_0_is_conversion_done()) {
				adc_state = 2;
			}
			break;
		case 2:
			// Get conversion from specified ADC channel.
			result = ADC_0_get_conversion_result();
			// Calculate the input voltage on the ADC pin.
			// 5000.0 is the VREF (5V) in mV and 1023 is the resolution of 10 bits.
			voltage = (result * 5000.0) / 1023;
			// R1 and R2 are the voltage divider resistors
			voltage = (voltage * (R1 + R2)) / R2;
			// Clear the Result Ready Interrupt Flag.
			ADC0.INTFLAGS |= ADC_RESRDY_bm;			
			// Change state.
			adc_state = 0;
			break;
		default:
			break;
	}	
}


//
int main(void) {
	// Initializes MCU, drivers and middleware
	atmel_start_init();
	// Enable global interruptions
	sei();
	
	// Initialize variables.
	adc_state = 0;
	result = 0;	
	voltage = 0;	

	// Main Loop
	while (1) {
		// Measure the voltage on ADC input pin 4 (PA4).
		measure_voltage(ADC_MUXPOS_AIN4_gc);
	}
}
