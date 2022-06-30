#include "a7105.h"
//#include "esphome/core/log.h"
 
namespace esphome {
namespace a7105 {

void A7105::setup() {
  //ESP_LOGI(TAG, "A7105 setup started!");
  this->spi_setup();

  this->cs_->digital_write(false);
  delay(10);
  //ESP_LOGI(TAG, "SPI setup finished!");
  //PN532::setup();
}

bool A7105::write_reg(uint8_t reg, uint8_t value) {
    // assert CS
    this->enable();

   // _spi.beginTransaction(SPISettings(A7105_SPI_FREQUENCY, MSBFIRST, SPI_MODE0));
    // write register
    this->transfer_byte(reg);
    // write value into register
    uint8_t ret = this->transfer_byte(value);
    // de-assert CS
    this->disable();
   // _spi.endTransaction();
    
    return true;
}
 
bool A7105::read_reg(uint8_t reg, uint8_t *value) {
    // assert CS
    this->enable();
    // write register and read value
    this->transfer_byte(_READ(reg));
    *value = this->transfer_byte(0);
    // de-assert CS
    this->disable();
    
    return true;
}
 
void A7105::strobe(enum A7105_State state) {
    // assert CS
    this->enable();

    // write strobe command
    uint8_t ret = this->transfer_byte(state);
    // de-assert CS
    this->disable();
    
    //return ret;
}
 
void A7105::write_data(const std::vector<uint8_t> &data, uint8_t channel) {
    // assert CS
    this->enable();
    this->transfer_byte(A7105_RST_WRPTR);
    this->transfer_byte(A7105_05_FIFO_DATA);
    for (size_t i = 0; i < data.size(); i++) {
        this->transfer_byte(data[i]);
    }
    this->disable();
    
    write_reg(A7105_0F_PLL_I, channel);
    strobe(A7105_TX);
}
 

//TODO: Change this to vector
void A7105::read_data(std::vector<uint8_t> &buffer, size_t len) {
    strobe(A7105_RST_RDPTR);

    buffer.resize(len);
    for (size_t i = 0; i < len; i++) {
        uint8_t read_value;
        read_reg(A7105_05_FIFO_DATA, &read_value);
        buffer[i] = read_value;
    }
}
 
void A7105::set_id(uint32_t id) {
    // assert CS
    this->enable();
    this->transfer_byte(A7105_06_ID_DATA);
    this->transfer_byte((uint8_t)(id >> 24) & 0xFF);
    this->transfer_byte((uint8_t)(id >> 16) & 0xFF);
    this->transfer_byte((uint8_t)(id >> 8) & 0xFF);
    this->transfer_byte((uint8_t)id & 0xFF);
    // de-assert CS
    this->disable();
}
 
void A7105::set_power(int32_t power) {
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
    write_reg(a7105::A7105_28_TX_TEST, (pac << 3) | tbg);
}
 
void A7105::set_tx_rx_mode(enum TXRX_State mode) {
    if(mode == TX_EN) {
        write_reg(A7105_0B_GPIO1_PIN_I, 0x33);
        write_reg(A7105_0C_GPIO2_PIN_II, 0x31);
    } else if (mode == RX_EN) {
        write_reg(A7105_0B_GPIO1_PIN_I, 0x31);
        write_reg(A7105_0C_GPIO2_PIN_II, 0x33);
    } else {
        //The A7105 seems to some with a cross-wired power-amp (A7700)
        //On the XL7105-D03, TX_EN -> RXSW and RX_EN -> TXSW
        //This means that sleep mode is wired as RX_EN = 1 and TX_EN = 1
        //If there are other amps in use, we'll need to fix this
        write_reg(A7105_0B_GPIO1_PIN_I, 0x33);
        write_reg(A7105_0C_GPIO2_PIN_II, 0x33);
    }
}
 
int8_t A7105::reset() {
    write_reg(A7105_00_MODE, 0x00);
    delayMicroseconds(1000);
    
    // Set both GPIO as output and low
   // setTxRxMode(TXRX_OFF);
    uint8_t result;
    read_reg(A7105_10_PLL_II, &result);
    strobe(A7105_STANDBY);
    
    return result == 0x9E;
}
}
}