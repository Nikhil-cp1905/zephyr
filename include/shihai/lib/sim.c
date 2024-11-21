#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "drive.h"   // Include the header file for drive-related functions
#include "sbus.h"    // Include the header file for SBUS-related functions

#define SBUS_CHANNELS 16

// Simulated SBUS data buffer (you can modify these values to simulate different inputs)
uint8_t simulated_sbus_buffer[25] = {
    0x0F, 0x01, 0xFF, 0x05, 0x2A, 0x40, 0x65, 0x7F, 0x99, 0xAB,
    0xC5, 0xDE, 0xF0, 0x13, 0x28, 0x4B, 0x5F, 0x6D, 0x7C, 0x8F,
    0xA0, 0xB1, 0xC3, 0xD4, 0xE5
};

// Function to simulate SBUS signal processing
void simulate_sbus_signal(uint8_t *buffer) {
    // Parse the SBUS buffer and convert to 11-bit channel values
    uint16_t *channels = parse_buffer(buffer);

    if (channels == NULL) {
        printf("Error: Invalid SBUS data.\n");
        return;
    }

    // Print out parsed SBUS channels for debugging
    printf("Parsed SBUS Channels:\n");
    for (int i = 0; i < SBUS_CHANNELS; i++) {
        printf("Channel %d: %d\n", i + 1, channels[i]);
    }

    // Now, convert the SBUS channels to PWM values
    for (int i = 0; i < SBUS_CHANNELS; i++) {
        uint32_t pwm_value = one_hot_interpolation(channels[i], (uint32_t[]){1000, 2000, 2000}, (uint16_t[]){0, 2047});
        printf("PWM Value for Channel %d: %u\n", i + 1, pwm_value);
    }
}

int main() {
    printf("SBUS to PWM Simulation\n");

    // Simulate the SBUS signal processing
    simulate_sbus_signal(simulated_sbus_buffer);

    return 0;
}

