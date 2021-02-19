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

#include "i2c_mux.h"
#include "sensirion_common.h"
#include "sensirion_i2c.h"
#include "sensirion_i2c_hal.h"
#include "sensirion_test_setup.h"
#include "stc3x_i2c.h"
#include <inttypes.h>
#include <stdio.h>

TEST_GROUP (STC3X_Tests) {
    void setup() {
        sensirion_i2c_hal_init();

        // Select Mux 2 channel 6
        int16_t error = sensirion_i2c_mux_set_single_channel(0x72, 6);
        CHECK_EQUAL_ZERO_TEXT(error, "sensirion_i2c_mux_set_single_channel")
    }

    void teardown() {
        sensirion_i2c_general_call_reset();
        sensirion_i2c_hal_sleep_usec(100000);  // wait after reset
        sensirion_i2c_hal_free();
    }
};

TEST (STC3X_Tests, STC3X_Test_set_binary_gas) {
    int16_t error;
    uint16_t binary_gas = 0x0001;
    error = stc3x_set_binary_gas(binary_gas);
    CHECK_EQUAL_ZERO_TEXT(error, "stc3x_set_binary_gas");
}

TEST (STC3X_Tests, STC3X_Test_set_relative_humidity) {
    int16_t error;
    uint16_t relative_humidity_ticks = 0;
    error = stc3x_set_relative_humidity(relative_humidity_ticks);
    CHECK_EQUAL_ZERO_TEXT(error, "stc3x_set_relative_humidity");
}

TEST (STC3X_Tests, STC3X_Test_set_temperature) {
    int16_t error;
    uint16_t temperature_ticks = 0;
    error = stc3x_set_temperature(temperature_ticks);
    CHECK_EQUAL_ZERO_TEXT(error, "stc3x_set_temperature");
}

TEST (STC3X_Tests, STC3X_Test_set_pressure) {
    int16_t error;
    uint16_t absolue_pressure = 1000;
    error = stc3x_set_pressure(absolue_pressure);
    CHECK_EQUAL_ZERO_TEXT(error, "stc3x_set_pressure");
}

TEST (STC3X_Tests, STC3X_Test_measure_gas_concentration) {
    int16_t error;
    error = stc3x_set_binary_gas(0x0001);
    CHECK_EQUAL_ZERO_TEXT(error, "stc3x_set_binary_gas");

    uint16_t gas_ticks;
    uint16_t temperature_ticks;
    error = stc3x_measure_gas_concentration(&gas_ticks, &temperature_ticks);
    CHECK_EQUAL_ZERO_TEXT(error, "stc3x_measure_gas_concentration");
    printf("gas_ticks: %u\n", gas_ticks);
    printf("temperature_ticks: %u\n", temperature_ticks);
}

TEST (STC3X_Tests, STC3X_Test_forced_recalibration) {
    int16_t error;
    uint16_t reference_concentration = 0;
    error = stc3x_forced_recalibration(reference_concentration);
    CHECK_EQUAL_ZERO_TEXT(error, "stc3x_forced_recalibration");
}

TEST (STC3X_Tests, STC3X_Test_enable_automatic_self_calibration) {
    int16_t error;
    error = stc3x_enable_automatic_self_calibration();
    CHECK_EQUAL_ZERO_TEXT(error, "stc3x_enable_automatic_self_calibration");
}

TEST (STC3X_Tests, STC3X_Test_disable_automatic_self_calibration) {
    int16_t error;
    error = stc3x_disable_automatic_self_calibration();
    CHECK_EQUAL_ZERO_TEXT(error, "stc3x_disable_automatic_self_calibration");
}

TEST (STC3X_Tests, STC3X_Test_get_and_set_sensor_state) {
    int16_t error;
    error = stc3x_prepare_read_state();
    CHECK_EQUAL_ZERO_TEXT(error, "stc3x_prepare_read_state");

    uint8_t state[30];
    uint8_t state_size = 30;
    error = stc3x_get_sensor_state(&state[0], state_size);
    CHECK_EQUAL_ZERO_TEXT(error, "stc3x_get_sensor_state");
    printf("state: %s\n", state);

    error = stc3x_set_sensor_state(&state[0], state_size);
    CHECK_EQUAL_ZERO_TEXT(error, "stc3x_set_sensor_state");

    error = stc3x_apply_state();
    CHECK_EQUAL_ZERO_TEXT(error, "stc3x_apply_state");
}

TEST (STC3X_Tests, STC3X_Test_self_test) {
    int16_t error;
    uint16_t self_test_output;
    error = stc3x_self_test(&self_test_output);
    CHECK_EQUAL_ZERO_TEXT(error, "stc3x_self_test");
    printf("self_test_output: %u\n", self_test_output);
}

TEST (STC3X_Tests, STC3X_Test_enter_sleep_mode) {
    int16_t error;
    error = stc3x_enter_sleep_mode();
    CHECK_EQUAL_ZERO_TEXT(error, "stc3x_enter_sleep_mode");

    sensirion_i2c_hal_sleep_usec(100000);  // in sleep

    stc3x_set_binary_gas(0x0001);          // wakeup
    sensirion_i2c_hal_sleep_usec(100000);  // wait for wakeup

    error = stc3x_set_binary_gas(0x0001);
    CHECK_EQUAL_ZERO_TEXT(error, "stc3x_set_binary_gas");

    uint16_t gas_ticks;
    uint16_t temperature_ticks;
    error = stc3x_measure_gas_concentration(&gas_ticks, &temperature_ticks);
    CHECK_EQUAL_ZERO_TEXT(error, "stc3x_measure_gas_concentration");
    printf("gas_ticks: %u\n", gas_ticks);
    printf("temperature_ticks: %u\n", temperature_ticks);
}

TEST (STC3X_Tests, STC3X_Test_read_product_identifier) {
    int16_t error;
    error = stc3x_prepare_product_identifier();
    CHECK_EQUAL_ZERO_TEXT(error, "stc3x_prepare_product_identifier");

    uint32_t product_number;
    uint8_t serial_number[8];
    uint8_t serial_number_size = 8;
    error = stc3x_read_product_identifier(&product_number, &serial_number[0],
                                          serial_number_size);
    CHECK_EQUAL_ZERO_TEXT(error, "stc3x_read_product_identifier");
    printf("product_number: %u\n", product_number);
    printf("serial_number: %s\n", serial_number);
}
