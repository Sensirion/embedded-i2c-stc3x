/*
 * I2C-Generator: 0.2.0
 * Yaml Version: 0.1.1
 * Template Version: 0.6.0
 */
/*
 * Copyright (c) 2021, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
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

#include <stdio.h>  // printf

#include "sensirion_common.h"
#include "sensirion_i2c_hal.h"
#include "stc3x_i2c.h"

/**
 * TO USE CONSOLE OUTPUT (PRINTF) IF NOT PRESENT ON YOUR PLATFORM
 */
//#define printf(...)
// TODO: DRIVER_GENERATOR Add missing commands and make prints more pretty

int main(void) {
    int16_t error = 0;

    sensirion_i2c_hal_init();

    uint32_t product_number;
    uint8_t serial_number[8];
    uint8_t serial_number_size = 8;

    error = stc3x_read_product_identifier(&product_number, serial_number,
                                          serial_number_size);

    if (error) {
        printf("Error executing stc3x_read_product_identifier(): %i\n", error);
    } else {
        printf("Product_number: %u\n", product_number);
        printf("Serial_number: ");
        for (size_t i = 0; i < serial_number_size; i++) {
            printf("%u, ", serial_number[i]);
        }
        printf("\n");
    }

    uint16_t self_test_output;

    error = stc3x_self_test(&self_test_output);

    if (error) {
        printf("Error executing stc3x_self_test(): %i\n", error);
    } else {
        printf("Self_test_output: %u\n", self_test_output);
    }

    // Start Measurement

    for (;;) {
        // Read Measurement
        // TODO: DRIVER_GENERATOR check and update measurement interval
        sensirion_i2c_hal_sleep_usec(1000000);
        // TODO: DRIVER_GENERATOR Add scaling and offset to printed measurement
        // values

        uint16_t gas_ticks;
        uint16_t temperature_ticks;

        error = stc3x_measure_gas_concentration(&gas_ticks, &temperature_ticks);

        if (error) {
            printf("Error executing stc3x_measure_gas_concentration(): %i\n",
                   error);
        } else {
            printf("Gas_ticks: %u\n", gas_ticks);
            printf("Temperature_ticks: %u\n", temperature_ticks);
        }
    }

    return 0;
}
