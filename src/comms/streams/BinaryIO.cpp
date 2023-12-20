

#include "./BinaryIO.h"

BinaryIO::BinaryIO(Stream& io) : _io(io) {
    // nothing to do here
};

BinaryIO::~BinaryIO(){
    // nothing to do here
};


BinaryIO& BinaryIO::operator<<(float value) {
    _io.write(value);
    return *this;
};



BinaryIO& BinaryIO::operator<<(uint32_t value) {
    _io.print(value);
    return *this;
};



BinaryIO& BinaryIO::operator<<(uint8_t value) {
    _io.write(value);
    return *this;
};



BinaryIO& BinaryIO::operator<<(char value) {
    _io.write((uint8_t)value);
    return *this;
};



BinaryIO& BinaryIO::operator<<(Packet value) {
    if (value.type!=0x00) {
        _io.write(MARKER_BYTE);
        _io.write(value.payload_size+1);
        _io.write(value.type);
    }
    return *this;
};



BinaryIO& BinaryIO::operator<<(Separator value) {
    // separator is ignored in binary mode
    return *this;
};



BinaryIO& BinaryIO::operator>>(float &value) {
    _io.readBytes((uint8_t*)&value, 4);
    return *this;
};



BinaryIO& BinaryIO::operator>>(uint32_t &value) {
    _io.readBytes((uint8_t*)&value, 4);
    return *this;
};



BinaryIO& BinaryIO::operator>>(uint8_t &value) {
    value = (uint8_t)_io.read();
    return *this;
};


bool BinaryIO::is_complete() {
    //return remaining <= 0; // TODO write me!
    return false;
};
