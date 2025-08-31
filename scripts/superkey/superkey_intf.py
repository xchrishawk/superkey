#
# @file     scripts/superkey/superkey_intf.py
# @brief    Module defining the SuperKeyInterface class.
#
# @author   Chris Vig (chris@invictus.so)
# @date     2025-08-29
# @cpyrt    © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
#

# ------------------------------------------------------ IMPORTS -------------------------------------------------------

import serial
import struct
from typing import Optional, Tuple

from .superkey_types import *

# ------------------------------------------------------ EXPORTS -------------------------------------------------------

__all__ = [
    "SuperKeyInterfaceError",
    "SuperKeyInterface"
]

# ------------------------------------------------------- TYPES --------------------------------------------------------

class SuperKeyInterfaceError(RuntimeError):
    """
    Error class thrown by `SuperKeyInterface`.
    """
    def __init__(self, message: str = "Operation failed."):
        super().__init__(message)


class SuperKeyInterface:

    def __init__(self, port: str = "COM1", baudrate: int = 19200, timeout: Optional[float] = 1.0):
        """
        Initializes a new instance for the specified port.
        """
        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout
        self.serial = None

    def __enter__(self):
        """
        Acquires the serial port.
        """
        self.serial = serial.Serial(port=self.port, baudrate=self.baudrate, timeout=self.timeout)
        if not self.serial.is_open:
            raise SuperKeyInterfaceError("Failed to open serial port!")
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        """
        Releases the serial port.
        """
        if self.serial is not None and self.serial.is_open:
            self.serial.close()

    def __validate_serial(self):
        """
        Raises an error if the serial port is not open.
        """
        if self.serial is None:
            raise SuperKeyInterfaceError("The serial port is not open.")

    def __check_empty_reply(self) -> bool:
        """
        Attempts to receive a generic empty reply from the device.
        """
        # Receive reply and verify we got enough data
        reply = self.receive(HEADER_STRUCT_SIZE)
        if len(reply) != HEADER_STRUCT_SIZE:
            raise SuperKeyInterfaceError('No reply received.')

        # Unpack the header and verify the size and CRC are correct
        message, size, crc = SuperKeyInterface.unpack_header(reply)
        if size != 0 or crc != 0:
            raise SuperKeyInterfaceError('Invalid reply received.')

        # Check the message ID
        if message == MessageID.REPLY_SUCCESS:
            # Valid reply
            return
        elif message == MessageID.REPLY_INVALID_MESSAGE:
            # Invalid message ID
            raise SuperKeyInterfaceError('Reply: Invalid message ID.')
        elif message == MessageID.REPLY_INVALID_SIZE:
            # Invalid size
            raise SuperKeyInterfaceError('Reply: Invalid size.')
        elif message == MessageID.REPLY_INVALID_CRC:
            # Invalid CRC
            raise SuperKeyInterfaceError('Reply: Invalid CRC.')
        elif message == MessageID.REPLY_INVALID_PAYLOAD:
            # Invalid payload
            raise SuperKeyInterfaceError('Reply: Invalid payload.')
        else:
            # ...?
            raise SuperKeyInterfaceError('Reply: Unknown reply?')

    @staticmethod
    def crc16(buffer: bytes, seed: int = 0xFFFF):
        """
        Returns a 16-bit CRC for the specified buffer.
        """
        # Adapted from avrlibc _crc16_update implementation
        def update(crc: int, b: int) -> int:
            crc ^= b & 0xFF
            for _ in range(8):
                if crc & 1:
                    crc = (crc >> 1) ^ 0xA001
                else:
                    crc >>= 1
            return crc & 0xFFFF

        # Calculate and return CRC
        crc = seed
        for b in buffer:
            crc = update(crc, b)
        return crc

    @staticmethod
    def pack_header(message: int, size: int = 0, crc: int = 0) -> bytes:
        """
        Returns a correctly formatted header struct with the specified values.
        """
        return struct.pack(HEADER_STRUCT_FORMAT, message, size, crc)

    @staticmethod
    def unpack_header(buffer: bytes) -> Tuple[int, int, int]:
        """
        Unpacks the specified header struct and returns the parsed values.
        """
        if len(buffer) != HEADER_STRUCT_SIZE:
            raise ValueError("The buffer has the wrong size!")
        return struct.unpack(HEADER_STRUCT_FORMAT, buffer)

    def send(self, buffer: bytes):
        """
        Transmits the specified buffer.
        """
        self.__validate_serial()
        self.serial.write(buffer)

    def receive(self, size: int):
        """
        Receives the specified number of bytes.
        """
        self.__validate_serial()
        return self.serial.read(size=size)

    def autokey(self, string: str):
        """
        Queues the specified string to be auto-keyed.
        """
        # Assemble packet
        payload = bytes(string, encoding='ascii') + b'\x00' # null char is not added by default
        size = len(payload)
        crc = SuperKeyInterface.crc16(payload)
        header = SuperKeyInterface.pack_header(MessageID.REQUEST_AUTOKEY, size, crc)

        # Send packet
        self.send(header)
        self.send(payload)
        self.__check_empty_reply()

    def panic(self):
        """
        Sends a command to immediately and unconditionally stop keying.
        """
        # Assemble packet
        header = SuperKeyInterface.pack_header(MessageID.REQUEST_PANIC)

        # Send packet
        self.send(header)
        self.__check_empty_reply()

    def ping(self):
        """
        Sends a ping command to the device. Returns True if a valid reply was received.
        """
        # Assemble packet
        header = SuperKeyInterface.pack_header(MessageID.REQUEST_PING)

        # Send packet
        self.send(header)
        self.__check_empty_reply()

    def restore_default_config(self):
        """
        Sends a command to restore the device's default configuration.
        """
        # Assemble packet
        header = SuperKeyInterface.pack_header(MessageID.REQUEST_RESTORE_DEFAULT_CONFIG)

        # Send packet
        self.send(header)
        self.__check_empty_reply()

    def set_buzzer_enabled(self, enabled: bool):
        """
        Sends a command to enable or disable the buzzer.
        """
        # Assemble packet
        payload = struct.pack('<?', enabled)
        size = len(payload)
        crc = SuperKeyInterface.crc16(payload)
        header = SuperKeyInterface.pack_header(MessageID.REQUEST_SET_BUZZER_ENABLED, size, crc)

        # Send packet
        self.send(header)
        self.send(payload)
        self.__check_empty_reply()

    def set_buzzer_frequency(self, frequency: int):
        """
        Sends a command to set the buzzer frequency.
        """
        # Assemble packet
        payload = struct.pack('<H', frequency)
        size = len(payload)
        crc = SuperKeyInterface.crc16(payload)
        header = SuperKeyInterface.pack_header(MessageID.REQUEST_SET_BUZZER_FREQUENCY, size, crc)

        # Send packet
        self.send(header)
        self.send(payload)
        self.__check_empty_reply()
