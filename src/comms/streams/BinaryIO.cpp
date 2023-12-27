

#include "./BinaryIO.h"

BinaryIO::BinaryIO(Stream& io) : _io(io) {
    // nothing to do here
};

BinaryIO::~BinaryIO(){
    // nothing to do here
};


BinaryIO& BinaryIO::operator<<(float value) {
    _buff((uint8_t*)&value, 4);
    return *this;
};



BinaryIO& BinaryIO::operator<<(uint32_t value) {
    _buff((uint8_t*)&value, 4);
    return *this;
};



BinaryIO& BinaryIO::operator<<(uint8_t value) {
    _buff(value);
    return *this;
};



BinaryIO& BinaryIO::operator<<(char value) {
    _buff((uint8_t)value);
    return *this;
};



BinaryIO& BinaryIO::operator<<(Packet value) {
    if (value.type!=0x00) {
        _buff(MARKER_BYTE);
        _buff(value.payload_size+1);
        _buff(value.type);
    }
    return *this;
};



BinaryIO& BinaryIO::operator<<(Separator value) {
    // separator is ignored in binary mode
    return *this;
};





void BinaryIO::_buff(uint8_t* data, uint8_t size) {
    for (uint8_t i=0; i<size; i++) {
        if (_pos >= BINARYIO_BUFFER_SIZE) {
            _flush();
        }
        _buffer[_pos++] = data[i];
    }
};


void BinaryIO::_buff(uint8_t data) {
    if (_pos >= BINARYIO_BUFFER_SIZE) {
        _flush();
    }
    _buffer[_pos++] = data;
};



void BinaryIO::_flush() {
    if (_pos>0) {
        _io.write(_buffer, _pos);
        _pos = 0;
    }
};






BinaryIO& BinaryIO::operator>>(float &value) {
    remaining -= _io.readBytes((uint8_t*)&value, 4);
    return *this;
};



BinaryIO& BinaryIO::operator>>(uint32_t &value) {
    remaining -= _io.readBytes((uint8_t*)&value, 4);
    return *this;
};



BinaryIO& BinaryIO::operator>>(uint8_t &value) {
    value = (uint8_t)_io.read();
    remaining--;
    return *this;
};



PacketIO& BinaryIO::operator>>(Packet& value) {
    while (!in_sync && _io.available() > 0) {
        if (_io.peek() == MARKER_BYTE)
            in_sync = true;
        else
            _io.read();
    }
    if (_io.peek() == MARKER_BYTE) {
        _io.read(); // discard the marker
    }
    if (!in_sync || _io.available() < 3) {  // size, frame type, payload = 3 bytes minimum frame size
        value.type = 0x00;
        value.payload_size = 0;
        return *this;
    }
    value.payload_size = (uint8_t)_io.read() - 1;
    value.type = (uint8_t)_io.read();
    remaining = value.payload_size;
    return *this;
};



bool BinaryIO::is_complete() {
    return (remaining <= 0);
};

