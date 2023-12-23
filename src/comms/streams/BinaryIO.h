

#pragma once

#include "../RegisterIO.h"
#include <Stream.h>


#define MARKER_BYTE 0xA5

// standard buffer size is 64 bytes, to match the FS USB packet transfer size
// the BinaryIO will send the data in chunks of this size, which should greatly
// improve the performance compared to the ASCII version, which will do a lot of
// converting data to ASCII, and then write in many small writes.
#ifndef BINARYIO_BUFFER_SIZE
#define BINARYIO_BUFFER_SIZE 58
#endif

class BinaryIO : public PacketIO {
    public:
        BinaryIO(Stream& io);
        virtual ~BinaryIO();
        BinaryIO& operator<<(float value) override;
        BinaryIO& operator<<(uint32_t value) override;
        BinaryIO& operator<<(uint8_t value) override;
        BinaryIO& operator<<(char value) override;
        BinaryIO& operator<<(Packet value) override;
        BinaryIO& operator<<(Separator value) override;
        BinaryIO& operator>>(float& value) override;
        BinaryIO& operator>>(uint32_t& value) override;
        BinaryIO& operator>>(uint8_t& value) override;
        PacketIO& operator>>(Packet& value) override;
        bool is_complete() override;
    protected:
        void _buff(uint8_t* data, uint8_t size);
        void _buff(uint8_t data);
        void _flush();
        Stream& _io;
        uint8_t remaining = 0;
        uint8_t _pos = 0;
        uint8_t _buffer[BINARYIO_BUFFER_SIZE];
};




