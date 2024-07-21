
#include "PacketCommander.h"

PacketCommander::PacketCommander(bool echo) : echo(echo) {
    // nothing to do
};



PacketCommander::~PacketCommander() {
    // nothing to do
};




void PacketCommander::addMotor(FOCMotor* motor) {
    if (numMotors < PACKETCOMMANDER_MAX_MOTORS)
        motors[numMotors++] = motor;
};




void PacketCommander::init(PacketIO& io) {
    _io = &io;
};



void PacketCommander::run() {
    // is a packet available?
    *_io >> curr_packet;
    if (curr_packet.type != 0x00) {
        // new packet arrived - handke ut
        commanderror = false;
        if (!handlePacket(curr_packet)) {
            // TODO error handling
        };
        lastcommanderror = commanderror;
        lastcommandregister = curRegister;
        if (commanderror) {
            _io->in_sync = false; // TODO flush input buffer, don't set out of sync
            //*_io << START_PACKET(PacketType::ALERT, 1) << '?' << END_PACKET;
        }        
        if (! _io->is_complete())
            _io->in_sync = false; // TODO flush input buffer
    }
};



bool PacketCommander::handlePacket(Packet& packet) {
    if (packet.type == PacketType::REGISTER) {
        *_io >> curRegister;
        handleRegisterPacket(!_io->is_complete(), curRegister);
        return true;
    }
    else if (packet.type == PacketType::SYNC) {            
        *_io << START_PACKET(PacketType::SYNC, 1);
        *_io << (uint8_t)0x01;
        *_io << END_PACKET;
        // TODO flush output packet
        return true;
    }
    return false;
};



void PacketCommander::handleRegisterPacket(bool write, uint8_t reg) {
    if (write) {
        bool ok = commsToRegister(reg);
        commanderror = commanderror || !ok;
    }
    if (!write || echo) {
        uint8_t size = SimpleFOCRegisters::regs->sizeOfRegister(reg);
        if (size > 0) { // sendable register
            *_io << START_PACKET(PacketType::RESPONSE, size+1) << reg << Separator('=');
            // TODO status?
            registerToComms(reg);
            *_io << END_PACKET;
            // TODO flush packet
        }
    }
};



bool PacketCommander::commsToRegister(uint8_t reg){
    switch (reg) {
        case SimpleFOCRegister::REG_MOTOR_ADDRESS:
            uint8_t val;
            *_io >> val;
            if (val >= numMotors)
                commanderror = true;
            else
                curMotor = val;
            return true;
        default:
            return SimpleFOCRegisters::regs->commsToRegister(*_io, reg, motors[curMotor]);
    }
};



bool PacketCommander::registerToComms(uint8_t reg){
    switch (reg) {
        case SimpleFOCRegister::REG_STATUS:
            // TODO implement status register
            return true;
        case SimpleFOCRegister::REG_MOTOR_ADDRESS:
            *_io << curMotor;
            return true;
        case SimpleFOCRegister::REG_NUM_MOTORS:
            *_io << numMotors;
            return true;
        default:
            return SimpleFOCRegisters::regs->registerToComms(*_io, reg, motors[curMotor]);
    }
};
