#
# @file     scripts/example.py
# @brief    Example Python file demonstrating usage of the superkey library.
#
# @author   Chris Vig (chris@invictus.so)
# @date     2025-08-30
# @cpyrt    © 2025 by Chris Vig. Licensed under the GNU General Public License v3 (GPLv3).
#

# ------------------------------------------------------ IMPORTS -------------------------------------------------------

import time

from superkey import *

# ----------------------------------------------------- PROCEDURES -----------------------------------------------------

def demo_autokey():
    """
    Demonstrates the "autokey" command.
    """
    with superkey_intf.SuperKeyInterface() as intf:
        print('Keying "cq cq"...')
        intf.autokey('cq cq')
        time.sleep(6.)

def demo_panic():
    """
    Demonstrates the "panic" command.
    """
    with superkey_intf.SuperKeyInterface() as intf:
        print('Keying a very long string...')
        intf.autokey('very long string that will not get fully keyed')
        time.sleep(2.)
        print('Sending panic command...')
        intf.panic()
        time.sleep(2.)

def demo_ping():
    """
    Demonstrates the "ping" command.
    """
    with superkey_intf.SuperKeyInterface() as intf:
        intf.ping()
        print('Ping succeeded!')
        time.sleep(2.)

def demo_restore_default_config():
    """
    Demonstrates the "restore default configuration" command.
    """
    with superkey_intf.SuperKeyInterface() as intf:
        # Set buzzer frequency
        print('Setting frequency to 900 Hz...')
        intf.set_buzzer_frequency(900)
        intf.autokey('cq')
        time.sleep(3.)
        # Restore config
        print('Restoring default configuration...')
        intf.restore_default_config()
        intf.autokey('cq')
        time.sleep(3.)

def demo_set_buzzer_enabled():
    """
    Demonstrates the "set buzzer enabled" command.
    """
    with superkey_intf.SuperKeyInterface() as intf:
        # Buzzer disabled
        print('Disabling buzzer...')
        intf.set_buzzer_enabled(False)
        intf.autokey('cq')
        time.sleep(3.)
        # Buzzer enabled
        print('Enabling buzzer...')
        intf.set_buzzer_enabled(True)
        intf.autokey('cq')
        time.sleep(3.)

def demo_set_buzzer_frequency():
    """
    Demonstrates the "set buzzer frequency" command.
    """
    with superkey_intf.SuperKeyInterface() as intf:
        intf.set_buzzer_enabled(True)
        # Frequency 800 Hz
        print('Setting frequency to 800 Hz...')
        intf.set_buzzer_frequency(800)
        intf.autokey('cq')
        time.sleep(3.)
        # Frequency 700 Hz
        print('Setting frequency to 700 Hz...')
        intf.set_buzzer_frequency(700)
        intf.autokey('cq')
        time.sleep(3.)

# --------------------------------------------------- MAIN PROCEDURE ---------------------------------------------------

if __name__ == '__main__':
    # Run all demos
    demo_autokey()
    demo_panic()
    demo_ping()
    demo_restore_default_config()
    demo_set_buzzer_enabled()
    demo_set_buzzer_frequency()
