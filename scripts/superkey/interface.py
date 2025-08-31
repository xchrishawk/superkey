#
# @file     scripts/superkey/interface.py
# @brief    Module defining the Interface class.
#
# @author   Chris Vig (chris@invictus.so)
# @date     2025-08-29
# @cpyrt    © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
#

# ------------------------------------------------------ IMPORTS -------------------------------------------------------

import serial
import struct
from typing import Optional, Tuple

from .types import *

# ------------------------------------------------------ EXPORTS -------------------------------------------------------

__all__ = [
    'InterfaceError',
    'InvalidMessageError',
    'InvalidSizeError',
    'InvalidCRCError',
    'InvalidPayloadError',
    'Interface',
    'InteractiveInterface',
]

# ----------------------------------------------------- CONSTANTS ------------------------------------------------------

# Header
HEADER_STRUCT_FORMAT = '<HxxHH'
HEADER_STRUCT_SIZE = struct.calcsize(HEADER_STRUCT_FORMAT)

# ------------------------------------------------------- TYPES --------------------------------------------------------

class InterfaceError(RuntimeError):
    """Base class for errors thrown by `Interface`."""
    def __init__(self, message: str):
        """Initializes a new error with the specified message."""
        super().__init__(message)


class InvalidMessageError(InterfaceError):
    """Exception indicating that a `REPLY_INVALID_MESSAGE` reply was received."""
    def __init__(self):
        """Initializes a new instance."""
        super().__init__("Reply: Invalid message.")


class InvalidSizeError(InterfaceError):
    """Exception indicating that a `REPLY_INVALID_SIZE` reply was received."""
    def __init__(self):
        """Initializes a new instance."""
        super().__init__("Reply: Invalid size.")


class InvalidCRCError(InterfaceError):
    """Exception indicating that a `REPLY_INVALID_CRC` reply was received."""
    def __init__(self):
        """Initializes a new instance."""
        super().__init__("Reply: Invalid CRC.")


class InvalidPayloadError(InterfaceError):
    """Exception indicating that a `REPLY_INVALID_PAYLOAD` reply was received."""
    def __init__(self):
        """Initializes a new instance."""
        super().__init__("Reply: Invalid payload.")


