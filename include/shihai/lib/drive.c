#include "drive.h"
#include <stdio.h>

#define SBUS_DEADZONE_MIN 950
#define SBUS_DEADZONE_MAX 1050
#define SBUS_MIN 0
#define SBUS_MAX 2047
#define PWM_MIN 1000
#define PWM_MAX 2000

// Function to interpolate sbus channel value to velocity
// Params:
// - channel: SBUS channel value (11-bit)
// - velocity_range: Array containing the min and max velocity for interpolation
// - channel_range: Array containing the min and max SBUS channel range
// Returns: Interpolated velocity value based on the channel
float sbus_velocity_interpolation(uint16_t channel, float *velocity_range, int *channel_range) {
    if (!velocity_range || !channel_range) {
        fprintf(stderr, "Error: Null pointer passed for range.\n");
        return 0.0f; // Return 0 in case of error
    }

    // Ensure channel is within the valid range
    if (channel < channel_range[0]) return velocity_range[0];
    if (channel > channel_range[1]) return velocity_range[1];

    // Handle deadzone for smooth control
    if (channel >= SBUS_DEADZONE_MIN && channel <= SBUS_DEADZONE_MAX) {
        return (velocity_range[0] + velocity_range[1]) / 2;
    }

    // Calculate interpolation ratio
    float channel_diff = (float)(channel_range[1] - channel_range[0]);
    float velocity_diff = velocity_range[1] - velocity_range[0];
    return velocity_range[0] + (velocity_diff / channel_diff) * (channel - channel_range[0]);
}

// Function to interpolate sbus channel value to PWM
// Params:
// - channel: SBUS channel value (11-bit)
// - pwm_range: Array containing the min, mid, and max PWM for interpolation
// - channel_range: Array containing the min and max SBUS channel range
// Returns: Interpolated PWM value based on the channel
uint32_t sbus_pwm_interpolation(uint16_t channel, uint32_t *pwm_range, uint16_t *channel_range) {
    if (!pwm_range || !channel_range) {
        fprintf(stderr, "Error: Null pointer passed for range.\n");
        return 0; // Return 0 in case of error
    }

    // Ensure channel is within the valid range
    if (channel < channel_range[0]) return pwm_range[0];
    if (channel > channel_range[1]) return pwm_range[2];

    // Handle deadzone for smooth control
    if (channel >= 995 && channel <= 1005) {
        return (pwm_range[0] + pwm_range[2]) / 2;
    }

    // Calculate interpolation ratio
    float channel_diff = (float)(channel_range[1] - channel_range[0]);
    float pwm_diff = pwm_range[1] - pwm_range[2];
    return pwm_range[0] + (pwm_diff / channel_diff) * (channel - channel_range[0]);
}

// Helper function to set GPIO pin state and stepper motor direction
// Params:
// - motor: Motor object containing the stepper motor configuration
// - channel: SBUS channel value
// - channel_range: Channel range for the interpolation
// Returns: 0 if successful, 1 if unsuccessful
int stepper_motor_write(const struct stepper_motor *motor, uint16_t channel, uint16_t *channel_range) {
    if (!motor || !channel_range) {
        fprintf(stderr, "Error: Null pointer passed for motor or range.\n");
        return 1; // Error due to null pointer
    }

    int ret = 0;
    int pos = 0; // Assume initial position is zero

    // Determine motor direction based on the channel value
    if (channel > channel_range[1]) {
        gpio_pin_set_dt(&(motor->dir), 1); // Set direction clockwise
        pos += 1;
    } else {
        gpio_pin_set_dt(&(motor->dir), 0); // Set direction counter-clockwise
        pos -= 1;
    }

    // Move motor one step forward or backward
    switch (pos & 0x03) {
        case 0:
            ret += gpio_pin_set_dt(&(motor->step), 0);
            break;
        case 1:
            ret += gpio_pin_set_dt(&(motor->step), 1);
            break;
        case 2:
            ret += gpio_pin_set_dt(&(motor->step), 1);
            break;
        case 3:
            ret += gpio_pin_set_dt(&(motor->step), 0);
            break;
        default:
            ret = 1; // Return error if something goes wrong
            break;
    }

    return ret;
}

// Function to calculate the delay required for a given speed in rpm
// Params:
// - speed: Speed in RPM (revolutions per minute)
// Returns: The time interval between steps in microseconds
float set_speed(float speed) {
    if (speed == 0.0f) {
        return 0.0f; // No movement, so no delay
    }
    
    const float pulse_per_rev = 200;  // Example value for pulse per revolution
    const float minutes_to_micro = 60000000;  // 1 minute in microseconds
    return minutes_to_micro / (pulse_per_rev * speed);
}


