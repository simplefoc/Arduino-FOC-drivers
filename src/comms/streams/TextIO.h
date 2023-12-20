

#pragma once

#include "../RegisterIO.h"
#include <Stream.h>


class TextIO : public PacketIO {
    public:
        TextIO(Stream& io);
        virtual ~TextIO();
        TextIO& operator<<(float value) override;
        TextIO& operator<<(uint32_t value) override;
        TextIO& operator<<(uint8_t value) override;
        TextIO& operator<<(char value) override;
        TextIO& operator<<(Packet value) override;
        TextIO& operator<<(Separator value) override;
        TextIO& operator>>(float& value) override;
        TextIO& operator>>(uint32_t& value) override;
        TextIO& operator>>(uint8_t& value) override;
        TextIO& operator>>(Packet &value) override;
        bool is_complete() override;
        
        uint8_t precision = 4;
    protected:
        Stream& _io;
        bool sep = false;
        bool in_sep = false;
};




