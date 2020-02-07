#!/usr/bin/python3

import pythonax25
import time

def main():
    # Check if there's any active AX25 port
    if pythonax25.config_load_ports() > 0:
        # Get the device name of the first port
        axport = pythonax25.config_get_first_port()
        axdevice = pythonax25.config_get_device(axport)
        axaddress = pythonax25.config_get_address(axport)
    else:
        exit(0)

    # Initiate a datagram socket
    socket = pythonax25.datagram_socket()

    srcCall = 'YD0ABH-13'
    portCall = axaddress

    res = pythonax25.datagram_bind(socket, srcCall, portCall)
    print(res)

    dest = 'APZINA'
    digi = 'WIDE2-2'
    msg = '!0611.08S/10649.35E$ INARad LoRa APRS#CO2=500'

    res = pythonax25.datagram_tx_digi(socket, dest, digi, msg)
    print(res)

    time.sleep(1)

    msg = 'T#001,034,034,034,034,000,11111111'
    res = pythonax25.datagram_tx_digi(socket, dest, digi, msg)
    print(res)

    time.sleep(1)

    msg = '_07190749c045s055g055t076r001h45b10101'
    res = pythonax25.datagram_tx_digi(socket, dest, digi, msg)
    print(res)

    pythonax25.close_socket(socket)

    return res

if __name__ == '__main__':
    main()