class Interface:
    """
    Class encapsulating the serial interface provided by the SuperKey hardware.
    """
    def __init__(self, port: str = "COM1", baudrate: int = 19200, timeout: Optional[float] = 1.0):
        """
        Initializes a new instance with the specified serial port configuration.
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
            raise InterfaceError("Failed to open serial port!")
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        """
        Releases the serial port.
        """
        if self.serial is not None and self.serial.is_open:
            self.serial.close()

    def autokey(self, string: str):
        """
        Sends the `REQUEST_AUTOKEY` command. Queues the specified string to be automatically keyed.
        """
        # Assemble packet
        payload = bytes(string, encoding='ascii') + b'\x00' # null char is not added by default
        size = len(payload)
        crc = self.__class__.__crc16(payload)
        header = self.__class__.__pack_header(MessageID.REQUEST_AUTOKEY, size, crc)

        # Send packet
        self.__send(header)
        self.__send(payload)
        self.__check_empty_reply()

    def panic(self):
        """
        Sends the `REQUEST_PANIC` command. Immediately and unconditionally stops keying.
        """
        # Assemble packet
        header = self.__class__.__pack_header(MessageID.REQUEST_PANIC)

        # Send packet
        self.__send(header)
        self.__check_empty_reply()

    def ping(self):
        """
        Sends the `REQUEST_PING` command. Keys a short test message.
        """
        # Assemble packet
        header = self.__class__.__pack_header(MessageID.REQUEST_PING)

        # Send packet
        self.__send(header)
        self.__check_empty_reply()

    def restore_default_config(self):
        """
        Sends the `REQUEST_RESTORE_DEFAULT_CONFIG` command. Restores the device to its default configuration.
        """
        # Assemble packet
        header = self.__class__.__pack_header(MessageID.REQUEST_RESTORE_DEFAULT_CONFIG)

        # Send packet
        self.__send(header)
        self.__check_empty_reply()

    def set_buzzer_enabled(self, enabled: bool):
        """
        Sends the `REQUEST_SET_BUZZER_ENABLED` command. Enables or disables the device's built-in buzzer.
        """
        # Assemble packet
        payload = struct.pack('<?', enabled)
        size = len(payload)
        crc = self.__class__.__crc16(payload)
        header = self.__class__.__pack_header(MessageID.REQUEST_SET_BUZZER_ENABLED, size, crc)

        # Send packet
        self.__send(header)
        self.__send(payload)
        self.__check_empty_reply()

    def set_buzzer_frequency(self, frequency: int):
        """
        Sends the `REQUEST_SET_BUZZER_FREQUENCY` command. Sets the frequency (in Hz) of the device's built-in buzzer.
        """
        # Assemble packet
        payload = struct.pack('<H', frequency)
        size = len(payload)
        crc = self.__class__.__crc16(payload)
        header = self.__class__.__pack_header(MessageID.REQUEST_SET_BUZZER_FREQUENCY, size, crc)

        # Send packet
        self.__send(header)
        self.__send(payload)
        self.__check_empty_reply()

    def __validate_serial(self):
        """
        Raises an error if the serial port is not open.
        """
        if self.serial is None:
            raise InterfaceError("The serial port is not open.")

    def __check_empty_reply(self) -> bool:
        """
        Attempts to receive a generic empty reply from the device.
        """
        # Receive reply and verify we got enough data
        reply = self.__receive(HEADER_STRUCT_SIZE)
        if len(reply) != HEADER_STRUCT_SIZE:
            raise InterfaceError('No reply received.')

        # Unpack the header and verify the size and CRC are correct
        message, size, crc = self.__class__.__unpack_header(reply)
        if size != 0 or crc != 0:
            raise InterfaceError('Reply: Invalid size / CRC?')

        # Check the message ID
        if message == MessageID.REPLY_SUCCESS:
            return # no error
        elif message == MessageID.REPLY_INVALID_MESSAGE:
            raise InvalidMessageError()
        elif message == MessageID.REPLY_INVALID_SIZE:
            raise InvalidSizeError()
        elif message == MessageID.REPLY_INVALID_CRC:
            raise InvalidCRCError()
        elif message == MessageID.REPLY_INVALID_PAYLOAD:
            raise InvalidPayloadError()
        else:
            raise InterfaceError('Reply: Unknown reply?')

    def __send(self, buffer: bytes):
        """
        Transmits the specified buffer.
        """
        self.__validate_serial()
        self.serial.write(buffer)

    def __receive(self, size: int):
        """
        Receives the specified number of bytes.
        """
        self.__validate_serial()
        return self.serial.read(size=size)

    @staticmethod
    def __crc16(buffer: bytes, seed: int = 0xFFFF):
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
    def __pack_header(message: int, size: int = 0, crc: int = 0) -> bytes:
        """
        Returns a correctly formatted header struct with the specified values.
        """
        return struct.pack(HEADER_STRUCT_FORMAT, message, size, crc)

    @staticmethod
    def __unpack_header(buffer: bytes) -> Tuple[int, int, int]:
        """
        Unpacks the specified header struct and returns the parsed values.
        """
        return struct.unpack(HEADER_STRUCT_FORMAT, buffer)


class InteractiveInterface:
    """
    Class acting as a proxy for `Interface`, wrapping each function call in a serial port transaction.

    This is intended as a convenience to improve the syntax for **occasional** commands (e.g., in an interactive Python
    terminal). It allows sending commands to the SuperKey without having to wrap each command in a `with` block. It
    should not be used in normal scripts, as there is significant overhead in repeatedly opening and closing the serial
    port for every command.

    Example usage:
    ```
    # Note that no `with` statement is required
    intf = InteractiveInterface()
    intf.set_buzzer_frequency(800)
    intf.set_buzzer_enabled(True)
    ```
    """
    def __init__(self, *args, **kwargs):
        """
        Initializes a new instance with the specified serial port configuration.
        """
        self.__intf = Interface(*args, **kwargs)

    def __getattr__(self, name: str):
        """
        Gets the specified attribute.
        """
        attr = getattr(self.__intf, name)
        if callable(attr):
            def wrapper(*args, **kwargs):
                with self.__intf as _:
                    return attr(*args, **kwargs)
            wrapper.__doc__ = attr.__doc__
            return wrapper
        else:
            return attr
