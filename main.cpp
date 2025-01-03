/*
PI PICO Demonstration of interface to an si5351

# Make sure that the PICO_SDK_PATH is set properly

cd /home/bruce/pico/hello-pico-si5351
# You need main.c and CMakeLists.txt
cp ../pico-sdk/external/pico_sdk_import.cmake .
mkdir build
cd build
cmake ..
make

# Make sure the SWD is connected properly:
# GPIO24 (Pin 18) to SWDIO
# GPIO25 (Pin 22) to SWCLK
# GND (Pin 20) to SWGND

# Use this command to flash:
openocd -f interface/raspberrypi-swd.cfg -f target/rp2040.cfg -c "program main.elf verify reset exit"

# Looking at the serial port:
minicom -b 115200 -o -D /dev/ttyACM0
*/
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

#include "si5351.h"

#define I2C0_SDA_PIN (4)  // Pin 6  - data
#define I2C0_SCL_PIN (5)  // Pin 7 - clock
#define LED_PIN (25)

int main(int, const char**) {
 
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // This example will use I2C0!
    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(I2C0_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C0_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C0_SDA_PIN);
    gpio_pull_up(I2C0_SDA_PIN);

    // Startup ID
    for (uint i = 0; i < 2; i++) {
        gpio_put(LED_PIN, 1);
        sleep_ms(500);
        gpio_put(LED_PIN, 0);
        sleep_ms(500);
    }    

    puts("SI5351 Demo");

    // NOTE NOTE NOTE NOTE
    // We are using I2C0 here!
    si_init(i2c0);
    // NOTE NOTE NOTE NOTE
    si_enable(0, true);

    int32_t freq = 7255000;
    int32_t cal = 490;

    // Change freq
    si_evaluate(0, freq + cal);

    // Prevent the main fom exiting
    while (1) { 
        int c = getchar_timeout_us(0);
        if (c > 0) {
            if (c == '-') {
                freq -= 1000;
            } else if (c == '=') {
                freq += 1000;
            }
            si_evaluate(0, freq + cal);
            char temp[64];
            sprintf(temp,"%d\n", freq);
            puts(temp);
        }

    }

    return 0;
}
