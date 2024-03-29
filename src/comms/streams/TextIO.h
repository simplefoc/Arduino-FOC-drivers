

#pragma once

#include "../RegisterIO.h"
#include <Stream.h>


#ifndef SIMPLEFOC_TEXTIO_BUFFER_SIZE
#define SIMPLEFOC_TEXTIO_BUFFER_SIZE 64
#endif



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
        uint32_t intFromBuffer();

        Stream& _io;
        bool sep = false;
        bool in_sep = false;
        uint8_t buffer_index = 0;
        uint8_t buffer_len = 0;
        char buffer[SIMPLEFOC_TEXTIO_BUFFER_SIZE];
};




