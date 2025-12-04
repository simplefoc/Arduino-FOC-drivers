
#include "./KTH7812.h"


KTH7812::KTH7812(SPISettings settings, int nCS, bool fastmode, bool withcrc) : settings(settings), nCS(nCS), fastmode(fastmode), checkcrc(withcrc) {
    // nix
}

KTH7812::~KTH7812() {
    // nix
}

void KTH7812::init(SPIClass* _spi) {
    spi = _spi;
    if (nCS >= 0) {
        pinMode(nCS, OUTPUT);
        digitalWrite(nCS, HIGH);
    }
    spi->begin();
    transfer16(0x0000); // dummy transfer to wake up
};



float KTH7812::getCurrentAngle(){
    int32_t rawangle = -1;
    if (checkcrc)
        rawangle = readRawAngle12WithCRC();
    else
        rawangle = readRawAngle16();
    if (rawangle < 0) // error
        return -1.0f;
    return rawangle / (float)KTH7812_CPR * _2PI;
};



uint16_t KTH7812::readRawAngle16(){
    uint16_t resp = transfer16(0x0000);
    if (!fastmode)
        resp = transfer16(0x0000);
    return resp;
};


int32_t KTH7812::readRawAngle12WithCRC(){
    uint16_t resp = transfer16(0x0000);
    if (!fastmode)
        resp = transfer16(0x0000);
    uint8_t crc = resp & 0x000F;
    // TODO check crc
    return resp & 0xFFF0;
};


uint8_t KTH7812::getGainTrim(){
    return readRegister(KTH7812_REG_GAIN);
};


void KTH7812::setGainTrim(uint8_t gain){
    writeRegister(KTH7812_REG_GAIN, gain);
};


KTH7812Trim KTH7812::getTrim(){
    KTH7812Trim trim;
    trim.reg = readRegister(KTH7812_REG_TRIM);
    return trim;
};


void KTH7812::setTrim(KTH7812Trim trim){
    KTH7812Trim val = getTrim();
    val.xtrim = trim.xtrim;
    val.ytrim = trim.ytrim;
    writeRegister(KTH7812_REG_TRIM, val.reg);
};


KTH7812_MGHL KTH7812::getMGHL(){
    KTH7812_MGHL mgh;
    mgh.reg = readRegister(KTH7812_REG_MGHL);
    return mgh;
};


void KTH7812::setMGHL(KTH7812_MGHL mgh){
    KTH7812_MGHL val = getMGHL();
    val.mgh = mgh.mgh;
    val.mgl = mgh.mgl;
    writeRegister(KTH7812_REG_MGHL, val.reg);
};


uint16_t KTH7812::getZero(){
    return readRegister(KTH7812_REG_ZLSB) | (readRegister(KTH7812_REG_ZMSB) << 8);
};


void KTH7812::setZero(uint16_t zero){
    writeRegister(KTH7812_REG_ZLSB, zero & 0xFF);
    writeRegister(KTH7812_REG_ZMSB, (zero >> 8) & 0xFF);
};


bool KTH7812::getDirection(){
    return (readRegister(KTH7812_REG_RD) & KTH7812_RD_BIT) != 0;
};


void KTH7812::setDirection(bool rd){
    uint8_t val = readRegister(KTH7812_REG_RD);
    if (rd)
        val |= KTH7812_RD_BIT;
    else
        val &= ~KTH7812_RD_BIT;
    writeRegister(KTH7812_REG_RD, val);
};


KTH7812_NPP KTH7812::getNPP(){
    KTH7812_NPP npp;
    npp.reg = readRegister(KTH7812_REG_NPP);
    return npp;
};


void KTH7812::setNPP(KTH7812_NPP npp){
    KTH7812_NPP val = getNPP();
    val.npp = npp.npp;
    writeRegister(KTH7812_REG_NPP, val.reg);    
};


uint16_t KTH7812::getPPT(){
    KTH7812_PPT0 ppt0;
    ppt0.reg = readRegister(KTH7812_REG_PPT0);
    uint16_t ppt = readRegister(KTH7812_REG_PPT1)<<2;
    ppt |= (ppt0.pptl & 0b11);
    return ppt;
};


void KTH7812::setPPT(uint16_t ppt){
    KTH7812_PPT0 ppt0;
    ppt0.reg = readRegister(KTH7812_REG_PPT0);
    ppt0.pptl = (ppt & 0b11);
    writeRegister(KTH7812_REG_PPT0, ppt0.reg);
    writeRegister(KTH7812_REG_PPT1, (ppt >> 2) & 0xFF);
};


uint8_t KTH7812::getABZLimit(){
    KTH7812_ABZL abz;
    abz.reg = readRegister(KTH7812_REG_ABZL);
    return abz.abzlimit;
};


void KTH7812::setABZLimit(uint8_t limit){
    KTH7812_ABZL abz;
    abz.reg = 0;
    abz.abzlimit = limit;
    writeRegister(KTH7812_REG_ABZL, abz.reg);
};


KTH7812_PPT0 KTH7812::getZlZd(){
    KTH7812_PPT0 zlzd;
    zlzd.reg = readRegister(KTH7812_REG_PPT0);
    return zlzd;
};


void KTH7812::setZlZd(KTH7812_PPT0 zlzd){
    KTH7812_PPT0 val = getZlZd();
    val.zl = zlzd.zl;
    val.zd = zlzd.zd;
    writeRegister(KTH7812_REG_PPT0, val.reg);
};


uint16_t KTH7812::transfer16(uint16_t val){
    uint16_t buff = val;
    spi->beginTransaction(settings);
    if(nCS >= 0)
        digitalWrite(nCS, LOW);
    buff = spi->transfer16(buff);
    if(nCS >= 0)
        digitalWrite(nCS, HIGH);
    spi->endTransaction();
    return buff;
};


uint8_t KTH7812::readRegister(uint8_t reg){
    uint16_t cmd = 0x8000 | ((reg&0x3F) << 8);
    uint16_t resp = transfer16(cmd);
    resp = transfer16(0x0000);
    // TODO status
    return resp & 0xFF;
};


uint8_t KTH7812::writeRegister(uint8_t reg, uint8_t value){
    uint16_t cmd = ((reg&0x3F) << 8) | (value & 0xFF);
    uint16_t resp = transfer16(cmd);
    delayMicroseconds(20100); // wait for write to complete
    resp = transfer16(0x0000);
    // TODO status
    return (resp & 0xFF); // TODO response value
};


uint8_t KTH7812::calcCrc(uint32_t angle, uint8_t status){
    // TODO write me
    return 0;
};

