#include <neewer_remote.h>

NeewerRemote::NeewerRemote(uint8_t cs) :
        _a7105(cs) {
}
 
NeewerRemote::~NeewerRemote() {}
 
void NeewerRemote::init(uint8_t channel) {

    uint8_t res = _a7105.reset();

    _a7105.setId(NEEWER_ID);
    _a7105.writeRegister(A7105_0F_CHANNEL, channel);

    _a7105.writeRegister(A7105_01_MODE_CONTROL, 0x42);
    _a7105.writeRegister(A7105_02_CALC, 0x00);
    _a7105.writeRegister(A7105_03_FIFOI, 0x05);
    _a7105.writeRegister(A7105_04_FIFOII, 0x00);
    _a7105.writeRegister(A7105_07_RC_OSC_I, 0x00);
    _a7105.writeRegister(A7105_08_RC_OSC_II, 0x00);
    _a7105.writeRegister(A7105_09_RC_OSC_III, 0x00);
    _a7105.writeRegister(A7105_0A_CK0_PIN, 0x00);
    _a7105.writeRegister(A7105_0B_GPIO1_PIN_I, 0x01);
    _a7105.writeRegister(A7105_0C_GPIO2_PIN_II, 0x09);
    _a7105.writeRegister(A7105_0D_CLOCK, 0x05);
    _a7105.writeRegister(A7105_0E_DATA_RATE, 0x04);

    _a7105.writeRegister(A7105_10_PLL_II, 0x9E);
    _a7105.writeRegister(A7105_11_PLL_III, 0x4B);
    _a7105.writeRegister(A7105_12_PLL_IV, 0x00);
    _a7105.writeRegister(A7105_13_PLL_V, 0x02);
    _a7105.writeRegister(A7105_14_TX_I, 0x16);
    _a7105.writeRegister(A7105_15_TX_II, 0x2B);
    _a7105.writeRegister(A7105_16_DELAY_I, 0x12);
    _a7105.writeRegister(A7105_17_DELAY_II, 0x00);

    _a7105.writeRegister(A7105_18_RX, 0x62);
    _a7105.writeRegister(A7105_19_RX_GAIN_I, 0x80);
    _a7105.writeRegister(A7105_1A_RX_GAIN_II, 0x80);
    _a7105.writeRegister(A7105_1B_RX_GAIN_III, 0x00);
    _a7105.writeRegister(A7105_1C_RX_GAIN_IV, 0x0A);

    _a7105.writeRegister(A7105_1D_RSSI_THOLD, 0x32);
    _a7105.writeRegister(A7105_1E_ADC, 0xC3);
    _a7105.writeRegister(A7105_1F_CODE_I, 0x07);
    _a7105.writeRegister(A7105_20_CODE_II, 0x16);
    _a7105.writeRegister(A7105_21_CODE_III, 0x00);

    _a7105.writeRegister(A7105_22_IF_CALIB_I, 0x00);
    _a7105.writeRegister(A7105_24_VCO_CURCAL, 0x13);
    _a7105.writeRegister(A7105_25_VCO_SBCAL_I, 0x00);
    _a7105.writeRegister(A7105_26_VCO_SBCAL_II, 0x3B);
    _a7105.writeRegister(A7105_27_BATTERY_DET, 0x00);
    _a7105.writeRegister(A7105_28_TX_TEST, 0x17);
    _a7105.writeRegister(A7105_29_RX_DEM_TEST_I, 0x07);
    _a7105.writeRegister(A7105_2A_RX_DEM_TEST_II, 0x80);

    _a7105.writeRegister(A7105_2B_CPC, 0x03);
    _a7105.writeRegister(A7105_2C_XTAL_TEST, 0x01);
    _a7105.writeRegister(A7105_2D_PLL_TEST, 0x45);

    _a7105.writeRegister(A7105_2E_VCO_TEST_I, 0x18);
    _a7105.writeRegister(A7105_2F_VCO_TEST_II, 0x00);
    _a7105.writeRegister(A7105_30_IFAT, 0x01);
    _a7105.writeRegister(A7105_31_RSCALE, 0x0F);
   
    //VCO Calibration
    _a7105.writeRegister(A7105_02_CALC, 0x1);
    unsigned long ms = millis();
    while(millis() - ms < 500) {
        if(!_a7105.readRegister(A7105_02_CALC))
            Serial.println("Calibration returned");
            Serial.println(millis() - ms);
            break;
    }
    if (millis() - ms >= 500) {
        Serial.println("Failed calibration");
        return;
    }
  //  _a7105.writeRegister(A7105_0F_PLL_I, 0x07);

    // It looks like the original code polls the calibration control register

    _a7105.writeRegister(A7105_22_IF_CALIB_I, 0x07);
    _a7105.writeRegister(A7105_24_VCO_CURCAL, 0x13);
    _a7105.writeRegister(A7105_25_VCO_SBCAL_I, 0x09);

}

void NeewerRemote::TurnOff(uint8_t channel) {

  uint8_t off_command[] = {0x7E, 0xA4, 0x01, 0x00, 0x00, 0x23};

  for (int i=0; i<4;i++) {
      _a7105.writeData(off_command, sizeof(off_command), channel);
  }
}

void NeewerRemote::TurnOn(uint8_t channel) {

  uint8_t on_command[] = {0x7E, 0xA4, 0x01, 0x01, 0x00, 0x24};

  for (int i=0; i<4;i++) {
      _a7105.writeData(on_command, sizeof(on_command), channel);
  }
}
