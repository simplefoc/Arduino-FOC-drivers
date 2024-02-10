

#pragma once


#include "../SimpleFOCRegisters.h"


#if !defined(PACKETCOMMANDER_MAX_MOTORS)
#define PACKETCOMMANDER_MAX_MOTORS 4
#endif



class PacketCommander {
public:
    PacketCommander(bool echo = false);
    virtual ~PacketCommander();

    void addMotor(FOCMotor* motor);

    virtual void init(PacketIO& io);
    virtual void run();

    bool echo = true;
protected:

    virtual bool commsToRegister(uint8_t reg);
    virtual bool registerToComms(uint8_t reg);
    virtual void handleRegisterPacket(bool write, uint8_t reg);
    virtual bool handlePacket(Packet& packet);

    FOCMotor* motors[PACKETCOMMANDER_MAX_MOTORS];
    uint8_t numMotors = 0;

    uint8_t curMotor = 0;
    uint8_t curRegister = REG_STATUS;

    bool commanderror = false;
    bool lastcommanderror = false;
    uint8_t lastcommandregister = REG_STATUS;

    PacketIO* _io;
    Packet curr_packet = Packet(0x00, 0x00);
};
