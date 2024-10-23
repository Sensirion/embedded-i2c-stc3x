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
#include <inttypes.h>

/**
 * TO USE CONSOLE OUTPUT (PRINTF) IF NOT PRESENT ON YOUR PLATFORM
 */
// #define printf(...)

int main(void) {
    int16_t error = 0;

    sensirion_i2c_hal_init();

    error = stc3x_prepare_product_identifier();
    if (error) {
        printf("Error executing stc3x_prepare_product_identifier(): %i\n",
               error);
    }

    uint32_t product_number;
    uint64_t serial_number = 0;
    error = stc3x_get_product_id(&product_number, &serial_number);
    if (error) {
        printf("Error executing stc3x_read_product_identifier(): %i\n", error);
    } else {
        printf("Product Number: 0x%08x\n", product_number);
        printf("Serial Number: %" PRIu64 "\n", serial_number);
    }

    stc3x_test_result_t self_test_output;
    error = stc3x_self_test(&self_test_output);
    if (error) {
        printf("Error executing stc3x_self_test(): %i\n", error);
    } else {
        printf("Self Test: 0x%04x (OK = 0x0000)\n", self_test_output);
    }

    error = stc3x_set_binary_gas(0x0001);
    if (error) {
        printf("Error executing stc3x_set_binary_gas(): %i\n", error);
    } else {
        printf("Set binary gas to 0x0001\n");
    }

    uint16_t gas_ticks;
    uint16_t temperature_ticks;

    float gas;
    float temperature;

    for (;;) {
        // Read Measurement
        error =
            stc3x_measure_gas_concentration_raw(&gas_ticks, &temperature_ticks);
        if (error) {
            printf(
                "Error executing stc3x_measure_gas_concentration_raw(): %i\n",
                error);
        } else {
            gas = 100 * ((float)gas_ticks - 16384.0) / 32768.0;
            temperature = (float)temperature_ticks / 200.0;
            printf("Gas: %f - Temperature: %f\n", gas, temperature);
        }

        sensirion_i2c_hal_sleep_usec(1000000);
    }

    return 0;
}
