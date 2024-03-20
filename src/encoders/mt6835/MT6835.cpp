
#include "./MT6835.h"
#include "common/foc_utils.h"


MT6835::MT6835(SPISettings settings, int nCS) : settings(settings), nCS(nCS) {
    // nix
};

MT6835::~MT6835() {
    // nix
};



void MT6835::init(SPIClass* _spi) {
    spi = _spi;
    if (nCS >= 0) {
        pinMode(nCS, OUTPUT);
        digitalWrite(nCS, HIGH);
    }
    spi->begin();
};




float MT6835::getCurrentAngle(){
    uint32_t rawangle = readRawAngle21();
    if (checkcrc) {
        if (lastcrc != calcCrc(rawangle, laststatus)) {
            laststatus |= MT6835_CRC_ERROR;
            return -1; // return -1 to signal CRC error - the current angle has to be non-negative otherwise
        }
    }
    return rawangle / (float)MT6835_CPR * _2PI;
};


// calculate crc8 of 21 angle bits and 3 status bits
// polynomial: x^8 + x^2 + x + 1 = 0x07, (0xE0 reflected) init is 0x00, no final xor
// TOOD table-based version
uint8_t MT6835::calcCrc(uint32_t angle, uint8_t status) {
    uint8_t crc = 0x00;

    uint8_t input = angle>>13;
    crc ^= input;
    for (int k = 8; k > 0; k--)
        crc = (crc & (0x01<<7))?(crc<<1)^0x07:crc<<1;

    input = (angle>>5) & 0xFF;
    crc ^= input;
    for (int k = 8; k > 0; k--)
        crc = (crc & (0x01<<7))?(crc<<1)^0x07:crc<<1;

    input = ((angle<<3) & 0xFF) | (status & 0x07);
    crc ^= input;
    for (int k = 8; k > 0; k--)
        crc = (crc & (0x01<<7))?(crc<<1)^0x07:crc<<1;

    return crc;
};



uint32_t MT6835::readRawAngle21(){
    uint8_t data[6]; // transact 48 bits
    data[0] = (MT6835_OP_ANGLE<<4);
    data[1] = MT6835_REG_ANGLE1;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = 0;
    spi->beginTransaction(settings);
    if (nCS >= 0)
        digitalWrite(nCS, LOW);
    spi->transfer(data, 6);
    if (nCS >= 0)
        digitalWrite(nCS, HIGH);
    spi->endTransaction();
    laststatus = data[4]&0x07;
    lastcrc = data[5];
    return (data[2] << 13) | (data[3] << 5) | (data[4] >> 3);
};


uint8_t MT6835::getStatus(){
    return laststatus;
};

uint8_t MT6835::getCalibrationStatus(){
    uint8_t data[3] = {0};
    data[0] = MT6835_OP_READ << 4 | MT6835_REG_CAL_STATUS >> 8;
    data[1] = MT6835_REG_CAL_STATUS & 0xFF;

    spi->beginTransaction(settings);
    if(nCS >= 0)
        digitalWrite(nCS, LOW);
    spi->transfer(data, 3);
    if(nCS >= 0)
        digitalWrite(nCS, HIGH);
    spi->endTransaction();

    return data[2] >> 6;
}

bool MT6835::setZeroFromCurrentPosition(){
    MT6835Command cmd;
    cmd.cmd = MT6835_OP_ZERO;
    cmd.addr = 0x000;
    transfer24(&cmd);
    return cmd.data == MT6835_WRITE_ACK;
};


/**
 * Wait 6s after calling this method
 */
bool MT6835::writeEEPROM(){
    delay(1); // wait at least 1ms
    MT6835Command cmd;
    cmd.cmd = MT6835_OP_PROG;
    cmd.addr = 0x000;
    transfer24(&cmd);
    return cmd.data == MT6835_WRITE_ACK;
};





uint8_t MT6835::getBandwidth(){
    MT6835Options5 opts = { .reg = readRegister(MT6835_REG_OPTS5) };
    return opts.bw;
};
void MT6835::setBandwidth(uint8_t bw){
    MT6835Options5 opts = { .reg = readRegister(MT6835_REG_OPTS5) };
    opts.bw = bw;
    writeRegister(MT6835_REG_OPTS5, opts.reg);
};

uint8_t MT6835::getHysteresis(){
    MT6835Options3 opts = { .reg = getOptions3().reg };
    return opts.hyst;
};
void MT6835::setHysteresis(uint8_t hyst){
    MT6835Options3 opts = { .reg = getOptions3().reg };
    opts.hyst = hyst;
    setOptions3(opts);
};

uint8_t MT6835::getRotationDirection(){
    MT6835Options3 opts = { .reg = getOptions3().reg };
    return opts.rot_dir;
};
void MT6835::setRotationDirection(uint8_t dir){
    MT6835Options3 opts = { .reg = getOptions3().reg };
    opts.rot_dir = dir;
    setOptions3(opts);
};


