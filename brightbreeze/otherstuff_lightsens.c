#include <stdio.h>

// ... [include and define the necessary functions as shown in the previous answer] ...

// Define a function to read the sensor data and print it
void read_and_display_data(uint8_t channel) {
    tca9548a_select_channel(channel);
    uint16_t ch0_data = tsl2591_read_als_data(0x14); // CH0 lower byte
    uint16_t ch1_data = tsl2591_read_als_data(0x16); // CH1 lower byte

    // Combine the lower and upper bytes to obtain the 16-bit values
    ch0_data |= (tsl2591_read_als_data(0x15) << 8); // CH0 upper byte
    ch1_data |= (tsl2591_read_als_data(0x17) << 8); // CH1 upper byte

    printf("Sensor %d - CH0: %d, CH1: %d\n", channel, ch0_data, ch1_data);
}

int main(void) {
    // ... [initialize the I2C communication and the TSL2591 sensors as shown in the previous answer] ...

    while (1) {
        for (uint8_t channel = 0; channel < 2; channel++) {
            read_and_display_data(channel);
        }

        _delay_ms(1000); // Add a delay between readings, e.g., 1 second
    }

    return 0;
}


/*calculate lux*/
float calculate_lux(uint16_t ch0_data, uint16_t ch1_data) {
    float lux = 0.0;
    float ratio = 0.0;

    if (ch0_data != 0) {
        ratio = (float)ch1_data / (float)ch0_data;
    }

    if (ratio < 0.5) {
        lux = (0.0304 * ch0_data) - (0.062 * ch0_data * pow(ratio, 1.4));
    } else if (ratio < 0.61) {
        lux = (0.0224 * ch0_data) - (0.031 * ch1_data);
    } else if (ratio < 0.8) {
        lux = (0.0128 * ch0_data) - (0.0153 * ch1_data);
    } else if (ratio < 1.3) {
        lux = (0.00146 * ch0_data) - (0.00112 * ch1_data);
    }

    return lux;
}
