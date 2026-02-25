// #include "./BinaryIOUnbuffered.h"

// BinaryIOUnbuffered::BinaryIOUnbuffered(Stream& io) : _io(io) {
//     // nothing to do here
// };

// BinaryIOUnbuffered::~BinaryIOUnbuffered(){
//     // nothing to do here
// };


// BinaryIOUnbuffered& BinaryIOUnbuffered::operator<<(float value) {
//     _io.write((uint8_t*)&value, 4);
//     return *this;
// };


// BinaryIOUnbuffered& BinaryIOUnbuffered::operator<<(uint32_t value) {
//     _io.write((uint8_t*)&value, 4);
//     return *this;
// };


// BinaryIOUnbuffered& BinaryIOUnbuffered::operator<<(uint8_t value) {
//     _io.write(value);
//     return *this;
// };


// BinaryIOUnbuffered& BinaryIOUnbuffered::operator<<(char value) {
//     _io.write((uint8_t)value);
//     return *this;
// };


// BinaryIOUnbuffered& BinaryIOUnbuffered::operator<<(Packet value) {
//     if (value.type!=0x00) {
//         _io.write(MARKER_BYTE);
//         _io.write(value.payload_size+1);
//         _io.write(value.type);
//     }
//     return *this;
// };


// BinaryIOUnbuffered& BinaryIOUnbuffered::operator<<(Separator value) {
//     // separator is ignored in binary mode
//     return *this;
// };


// // Immediate flush - no buffering, so nothing to do
// void BinaryIOUnbuffered::_flush() {
//     // No internal buffer to flush
// };


// BinaryIOUnbuffered& BinaryIOUnbuffered::operator>>(float &value) {
//     remaining -= _io.readBytes((uint8_t*)&value, 4);
//     return *this;
// };


// BinaryIOUnbuffered& BinaryIOUnbuffered::operator>>(uint32_t &value) {
//     remaining -= _io.readBytes((uint8_t*)&value, 4);
//     return *this;
// };


// BinaryIOUnbuffered& BinaryIOUnbuffered::operator>>(uint8_t &value) {
//     value = (uint8_t)_io.read();
//     remaining--;
//     return *this;
// };


// PacketIO& BinaryIOUnbuffered::operator>>(Packet& value) {
//     while (!in_sync && _io.available() > 0) {
//         if (_io.peek() == MARKER_BYTE)
//             in_sync = true;
//         else
//             _io.read();
//     }
//     if (_io.peek() == MARKER_BYTE) {
//         _io.read(); // discard the marker
//     }
//     if (!in_sync || _io.available() < 3) {  // size, frame type, payload = 3 bytes minimum frame size
//         value.type = 0x00;
//         value.payload_size = 0;
//         return *this;
//     }
//     value.payload_size = (uint8_t)_io.read() - 1;
//     value.type = (uint8_t)_io.read();
//     remaining = value.payload_size;
//     return *this;
// };

// bool BinaryIOUnbuffered::is_complete() {
//     return (remaining <= 0);
// };
