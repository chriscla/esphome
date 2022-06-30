#ifndef _NEEWER_REMOTE_H
#define _NEEWER_REMOTE_H

#include <Arduino.h>
#include <a7105.h>

#define NEEWER_ID 0xD253694D

#define NEEWER_CH1 0x3
#define NEEWER_CH2 0x7

 
/**
 * Class for interfacing with the AMICCOM A7105 2.4G FSK/GFSK Transceiver
 *
 * See the A7105 datasheet for complete documentation on this part
 * http://www.avantcom.com.tw/AVANTCOM/TC/DATA/PRODUCT/SOLVE/18_3.pdf
 *
 *
 * Example:
 * @code
 * #include "mbed.h"
 * #include "A7105.h"
 *
 * #define A7105_SPI_FREQUENCY  10000000 // 10MHz
 * 
 * A7105 txrx(D4, D5, D3, D6, A7105_SPI_FREQUENCY);
 * 
 * int main() {
 *     // reset
 *     ret = txrx.reset();
 *     // use GPIO1 as miso
 *     ret = txrx.writeRegister(A7105_0B_GPIO1_PIN_I, 0x19);
 *     // set various radio options
 *     ret = txrx.writeRegister(A7105_01_MODE_CONTROL, 0x63);
 *     // set packet length (FIFO end pointer) to 0x0f + 1 == 16
 *     ret = txrx.writeRegister(A7105_03_FIFOI, 0x0f);
 *     // select crystal oscillator and system clock divider of 1/2
 *     ret = txrx.writeRegister(A7105_0D_CLOCK, 0x05);
 * 
 *     // sanity check
 *     ret = txrx.readRegister(A7105_0D_CLOCK);
 *     if (ret != 0x05) {
 *         // do something :)
 *     }
 * }
 * @endcode
 */
class NeewerRemote {
    public:
        /**
         * @param mosi Pin used to transmit data to the slave
         * @param miso Pin used to receive data from the slave
         * @param clk Pin used for the clock
         * @param cs Pin used for the chip select
         * @param freqHz Frequency used to clock data in and out
         */
        NeewerRemote(uint8_t cs);
        ~NeewerRemote();
        
        void init(uint8_t channel);
        void TurnOff(uint8_t channel);

        void TurnOn(uint8_t channel);
        
    private:        
        A7105 _a7105;
};
 
#endif  // #ifndef _NEEWER_REMOTE_H