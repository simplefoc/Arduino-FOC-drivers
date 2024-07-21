
#pragma once

#include <inttypes.h>


class RegisterIO {
    public:
        virtual ~RegisterIO() = default;
        virtual RegisterIO& operator<<(float value) = 0;
        virtual RegisterIO& operator<<(uint32_t value) = 0;
        virtual RegisterIO& operator<<(uint8_t value) = 0;
        virtual RegisterIO& operator>>(float& value) = 0;
        virtual RegisterIO& operator>>(uint32_t& value) = 0;
        virtual RegisterIO& operator>>(uint8_t& value) = 0;
};


class Packet {
public:
    Packet(uint8_t type, uint8_t size = 0) : type(type), payload_size(size) {};
    uint8_t type;
    uint8_t payload_size;
};


class Separator {
public:
    Separator(char value) : value(value) {};
    char value;
};


enum PacketType : uint8_t {
    TELEMETRY_HEADER = 'H',
    TELEMETRY = 'T',
    REGISTER = 'R',
    RESPONSE = 'r',
    ALERT = 'A',
    DEBUG = 'D',
    SYNC = 'S'
};


#define START_PACKET(type, size) Packet(type, size)
#define END_PACKET Packet(0x00,0x00)



class PacketIO : public RegisterIO {
    public:
        virtual ~PacketIO() = default;

        virtual PacketIO& operator<<(float value) = 0;
        virtual PacketIO& operator<<(uint32_t value) = 0;
        virtual PacketIO& operator<<(uint8_t value) = 0;
        virtual PacketIO& operator>>(float& value) = 0;
        virtual PacketIO& operator>>(uint32_t& value) = 0;
        virtual PacketIO& operator>>(uint8_t& value) = 0;

        virtual PacketIO& operator<<(char value) = 0;
        virtual PacketIO& operator<<(Separator value) = 0;
        virtual PacketIO& operator<<(Packet value) = 0;
        virtual PacketIO& operator>>(Packet& value) = 0;
        virtual bool is_complete() = 0;
    
        bool in_sync = true; // start in-sync?
};
