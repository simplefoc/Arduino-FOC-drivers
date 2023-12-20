

#pragma once

#include "../RegisterIO.h"
#include <Stream.h>


#define MARKER_BYTE 0xA5


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
        bool is_complete() override;
    protected:
        Stream& _io;
};




