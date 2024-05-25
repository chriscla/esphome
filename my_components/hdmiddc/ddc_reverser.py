import struct

GET_VCP_HEADER_LENGTH = 2  # header packet length
PROTOCOL_FLAG = 0x80  # protocol flag is bit 7 of the length byte

# VCP commands
GET_VCP_CMD = 0x01  # get VCP feature command
GET_VCP_REPLY = 0x02  # get VCP feature reply code
SET_VCP_CMD = 0x03  # set VCP feature command
GET_VCP_CAPS_CMD = 0xF3  # Capabilities Request command
GET_VCP_CAPS_REPLY = 0xE3  # Capabilities Request reply

# timeouts
GET_VCP_TIMEOUT = 0.04  # at least 40ms per the DDCCI specification
CMD_RATE = 0.05  # at least 50ms between messages

# addresses
DDCCI_ADDR = 0x37  # DDC-CI command address on the I2C bus
HOST_ADDRESS = 0x51  # virtual I2C slave address of the host
I2C_SLAVE = 0x0703  # I2C bus slave address

GET_VCP_RESULT_CODES = {
    0: "No Error",
    1: "Unsupported VCP code",
}

@staticmethod
def get_checksum(data: bytearray) -> int:
    """
    Computes the checksum for a set of data, with the option to
    use the virtual host address (per the DDC-CI specification).

    Args:
        data: Data array to transmit.

    Returns:
        Checksum for the data.
    """
    checksum = 0x00
    for data_byte in data:
        checksum ^= data_byte
    return checksum

@staticmethod
def set_vcp_feature(code: int, value: int):
    """
    Sets the value of a feature on the virtual control panel.

    Args:
        code: feature code
        value: feature value

    Raises:
        VCPIOError: failed to set VCP feature
    """

    # transmission data
    data = bytearray()
    data.append(SET_VCP_CMD)
    data.append(code)
    low_byte, high_byte = struct.pack("H", value)
    data.append(high_byte)
    data.append(low_byte)

    # add headers and footers
    data.insert(0, (len(data) | PROTOCOL_FLAG))
    data.insert(0, HOST_ADDRESS)
    data.append(get_checksum(bytearray([DDCCI_ADDR << 1]) + data))

    # write data
    print("data=" + " ".join([f"{x:02X}" for x in data]))

@staticmethod
def get_vcp_feature(code: int):
    """
    Gets the value of a feature from the virtual control panel.

    Args:
        code: Feature code.

    Returns:
        Current feature value, maximum feature value.

    Raises:
        VCPIOError: Failed to get VCP feature.
    """
    # transmission data
    data = bytearray()
    data.append(GET_VCP_CMD)
    data.append(code)

    # add headers and footers
    data.insert(0, (len(data) | PROTOCOL_FLAG))
    data.insert(0, HOST_ADDRESS)
    data.append(get_checksum(bytearray([DDCCI_ADDR << 1]) + data))

    # write data
    print("data=" + " ".join([f"{x:02X}" for x in data]))

    