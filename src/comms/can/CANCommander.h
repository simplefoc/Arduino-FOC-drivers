#ifndef CANCOMMANDER_H 
#define CANCOMMANDER_H
#if __cplusplus >= 201703L //Check for C++17 or later
    #if __has_include("SimpleCANio.h") //C++17 required for __has_include
        #define has_SimpleCANio 1
    #endif
#endif

#ifdef has_SimpleCANio
#include "SimpleFOC.h"
#include "../SimpleFOCRegisters.h"
#include "../RegisterIO.h"
#include "SimpleCANio.h"

#if !defined(CANCOMMANDER_MAX_MOTORS)
#define CANCOMMANDER_MAX_MOTORS 16
#endif

// CAN packet structure for register-based protocol
// CAN ID bits allocation (Optimized for hardware filtering):
// [27:20] - Node Address (8 bits) - MOST SIGNIFICANT for efficient filtering
// [19:16] - Packet Type (4 bits) 
// [15:8]  - Register number (8 bits) - see SimpleFOCRegisters for the register numbers 
// [7:0]   - Motor Index (8 bits) - Selects motor 0-256 on the node

#define CAN_ADDRESS_SHIFT 20        // Address in MSBs for efficient filtering
#define CAN_PACKET_TYPE_SHIFT 16    // Packet type after address
#define CAN_REGISTER_SHIFT 8        // Register number
#define CAN_MOTOR_INDEX_SHIFT 0     // Motor selection (0-256)

enum CANPacketType : uint8_t {
    CAN_READ_REQUEST = 0x1,
    CAN_WRITE_REQUEST = 0x2,
    CAN_READ_RESPONSE = 0x3,
    CAN_SYNC = 0xF
};

class CANCommander : public RegisterIO
{
public:
    CANCommander(HardwareCAN& can, uint8_t address = 0, bool echo = false, int baudrate = 1000000, bool no_filter = false);
    virtual ~CANCommander();

    void addMotor(FOCMotor* motor);
    virtual void init();
    virtual void run();

    // RegisterIO interface implementation
    RegisterIO& operator<<(float value) override;
    RegisterIO& operator<<(uint32_t value) override;
    RegisterIO& operator<<(uint8_t value) override;
    RegisterIO& operator>>(float& value) override;
    RegisterIO& operator>>(uint32_t& value) override;
    RegisterIO& operator>>(uint8_t& value) override;

    bool echo = false;
    uint8_t address = 0;
    int baudrate = 1000000;
    bool no_filter = false;

    bool addCustomRegister(uint8_t reg, uint8_t size, 
        RegisterReadHandler readHandler, 
        RegisterWriteHandler writeHandler) {
        return SimpleFOCRegisters::regs->addCustomRegister(reg, size, readHandler, writeHandler);
    }

protected:
    virtual bool commsToRegister(uint8_t reg);
    virtual bool registerToComms(uint8_t reg);
    virtual void handleCANMessage(CanMsg& msg);
    virtual void sendRegisterResponse(uint8_t reg, uint8_t dest_addr);

    HardwareCAN* _can;
    FOCMotor* motors[CANCOMMANDER_MAX_MOTORS];
    uint8_t numMotors = 0;
    uint8_t curMotor = 0;
    uint8_t curRegister = REG_STATUS;
    
    bool commanderror = false;
    bool lastcommanderror = false;
    uint8_t lastcommandregister = REG_STATUS;

    // Transmit/receive buffers
    uint8_t tx_buffer[8];
    uint8_t tx_pos = 0;
    uint8_t rx_buffer[8];
    uint8_t rx_pos = 0;
    uint8_t rx_available = 0;
};

#endif
#endif