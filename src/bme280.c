/*=============================================================================
 * Copyright 2016, Eric Pernia.
 * Copyright 2019, Miguel del Valle <m.e.delvallecamino@ieee.org>
 * All rights reserved.
 *
 * This file is part sAPI library for microcontrollers.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* Date: 2019-09-02 */

/*=====[Inclusions of function dependencies]=================================*/

#include "sapi_bme280.h" // tested on GY-BME/P280
#include "sapi.h"		// <= sAPI header

/*=====[Definition macros of private constants]==============================*/

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Main function, program entry point after power on or reset]==========*/
int main(void) {

	struct bme280_dev dev;
	int8_t rslt = BME280_OK;
	uint8_t settings_sel;
	struct bme280_data comp_data;

	dev.dev_id = BME280_I2C_ADDR_PRIM;
	dev.intf = BME280_I2C_INTF;
	dev.read = bme280I2cRead;
	dev.write = bme280I2cWrite;
	dev.delay_ms = bme280Delay_ms;

	// ----- init -----------------------------------

	boardInit();

	uartConfig(UART_USB, 9600); // Inicializar periferico UART_USB

	i2cInit(I2C0, BME280_I2C_RATE);

	rslt = bme280Init(&dev);

	if (rslt != BME280_OK) {
		printf("Fallo en la inicializacion (code %+d)\r\n", rslt);
	} else {
		printf("Resultado de la inicializacion (code %+d)\r\n", rslt);
		printf("Temperature, Pressure, Humidity\r\n");
	}
	// ----- bme280 sensor setup-----------------------------------

	/* Recommended mode operation: Indoor navigation */

	dev.settings.osr_h = BME280_OVERSAMPLING_1X;
	dev.settings.osr_p = BME280_OVERSAMPLING_1X;
	dev.settings.osr_t = BME280_OVERSAMPLING_16X;
	dev.settings.filter = BME280_FILTER_COEFF_16;
	dev.settings.standby_time = BME280_STANDBY_TIME_62_5_MS;

	settings_sel = BME280_OSR_PRESS_SEL;
	settings_sel |= BME280_OSR_TEMP_SEL;
	settings_sel |= BME280_OSR_HUM_SEL;
	settings_sel |= BME280_STANDBY_SEL;
	settings_sel |= BME280_FILTER_SEL;

	rslt = bme280SetSensorSettings(settings_sel, &dev);
	rslt = bme280SetSensorMode(BME280_NORMAL_MODE, &dev);

	/* ------------- REPETIR POR SIEMPRE ------------- */
	while (TRUE) {

		/* Wait for the measurement to complete and print data @25Hz */
		dev.delay_ms(2750);

		rslt = bme280GetSensorData(BME280_ALL, &comp_data, &dev);

		bme280PrintSensorData(&comp_data);

		gpioToggle(LED);

	}

	// YOU NEVER REACH HERE, because this program runs directly or on a
	// microcontroller and is not called by any Operating System, as in the
	// case of a PC program.
	return 0;
}
