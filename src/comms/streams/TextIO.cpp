
#include "./TextIO.h"
#include "Arduino.h"

TextIO::TextIO(Stream& io) : _io(io) {
    // nothing to do here
};

TextIO::~TextIO(){
    // nothing to do here
};


TextIO& TextIO::operator<<(float value) {
    if (sep) {
        _io.print(",");
    }
    _io.print(value, precision);
    sep = true;
    return *this;
};



TextIO& TextIO::operator<<(uint32_t value) {
    if (sep) {
        _io.print(",");
    }
    _io.print(value);
    sep = true;
    return *this;
};



TextIO& TextIO::operator<<(uint8_t value) {
    if (sep) {
        _io.print(",");
    }
    _io.print(value);
    sep = true;
    return *this;
};



TextIO& TextIO::operator<<(char value) {
    _io.print(value);
    sep = false;
    return *this;
};



TextIO& TextIO::operator<<(Packet value) {
    if (value.type==0x00)
        _io.println();
    else {
        _io.print((char)value.type);
    }
    sep = false;
    return *this;
};



TextIO& TextIO::operator<<(Separator value) {
    _io.print(value.value);
    sep = false;
    return *this;
};

#ifndef ESP32
#define LOOKAHEADARGS LookaheadMode::SKIP_NONE
#else
#define LOOKAHEADARGS
#endif

TextIO& TextIO::operator>>(float &value) {
    if (_io.peek() == '\n') {
        return *this; // TODO flag this error
    }
    if (in_sep) {
        _io.read(); // discard the separator
    }
    value = _io.parseFloat(LOOKAHEADARGS);  // TODO LookaheadMode is not defined on ESP32
    in_sep = true;
    return *this;
};

TextIO& TextIO::operator>>(uint32_t &value) {
    if (_io.peek() == '\n') {
        return *this; // TODO flag this error
    }
    if (in_sep) {
        _io.read(); // discard the separator
    }
    value = (uint32_t)_io.parseInt(LOOKAHEADARGS);
    in_sep = true;
    return *this;
};



TextIO& TextIO::operator>>(uint8_t &value) {
    if (_io.peek() == '\n') {
        return *this; // TODO flag this error
    }
    if (in_sep) {
        _io.read(); // discard the separator
    }
    value = (uint8_t)_io.parseInt(LOOKAHEADARGS);
    in_sep = true;
    return *this;
};



TextIO& TextIO::operator>>(Packet &value) {
    while (!in_sync && _io.available() > 0) {
        if (_io.read() == '\n')
            in_sync = true;
    }
    if (_io.peek() == '\n') {
        _io.read(); // discard the \n
    }
    if (!in_sync || _io.available() < 3) {  // frame type, register id, \n to end frame = 3 bytes minimum frame size
        value.type = 0x00;
        value.payload_size = 0;
        return *this;
    }
    value.type = _io.read();
    value.payload_size = 0;
    in_sep = false;
    return *this;
};



bool TextIO::is_complete() {
    return _io.peek() == '\n';
};