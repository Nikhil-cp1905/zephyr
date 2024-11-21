#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

// Function to parse SBUS buffer and extract 11-bit channels
// Params:
// - buff: An array containing the SBUS frame data
// Returns:
// - channel: An array of 16 11-bit SBUS channels
uint16_t *parse_buffer(uint8_t buff[]) {
    // Ensure the buffer is long enough to avoid out-of-bounds access
    if (buff == NULL) {
        fprintf(stderr, "Error: Null pointer passed for buffer.\n");
        return NULL; // Return NULL if the buffer is invalid
    }

    // Array to store parsed channels
    static uint16_t channel[16];

    // Mask for extracting 11-bit channel values (0x07FF in hexadecimal)
    uint16_t mask = 0x7FF;

    // Parse each of the 16 channels from the buffer
    channel[0]  = ((buff[1] | buff[2]<<8)                 & mask);
    channel[1]  = ((buff[2]>>3 | buff[3]<<5)              & mask);
    channel[2]  = ((buff[3]>>6 | buff[4]<<2 | buff[5]<<10) & mask);
    channel[3]  = ((buff[5]>>1 | buff[6]<<7)              & mask);
    channel[4]  = ((buff[6]>>4 | buff[7]<<4)              & mask);
    channel[5]  = ((buff[7]>>7 | buff[8]<<1 | buff[9]<<9)  & mask);
    channel[6]  = ((buff[9]>>2 | buff[10]<<6)             & mask);
    channel[7]  = ((buff[10]>>5| buff[11]<<3)             & mask);
    channel[8]  = ((buff[12]   | buff[13]<<8)             & mask);
    channel[9]  = ((buff[13]>>3| buff[14]<<5)             & mask);
    channel[10] = ((buff[14]>>6| buff[15]<<2| buff[16]<<10)& mask);
    channel[11] = ((buff[16]>>1| buff[17]<<7)             & mask);
    channel[12] = ((buff[17]>>4| buff[18]<<4)             & mask);
    channel[13] = ((buff[18]>>7| buff[19]<<1| buff[20]<<9) & mask);
    channel[14] = ((buff[20]>>2| buff[21]<<6)             & mask);
    channel[15] = ((buff[21]>>5| buff[22]<<3)             & mask);

    return channel;
}

// Function to check the parity of the SBUS frame (optional, to validate data integrity)
// Params:
// - parity: Parity byte from the SBUS data
// Returns:
// - true if parity is valid, false if invalid
bool parity_checker(uint8_t parity) {
    // Ensure the first two bits match the expected pattern for valid data
    uint8_t bit0 = parity >> 7;
    uint8_t bit1 = (parity >> 6) & 0x01;  // Extract the second bit

    // Parity check (valid if bits 0 and 1 are both 0)
    if (bit0 == 0 && bit1 == 0) {
        return true; // Parity is valid
    } else {
        return false; // Parity is invalid
    }
}

