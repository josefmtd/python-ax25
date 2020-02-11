# python-ax25

Python AX.25 Module for Python3

## Introduction

This is a python module designed for Python3 to access AX.25 features. This module is a C extension that can access the AX.25 interface from the Linux kernel.

This C extension is inspired from pyax25 https://github.com/ha5di/pyax25

## Installing the Module

Clone the Git repository

```
$ git clone https://github.com/josefmtd/python-ax25
```

Install the module by running the install script inside the python-ax25 directory

```
$ cd python-ax25
# ./install.sh
```

## Module Functions

Before using any of the functions, make sure to load all the available ports using `config_load_ports()`

```
pythonax25.config_load_ports()

Returns = number of available ports (int)
```

To get the names of available ports, use the `config_get_first_port` and `config_get_next_port`

```
pythonax25.config_get_first_port()

Returns = name of first port (unicode string)


pythonax25.config_get_next_port(portname)

Returns = name of port after 'portname' (unicode string)
```

To retrieve further information for each available port, use these functions:
1. `config_get_port_name(device)`
2. `config_get_address(portname)`
3. `config_get_device(portname)`
4. `config_get_window(portname)`
5. `config_get_packet_length(portname)`
6. `config_get_baudrate(portname)`
7. `config_get_description(portname)`

To change the callsign from ASCII to network format and vice versa, use the functions `ascii_to_network(callsignascii)` and `network_to_ascii(callsignnetwork)`

```
pythonax25.ascii_to_network(callsignascii)

Returns = callsign in network format (byte literal string)


pythonax25.network_to_ascii(callsignnetwork)

Returns = callsign in ascii format (unicode string)
```

For receiving AX.25 packets, the packet socket is mostly used in C programs. Start a socket by using `packet_socket()` and begin receiving by using `packet_rx(fd, timeout)`

```
pythonax25.packet_socket()

Returns = file descriptor (int)


pythonax25.packet_rx(fd, timeout)

Returns = Protocol and Address (tuple of int and string) and packet (byte-literal string)
```

For sending APRS messages, the datagram socket is used. Socket is started by using `datagram_socket()`, bound to a port by using `datagram_bind(fd, srccall, portcall)` and send packets via `datagram_tx(fd, destcall, message)` or `datagram_tx(fd, destcall, digicall, message)`

```
pythonax25.datagram_socket()

Returns = file descriptor (int)


pythonax25.datagram_bind(fd, srccall, destcall)

Returns = result of bind (int)


pythonax25.datagram_tx(fd, destcall, message)

Returns = result of transmission (int)


pythonax25.datagram_tx_digi(fd, destcall, digicall, message)

Returns = result of transmission (int)
```

Closing socket is done by using `close_socket(fd)`

```
pythonax25.close_socket(fd)

Returns = result of close (int)
```

2020 - Josef Matondang
