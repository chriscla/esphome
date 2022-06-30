#include <a7105.h>

A7105::A7105(uint8_t cs, uint32_t freqHz) :
        _spi(), _cs(cs) {
    //_spi.frequency(freqHz);
    // Chip select is active low so set it to high until we are ready
    deassertChipSelect();
    // _cs = 1;
}
 
A7105::~A7105() {}
 
uint8_t A7105::writeRegister(uint8_t regAddr, uint8_t value) {
    // assert CS
    assertChipSelect();

   // _spi.beginTransaction(SPISettings(A7105_SPI_FREQUENCY, MSBFIRST, SPI_MODE0));
    // write register
    _spi.transfer(regAddr);
    // write value into register
    uint8_t ret = _spi.transfer(value);
    // de-assert CS
    deassertChipSelect();
   // _spi.endTransaction();
    
    return ret;
}
 
uint8_t A7105::readRegister(uint8_t regAddr) {
    // assert CS
    assertChipSelect();
    // write register and read value
    _spi.transfer(_READ(regAddr));
    uint8_t ret = _spi.transfer(0);
    // de-assert CS
    deassertChipSelect();
    
    return ret;
}
 
uint8_t A7105::strobe(enum A7105_State state) {
    // assert CS
    assertChipSelect();
    //_spi.beginTransaction(SPISettings(A7105_SPI_FREQUENCY, MSBFIRST, SPI_MODE0));

    // write strobe command
    uint8_t ret = _spi.transfer(state);
    // de-assert CS
    deassertChipSelect();
    //_spi.endTransaction();
    
    return ret;
}
 
void A7105::writeData(uint8_t* data, size_t len, uint8_t channel) {
    // assert CS
    assertChipSelect();
    _spi.write(A7105_RST_WRPTR);
    _spi.write(A7105_05_FIFO_DATA);
    for (size_t i = 0; i < len; i++) {
        _spi.write(data[i]);
    }
    // de-assert CS
    deassertChipSelect();
    
    writeRegister(A7105_0F_PLL_I, channel);
    strobe(A7105_TX);
}
 
void A7105::readData(uint8_t* buffer, size_t len) {
    strobe(A7105_RST_RDPTR);
    for (size_t i = 0; i < len; i++) {
        buffer[i] = readRegister(A7105_05_FIFO_DATA);
    }
}
 
void A7105::setId(uint32_t id) {
    // assert CS
    assertChipSelect();
    _spi.transfer(A7105_06_ID_DATA);
    _spi.transfer((uint8_t)(id >> 24) & 0xFF);
    _spi.transfer((uint8_t)(id >> 16) & 0xFF);
    _spi.transfer((uint8_t)(id >> 8) & 0xFF);
    _spi.transfer((uint8_t)id & 0xFF);
    // de-assert CS
    deassertChipSelect();
}
 
void A7105::setPower(int32_t power) {
   /*
    Power amp is ~+16dBm so:
    TXPOWER_100uW  = -23dBm == PAC=0 TBG=0
    TXPOWER_300uW  = -20dBm == PAC=0 TBG=1
    TXPOWER_1mW    = -16dBm == PAC=0 TBG=2
    TXPOWER_3mW    = -11dBm == PAC=0 TBG=4
    TXPOWER_10mW   = -6dBm  == PAC=1 TBG=5
    TXPOWER_30mW   = 0dBm   == PAC=2 TBG=7
    TXPOWER_100mW  = 1dBm   == PAC=3 TBG=7
    TXPOWER_150mW  = 1dBm   == PAC=3 TBG=7
    */
    uint8_t pac, tbg;
    switch(power) {
        case 0: pac = 0; tbg = 0; break;
        case 1: pac = 0; tbg = 1; break;
        case 2: pac = 0; tbg = 2; break;
        case 3: pac = 0; tbg = 4; break;
        case 4: pac = 1; tbg = 5; break;
        case 5: pac = 2; tbg = 7; break;
        case 6: pac = 3; tbg = 7; break;
        case 7: pac = 3; tbg = 7; break;
        default: pac = 0; tbg = 0; break;
    };
    writeRegister(A7105_28_TX_TEST, (pac << 3) | tbg);
}
 
void A7105::setTxRxMode(enum TXRX_State mode) {
    if(mode == TX_EN) {
        writeRegister(A7105_0B_GPIO1_PIN_I, 0x33);
        writeRegister(A7105_0C_GPIO2_PIN_II, 0x31);
    } else if (mode == RX_EN) {
        writeRegister(A7105_0B_GPIO1_PIN_I, 0x31);
        writeRegister(A7105_0C_GPIO2_PIN_II, 0x33);
    } else {
        //The A7105 seems to some with a cross-wired power-amp (A7700)
        //On the XL7105-D03, TX_EN -> RXSW and RX_EN -> TXSW
        //This means that sleep mode is wired as RX_EN = 1 and TX_EN = 1
        //If there are other amps in use, we'll need to fix this
        writeRegister(A7105_0B_GPIO1_PIN_I, 0x33);
        writeRegister(A7105_0C_GPIO2_PIN_II, 0x33);
    }
}
 
int8_t A7105::reset() {
    writeRegister(A7105_00_MODE, 0x00);
    delayMicroseconds(1000);
    
    // Set both GPIO as output and low
   // setTxRxMode(TXRX_OFF);
    int8_t result = readRegister(A7105_10_PLL_II) == 0x9E;
    strobe(A7105_STANDBY);
    
    return result;
}
         