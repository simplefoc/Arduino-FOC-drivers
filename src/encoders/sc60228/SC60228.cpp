
#include "./SC60228.h"
#include "Arduino.h"

SC60228::SC60228(SPISettings settings, int nCS) : settings(settings), nCS(nCS) {
    // nix
};

SC60228::~SC60228() { };

void SC60228::init(SPIClass* _spi) {
    spi = _spi;
	if (nCS>=0)
		pinMode(nCS, OUTPUT);
	digitalWrite(nCS, HIGH);
    delay(1);
	spi->begin();
	readRawAngle();
};



SC60228Angle SC60228::readRawAngle(){
    SC60228Angle result;
    result.reg = spi_transfer16(0x0000);
    errorflag = (result.err==1);
    // TODO check parity
    // Serial.print("0x");
    // Serial.print(result.angle, HEX);
    // Serial.print(" - 0x");
    // Serial.println(result.reg, HEX);
    return result;
};



bool SC60228::isError() {
    return errorflag;
};



uint16_t SC60228::spi_transfer16(uint16_t outdata){
    uint16_t result;
    spi->beginTransaction(settings);
    if (nCS>=0)
        digitalWrite(nCS, LOW);
    // min delay here: 250ns
    result = spi->transfer16(outdata);
    // min delay here: clock period / 2
    if (nCS>=0)
        digitalWrite(nCS, HIGH);
    spi->endTransaction();
    // min delay until next read: 250ns
    return result;
};


