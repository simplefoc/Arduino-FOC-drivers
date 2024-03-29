
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



uint32_t TextIO::intFromBuffer() {
    if (in_sep) {
        buffer_index++; // discard the separator
    }
    if (buffer_index >= buffer_len)
        in_sync = false;
    uint32_t value = 0;
    while (buffer_index < buffer_len) {
        char c = buffer[buffer_index];
        if (c >= '0' && c <= '9') {
            value = value * 10 + (c - '0');
            buffer_index++;
        }
        else {
            break;
        }
    }
    return value;
};


TextIO& TextIO::operator>>(float &value) {
    if (in_sep) {
        buffer_index++; // discard the separator
        in_sep = false;
    }
    if (buffer_index >= buffer_len) {
        in_sync = false;
        return *this;
    }
    char c = buffer[buffer_index];
    int8_t sign = 1;
    if (c == '-') {
        buffer_index++;
        sign = -1;
    }
    uint32_t val = 0;
    if (c != '.')
        val = intFromBuffer();
    if (buffer_index < buffer_len) {
        c = buffer[buffer_index];
        if (c == '.') {
            uint8_t pos = ++buffer_index;
            uint32_t frac = intFromBuffer();
            if (pos < buffer_index) {
                value = ((float)val + (float)frac / pow(10, buffer_index - pos)) * sign;
                in_sep = true;
            }
            else
                in_sync = false;
            return *this;
        }
    }
    value = (float)val * sign;
    in_sep = true;
    return *this;
};



TextIO& TextIO::operator>>(uint32_t &value) {
    value = intFromBuffer();
    in_sep = true;
    return *this;
};



TextIO& TextIO::operator>>(uint8_t &value) {
    value = (uint8_t)intFromBuffer();
    in_sep = true;
    return *this;
};



TextIO& TextIO::operator>>(Packet &value) {
    while (!in_sync && _io.available() > 0) {
        char skip = _io.read();
        if (skip == '\n' || skip == '\r') {
            in_sync = true;
            buffer_len = 0;
        }
    }
    if (buffer_index >= buffer_len) {
        buffer_len = 0;
        buffer_index = 0;
    }
    int avail = _io.available();
    while (in_sync && avail>0) {
        uint8_t peek = _io.peek();
        if (peek == '\n' || peek == '\r') {            
            // skip newlines and carriage returns
            while (avail>0 && (peek == '\n' || peek == '\r')) {
                _io.read(); // discard the \n
                avail = _io.available();
                if (avail>0)
                    peek = _io.peek();
            }
            if (buffer_len>1) {
                value.type = buffer[0];
                value.payload_size = 0;
                in_sep = false;
                buffer_index = 1;
                return *this;
            }
            else
                buffer_len = 0;
        }
        else {
            if (buffer_len < SIMPLEFOC_TEXTIO_BUFFER_SIZE) {
                buffer[buffer_len++] = _io.read();
                avail = _io.available();
            }
            else {
                buffer_len = 0;
                in_sync = false;
            }
        }
    }

    value.type = 0x00;
    value.payload_size = 0;
    return *this;
};



bool TextIO::is_complete() {
    return  buffer_index >= buffer_len;
};