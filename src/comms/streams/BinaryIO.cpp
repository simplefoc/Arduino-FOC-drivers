

#include "./BinaryIO.h"

#include <string.h>

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
    uint8_t buf[4] = {0};
    uint8_t read = 0;
    _rx_fill();
    while (read < 4 && _rx_available() > 0) {
        int byte = _rx_read();
        if (byte < 0) {
            break;
        }
        buf[read++] = (uint8_t)byte;
    }
    memcpy(&value, buf, sizeof(buf));
    if (read > 0) {
        remaining -= read;
    }
    return *this;
};



BinaryIO& BinaryIO::operator>>(uint32_t &value) {
    uint8_t buf[4] = {0};
    uint8_t read = 0;
    _rx_fill();
    while (read < 4 && _rx_available() > 0) {
        int byte = _rx_read();
        if (byte < 0) {
            break;
        }
        buf[read++] = (uint8_t)byte;
    }
    memcpy(&value, buf, sizeof(buf));
    if (read > 0) {
        remaining -= read;
    }
    return *this;
};



BinaryIO& BinaryIO::operator>>(uint8_t &value) {
    _rx_fill();
    int byte = _rx_read();
    if (byte >= 0) {
        value = (uint8_t)byte;
        remaining--;
    } else {
        value = 0;
    }
    return *this;
};



PacketIO& BinaryIO::operator>>(Packet& value) {
    _rx_fill();
    if (_pending) {
        if (_rx_available() < _pending_payload) {
            value.type = 0x00;
            value.payload_size = 0;
            return *this;
        }
        value.type = _pending_type;
        value.payload_size = _pending_payload;
        remaining = value.payload_size;
        _pending = false;
        return *this;
    }
    // Always resync to marker, even if in_sync was true.
    while (_rx_available() > 0 && _rx_peek() != MARKER_BYTE) {
        in_sync = false;
        _rx_read();
    }
    if (_rx_available() == 0) {
        value.type = 0x00;
        value.payload_size = 0;
        return *this;
    }
    in_sync = true;
    if (_rx_available() < 3) {  // size, frame type, payload = 3 bytes minimum frame size
        value.type = 0x00;
        value.payload_size = 0;
        return *this;
    }
    _rx_read(); // discard the marker
    int size_byte = _rx_read();
    int type_byte = _rx_read();
    if (size_byte <= 0) {
        in_sync = false;
        value.type = 0x00;
        value.payload_size = 0;
        return *this;
    }
    uint8_t payload_size = (uint8_t)(size_byte - 1);
    if (payload_size >= BINARYIO_RX_BUFFER_SIZE) {
        // Payload cannot fit; drop out of sync and wait for next marker.
        in_sync = false;
        value.type = 0x00;
        value.payload_size = 0;
        return *this;
    }
    if (_rx_available() < payload_size) {
        _pending = true;
        _pending_type = (uint8_t)type_byte;
        _pending_payload = payload_size;
        value.type = 0x00;
        value.payload_size = 0;
        return *this;
    }
    value.type = (uint8_t)type_byte;
    value.payload_size = payload_size;
    remaining = value.payload_size;
    return *this;
};



bool BinaryIO::is_complete() {
    return (remaining <= 0);
};

void BinaryIO::_rx_fill() {
    while (_io.available() > 0 && _rx_count < BINARYIO_RX_BUFFER_SIZE) {
        int byte = _io.read();
        if (byte < 0) {
            break;
        }
        _rx_buffer[_rx_head] = (uint8_t)byte;
        _rx_head = (uint8_t)((_rx_head + 1) % BINARYIO_RX_BUFFER_SIZE);
        _rx_count++;
    }
}

uint8_t BinaryIO::_rx_available() const {
    return _rx_count;
}

int BinaryIO::_rx_peek() {
    if (_rx_count == 0) {
        return -1;
    }
    return _rx_buffer[_rx_tail];
}

int BinaryIO::_rx_read() {
    if (_rx_count == 0) {
        return -1;
    }
    uint8_t value = _rx_buffer[_rx_tail];
    _rx_tail = (uint8_t)((_rx_tail + 1) % BINARYIO_RX_BUFFER_SIZE);
    _rx_count--;
    return value;
}