uint16_t MT6835::getABZResolution(){
    uint8_t hi = readRegister(MT6835_REG_ABZ_RES1);
    MT6835ABZRes lo = {
			.reg = readRegister(MT6835_REG_ABZ_RES2)
	};
    return (hi << 6) | lo.abz_res_low;
};
void MT6835::setABZResolution(uint16_t res){
    uint8_t hi = (res >> 6);
    MT6835ABZRes lo = {
			.reg = readRegister(MT6835_REG_ABZ_RES2)
	};
    lo.abz_res_low = (res & 0x3F);
    writeRegister(MT6835_REG_ABZ_RES1, hi);
    writeRegister(MT6835_REG_ABZ_RES2, lo.reg);
};



bool MT6835::isABZEnabled(){
    MT6835ABZRes lo = {
			.reg = readRegister(MT6835_REG_ABZ_RES2)
	};
    return lo.abz_off==0;
};
void MT6835::setABZEnabled(bool enabled){
    MT6835ABZRes lo = {
			.reg = readRegister(MT6835_REG_ABZ_RES2)
	};
    lo.abz_off = enabled?0:1;
    writeRegister(MT6835_REG_ABZ_RES2, lo.reg);
};



bool MT6835::isABSwapped(){
    MT6835ABZRes lo = {
			.reg = readRegister(MT6835_REG_ABZ_RES2)
	};
    return lo.ab_swap==1;
};
void MT6835::setABSwapped(bool swapped){
    MT6835ABZRes lo = {
			.reg = readRegister(MT6835_REG_ABZ_RES2)
	};
    lo.ab_swap = swapped?1:0;
    writeRegister(MT6835_REG_ABZ_RES2, lo.reg);
};



uint16_t MT6835::getZeroPosition(){
    uint8_t hi = readRegister(MT6835_REG_ZERO1);
    MT6835Options0 lo = {
            .reg = readRegister(MT6835_REG_ZERO2)
    };
    return (hi << 4) | lo.zero_pos_low;
};
void MT6835::setZeroPosition(uint16_t pos){
    uint8_t hi = (pos >> 4);
    MT6835Options0 lo = {
            .reg = readRegister(MT6835_REG_ZERO2)
    };
    lo.zero_pos_low = pos & 0x0F;
    writeRegister(MT6835_REG_ZERO1, hi);
    writeRegister(MT6835_REG_ZERO2, lo.reg);
};



MT6835Options1 MT6835::getOptions1(){
    MT6835Options1 result = {
			.reg = readRegister(MT6835_REG_OPTS1)
	};
    return result;
};
void MT6835::setOptions1(MT6835Options1 opts){
    writeRegister(MT6835_REG_OPTS1, opts.reg);
};



MT6835Options2 MT6835::getOptions2(){
    MT6835Options2 result = {
			.reg = readRegister(MT6835_REG_OPTS2)
	};
    return result;
};
void MT6835::setOptions2(MT6835Options2 opts){
    MT6835Options2 val = getOptions2();
    val.nlc_en = opts.nlc_en;
    val.pwm_fq = opts.pwm_fq;
    val.pwm_pol = opts.pwm_pol;
    val.pwm_sel = opts.pwm_sel;
    writeRegister(MT6835_REG_OPTS2, val.reg);
};



MT6835Options3 MT6835::getOptions3(){
    MT6835Options3 result = {
			.reg = readRegister(MT6835_REG_OPTS3)
	};
    return result;    
};
void MT6835::setOptions3(MT6835Options3 opts){
    MT6835Options3 val = getOptions3();
    val.rot_dir = opts.rot_dir;
    val.hyst = opts.hyst;
    writeRegister(MT6835_REG_OPTS3, val.reg);
};



MT6835Options4 MT6835::getOptions4(){
    MT6835Options4 result = {
			.reg = readRegister(MT6835_REG_OPTS4)
	};
    return result;
};
void MT6835::setOptions4(MT6835Options4 opts){
    MT6835Options4 val = getOptions4();
    val.gpio_ds = opts.gpio_ds;
    val.autocal_freq = opts.autocal_freq;
    writeRegister(MT6835_REG_OPTS4, val.reg);
};



uint32_t swap_bytes(uint32_t net)
{
    return __builtin_bswap32(net);
}





void MT6835::transfer24(MT6835Command* outValue) {
    uint32_t buff = swap_bytes(outValue->val);
    spi->beginTransaction(settings);
    if (nCS >= 0)
        digitalWrite(nCS, LOW);
    spi->transfer(&buff, 3);
    if (nCS >= 0)
        digitalWrite(nCS, HIGH);
    spi->endTransaction();
    outValue->val = swap_bytes(buff);
};
uint8_t MT6835::readRegister(uint16_t reg) {
    MT6835Command cmd;
    cmd.cmd = MT6835_OP_READ;
    cmd.addr = reg;
    transfer24(&cmd);
    return cmd.data;
};
bool MT6835::writeRegister(uint16_t reg, uint8_t value) {
    MT6835Command cmd;
    cmd.cmd = MT6835_OP_WRITE;
    cmd.addr = reg;
    cmd.data = value;
    transfer24(&cmd);
    return cmd.data == MT6835_WRITE_ACK;
};