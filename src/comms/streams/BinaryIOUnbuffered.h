// #pragma once

// #include "../RegisterIO.h"
// #include <Stream.h>
// #include "Arduino.h"


// #define MARKER_BYTE 0xA5

// // Unbuffered Binary IO - writes data immediately without buffering
// // Useful for real-time applications where latency is critical
// // Trade-off: More serial writes (potentially slower overall) but lower latency

// class BinaryIOUnbuffered : public PacketIO {
//     public:
//         BinaryIOUnbuffered(Stream& io);
//         virtual ~BinaryIOUnbuffered();
//         BinaryIOUnbuffered& operator<<(float value) override;
//         BinaryIOUnbuffered& operator<<(uint32_t value) override;
//         BinaryIOUnbuffered& operator<<(uint8_t value) override;
//         BinaryIOUnbuffered& operator<<(char value) override;
//         BinaryIOUnbuffered& operator<<(Packet value) override;
//         BinaryIOUnbuffered& operator<<(Separator value) override;
//         BinaryIOUnbuffered& operator>>(float& value) override;
//         BinaryIOUnbuffered& operator>>(uint32_t& value) override;
//         BinaryIOUnbuffered& operator>>(uint8_t& value) override;
//         PacketIO& operator>>(Packet& value) override;
//         bool is_complete() override;
//         virtual void _flush() override;

//     protected:
//         Stream& _io;
//         uint8_t remaining = 0;
// };
