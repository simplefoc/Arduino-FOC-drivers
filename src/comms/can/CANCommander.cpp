#if has_SimpleCANio
#include "CANCommander.h"

CANCommander::CANCommander(HardwareCAN& can, uint8_t addr, bool echo_enabled, int baudrate, bool no_filter) 
    : _can(&can), address(addr), echo(echo_enabled), baudrate(baudrate), no_filter(no_filter) {
}

CANCommander::~CANCommander() {
}

void CANCommander::init() {
    
    if(no_filter) {
        // Accept all messages (promiscuous mode)
        _can->filter(CanFilter(MASK_ACCEPT_ALL));
    }else{
        // Filter to accept only our address (broadcast filtering handled in software)
        // Mask the address bits [28:21] - the most significant 8 bits
        uint32_t address_mask = 0xFF << CAN_ADDRESS_SHIFT;  // Mask bits [28:21] (8 bits for address)
        uint32_t our_address_filter = (uint32_t)address << CAN_ADDRESS_SHIFT;
        
        // Use MASK_EXTENDED for 29-bit extended CAN IDs
        // This will only accept messages where the address bits match
        _can->filter(CanFilter(MASK_EXTENDED, our_address_filter, address_mask, FILTER_ANY_FRAME));
    }
    _can->begin(baudrate); // 1 Mbps CAN speed
}

void CANCommander::addMotor(FOCMotor* motor) {
    if (numMotors < CANCOMMANDER_MAX_MOTORS)
        motors[numMotors++] = motor;
}

void CANCommander::run() {
    // Process incoming CAN messages
    if (_can->available() > 0) {
        CanMsg msg = _can->read();
        if (!msg.isEmpty()) {
            handleCANMessage(msg);
        }
    }
}

void CANCommander::handleCANMessage(CanMsg& msg) {
    // Parse CAN ID to extract fields (address is now in MSBs)
    uint8_t msg_addr = (msg.id >> CAN_ADDRESS_SHIFT) & 0xFF;
    uint8_t packet_type = (msg.id >> CAN_PACKET_TYPE_SHIFT) & 0xF;
    uint8_t reg = (msg.id >> CAN_REGISTER_SHIFT) & 0xFF;
    uint8_t motor_idx = (msg.id >> CAN_MOTOR_INDEX_SHIFT) & 0xF;

    // Check if message is for us (hardware filter should handle this, but double-check)
    if (msg_addr != address && msg_addr != 0xFF) // 0xFF = broadcast
        return;

    lastcommanderror = commanderror;
    lastcommandregister = curRegister;
    commanderror = false;
    curRegister = reg;
    
    // Use motor index from CAN ID if provided, otherwise keep current
    if (motor_idx < numMotors) {
        curMotor = motor_idx;
    }

    // Copy data to rx_buffer
    rx_available = msg.data_length;
    rx_pos = 0;
    memcpy(rx_buffer, msg.data, msg.data_length);

    switch (packet_type) { 
        case CAN_READ_REQUEST:
            // Read register and send response
            sendRegisterResponse(reg, msg_addr);
            break;
            
        case CAN_WRITE_REQUEST:
            // Write to register
            if (commsToRegister(reg)) {
                if (echo) {
                    // Echo back the value
                    sendRegisterResponse(reg, msg_addr);
                }
            } else {
                commanderror = true;
            }
            break;
            
        case CAN_SYNC:
            // Sync request - respond with sync ack
            tx_pos = 0;
            tx_buffer[tx_pos++] = 0x01; // Sync ack
            uint32_t sync_id = (address << CAN_ADDRESS_SHIFT) | 
                              (CAN_SYNC << CAN_PACKET_TYPE_SHIFT);
            CanMsg sync_msg = CanMsg(CanExtendedId(sync_id), tx_pos, tx_buffer);
            _can->write(sync_msg);
            break;
    }
}

void CANCommander::sendRegisterResponse(uint8_t reg, uint8_t dest_addr) {
    // Prepare transmit buffer
    tx_pos = 0;
    
    // Write register data to buffer
    if (registerToComms(reg)) {
        // Build CAN ID for response (address in MSBs)
        uint32_t can_id = (address << CAN_ADDRESS_SHIFT) | 
                         (CAN_READ_RESPONSE << CAN_PACKET_TYPE_SHIFT) |
                         (reg << CAN_REGISTER_SHIFT) |
                         (curMotor << CAN_MOTOR_INDEX_SHIFT);
        
        // Send response
        CanMsg msg = CanMsg(CanExtendedId(can_id), tx_pos, tx_buffer);
        _can->write(msg);
    }
}

bool CANCommander::commsToRegister(uint8_t reg) {
  return SimpleFOCRegisters::commsToRegister(*this, reg, motors[curMotor]);
}

bool CANCommander::registerToComms(uint8_t reg) {
    switch (reg) {
        case SimpleFOCRegister::REG_STATUS:
            *this << (uint8_t)curMotor;
            *this << (uint8_t)lastcommandregister;
            *this << (uint8_t)(lastcommanderror ? 1 : 0);
            if (curMotor < numMotors)
                *this << (uint8_t)motors[curMotor]->motor_status;
            return true;
        case SimpleFOCRegister::REG_NUM_MOTORS:
            *this << numMotors;
            return true;
        default:
            if (curMotor >= numMotors) {
                commanderror = true;
                return false;
            }
            return SimpleFOCRegisters::registerToComms(*this, reg, motors[curMotor]);
    }
}

// RegisterIO interface implementation
RegisterIO& CANCommander::operator<<(uint8_t value) {
    if (tx_pos < 8) {
        tx_buffer[tx_pos++] = value;
    }
    return *this;
}

RegisterIO& CANCommander::operator<<(uint32_t value) {
    if (tx_pos + 4 <= 8) {
        memcpy(&tx_buffer[tx_pos], &value, 4);
        tx_pos += 4;
    }
    return *this;
}

RegisterIO& CANCommander::operator<<(float value) {
    if (tx_pos + 4 <= 8) {
        memcpy(&tx_buffer[tx_pos], &value, 4);
        tx_pos += 4;
    }
    return *this;
}

RegisterIO& CANCommander::operator>>(uint8_t& value) {
    if (rx_pos < rx_available) {
        value = rx_buffer[rx_pos++];
    } else {
        commanderror = true;
    }
    return *this;
}

RegisterIO& CANCommander::operator>>(uint32_t& value) {
    if (rx_pos + 4 <= rx_available) {
        memcpy(&value, &rx_buffer[rx_pos], 4);
        rx_pos += 4;
    } else {
        commanderror = true;
    }
    return *this;
}

RegisterIO& CANCommander::operator>>(float& value) {
    if (rx_pos + 4 <= rx_available) {
        memcpy(&value, &rx_buffer[rx_pos], 4);
        rx_pos += 4;
    } else {
        commanderror = true;
    }
    return *this;
}

#endif