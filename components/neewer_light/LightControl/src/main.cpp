#include <Arduino.h>
#include <neewer_remote.h>

#define CHIP_SELECT D8
NeewerRemote neewerRemote(CHIP_SELECT);

bool run_once;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  Serial.println("Hello\n");

  pinMode(CHIP_SELECT, OUTPUT); // Chip select

  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);

  run_once = true;
}

void loop()
{

  if (run_once) {
    neewerRemote.init(NEEWER_CH1);
    run_once = false;

    neewerRemote.TurnOn(NEEWER_CH1);
  }

}

