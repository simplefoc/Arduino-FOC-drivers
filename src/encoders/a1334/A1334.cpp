
#include "./A1334.h"



A1334::A1334(SPISettings settings, int nCS) : settings(settings), nCS(nCS) {
	// nix
};



A1334::~A1334() {
};




void A1334::init(SPIClass* _spi) {
	spi = _spi;
	if (nCS>=0) {
		pinMode(nCS, OUTPUT);
		digitalWrite(nCS, HIGH);
	}
	//SPI has an internal SPI-device counter, it is possible to call "begin()" from different devices
	spi->begin();
	readRawAngle(); // read an angle
};




A1334Angle A1334::readRawAngle() {
	uint16_t command = A1334_REG_ANG;
	uint16_t cmdResult = spi_transfer16(command);	// TODO fast mode
	cmdResult = spi_transfer16(command);	
	A1334Angle result = { .reg = cmdResult };
	// TODO check parity
	// errorflag = result.ef;
	return result;
};




uint16_t A1334::spi_transfer16(uint16_t outdata) {
	spi->beginTransaction(settings);
	if (nCS>=0)
		digitalWrite(nCS, 0);
	uint16_t result = spi->transfer16(outdata);
	if (nCS>=0)
		digitalWrite(nCS, 1);
	spi->endTransaction();
	return result;
};

