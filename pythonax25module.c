#include <Python.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <netdb.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <poll.h>

// #include <curses.h>

#include <sys/socket.h>
#include <net/if.h>
#include <net/ethernet.h>

#include <netax25/ax25.h>
#include <netax25/axconfig.h>
#include <netax25/axlib.h>

static PyObject * config_load_ports(PyObject* self, PyObject* args) {
    int activePort;
    activePort = ax25_config_load_ports();
    return PyLong_FromLong(activePort);
}

static PyObject * config_get_first_port(PyObject* self, PyObject* args) {
    char *portName;
    portName = ax25_config_get_next(NULL);
    return Py_BuildValue("s", portName);
}

static PyObject * config_get_next_port(PyObject* self, PyObject* args) {
    char *portName;
    char *nextPort;
    PyArg_ParseTuple(args, "s", &portName);
    nextPort = ax25_config_get_next(portName);
    return Py_BuildValue("s", nextPort);
}

static PyObject * config_get_port_name(PyObject* self, PyObject* args) {
    char *device;
    char *portName;
    PyArg_ParseTuple(args, "s", &device);
    portName = ax25_config_get_name(device);
    return Py_BuildValue("s", portName);
}

static PyObject * config_get_address(PyObject* self, PyObject* args) {
    char *portName;
    char *address;
    PyArg_ParseTuple(args, "s", &portName);
    address = ax25_config_get_addr(portName);
    return Py_BuildValue("s", address);
}

static PyObject * config_get_device(PyObject* self, PyObject* args) {
    char *device;
    char *portName;
    PyArg_ParseTuple(args, "s", &portName);
    device = ax25_config_get_dev(portName);
    return Py_BuildValue("s", device);
}

static PyObject * config_get_window(PyObject* self, PyObject* args) {
    int window;
    char *portName;
    PyArg_ParseTuple(args, "s", &portName);
    window = ax25_config_get_window(portName);
    return PyLong_FromLong(window);
}

static PyObject * config_get_packet_length(PyObject* self, PyObject* args) {
    int packetLength;
    char *portName;
    PyArg_ParseTuple(args, "s", &portName);
    packetLength = ax25_config_get_paclen(portName);
    return PyLong_FromLong(packetLength);
}

static PyObject * config_get_baudrate(PyObject* self, PyObject* args) {
    int baudRate;
    char *portName;
    PyArg_ParseTuple(args, "s", &portName);
    baudRate = ax25_config_get_baud(portName);
    return Py_BuildValue("i", baudRate);
}

static PyObject * config_get_description(PyObject* self, PyObject* args) {
    char *description;
    char *portName;
    PyArg_ParseTuple(args, "s", &portName);
    description = ax25_config_get_desc(portName);
    return Py_BuildValue("s", description);
}

static PyObject * aton_entry(PyObject* self, PyObject* args) {
    char *callsignNetwork = null_ax25_address.ax25_call;
    char *callsignString;
    int result;

    PyArg_ParseTuple(args, "s", &callsignString);
    result = ax25_aton_entry(callsignString, callsignNetwork);
    return Py_BuildValue("is", result, callsignNetwork);
}

static PyObject * ntoa(PyObject* self, PyObject* args) {
    static PyObject * callsignPython;
    char *callsignNetwork;
    char *callsignString;

    ax25_address *callsign = &null_ax25_address;

    if (!PyArg_ParseTuple(args, "y", &callsignNetwork))
        fprintf(stderr, "ERROR: CANNOT ASSIGN\n");

    strncpy(callsign->ax25_call, callsignNetwork, 7);
    callsignString = ax25_ntoa(callsign);

    callsignPython = Py_BuildValue("s", callsignString);

    return callsignPython;
}

static PyObject * datagram_socket(PyObject* self, PyObject* args) {
    int fileDescriptor;
    fileDescriptor = socket(AF_AX25, SOCK_DGRAM, 0);
    return PyLong_FromLong(fileDescriptor);
}

static PyObject * datagram_bind(PyObject* self, PyObject* args) {
  struct full_sockaddr_ax25 src;
  char *portcall, *srccall;
  int len, sock, result;

  PyArg_ParseTuple(args, "iss", &sock, &srccall, &portcall);

  char * addr = malloc(sizeof(char*) * (strlen(srccall) + strlen(portcall) + 2));
  sprintf(addr, "%s %s", srccall, portcall);

  len = ax25_aton(addr, &src);

  free(addr);

  // Binding the socket to source
  if (bind(sock, (struct sockaddr *)&src, len) == -1) {
    result = 1;
  }
  else {
    result = 0;
  }

  return PyLong_FromLong(result);

}

static PyObject * datagram_tx_digi(PyObject* self, PyObject* args) {
  struct full_sockaddr_ax25 dest;
  char *destcall = NULL, *digicall = NULL;
  char *message;
  int dlen, sock, result;

  PyArg_ParseTuple(args, "isss", &sock, &destcall, &digicall, &message);

  char * addr = malloc(sizeof(char*) * (strlen(destcall) + strlen(digicall) + 2));
  sprintf(addr, "%s %s", destcall, digicall);

  dlen = ax25_aton(addr, &dest);

  free(addr);

  // Send a datagram packet to socket
  if (sendto(sock, message, strlen(message), 0, (struct sockaddr *)&dest, dlen) == -1) {
    result = 1;
  }

  result = 0;
  return PyLong_FromLong(result);
}

static PyObject * datagram_tx(PyObject* self, PyObject* args) {
  struct full_sockaddr_ax25 dest;
  char *destcall = NULL;
  char *message;
  int dlen, sock, result;

  PyArg_ParseTuple(args, "iss", &sock, &destcall, &message);

  dlen = ax25_aton(destcall, &dest);

  // Send a datagram packet to socket
  if (sendto(sock, message, strlen(message), 0, (struct sockaddr *)&dest, dlen) == -1) {
    result = 1;
  }

  result = 0;
  return PyLong_FromLong(result);
}

// Using PF_PACKET Socket

static PyObject * packet_socket(PyObject* self, PyObject* args) {
    int fileDescriptor;
    fileDescriptor = socket(PF_PACKET, SOCK_PACKET, htons(ETH_P_AX25));
    return PyLong_FromLong(fileDescriptor);
}

// Close a socket
static PyObject * close_socket(PyObject* self, PyObject* args) {
  int fileDescriptor;
  int result;

  PyArg_ParseTuple(args, "i", &fileDescriptor);
  result = close(fileDescriptor);
  return PyLong_FromLong(result);
}

static PyObject * packet_tx(PyObject* self, PyObject* args) {
    int fileDescriptor;
    int result;
    int length;
    char *buffer;
    char *destination;
    struct sockaddr socketAddress;
    int addressSize = sizeof(socketAddress);
    unsigned char newBuffer[1000];
    int bufferLength;
    int i;
    int k;
    unsigned char charBuffer;

    PyArg_ParseTuple(args, "isis", &fileDescriptor, &buffer, &length, &destination);

    bufferLength = strlen(buffer);

    i = 0;
    k = 0;

    while ( i < bufferLength ) {
        charBuffer = (buffer[i++] & 0x0f) << 4;
        charBuffer = charBuffer | (buffer[i++] & 0x0f);
        newBuffer[k++] = charBuffer;
    }

    strcpy(socketAddress.sa_data, destination);
    socketAddress.sa_family = AF_AX25;

    result = sendto(fileDescriptor, newBuffer, k, 0, &socketAddress, addressSize);

    return Py_BuildValue("i", result);
}

static PyObject *  packet_rx(PyObject* self, PyObject* args) {
    int fileDescriptor;
    int result;
    int addressSize;
    int packetSize;
    int timeout;

    struct sockaddr socketAddress;
    struct pollfd pollFileDescriptor;

    unsigned char receiveBuffer[1024];

    PyArg_ParseTuple(args, "ii", &fileDescriptor, &timeout);

    // Poll the socket for an available data
    pollFileDescriptor.fd = fileDescriptor;
    pollFileDescriptor.events = POLLRDNORM;

    result = poll(&pollFileDescriptor, 1, timeout);

    // Read all packet received
    packetSize = 0;
    socketAddress.sa_family = AF_UNSPEC;
    strcpy(socketAddress.sa_data, "");

    if (result == 1) {
        addressSize = sizeof(socketAddress);
        packetSize = recvfrom(fileDescriptor, receiveBuffer, sizeof(receiveBuffer),
                                  0, &socketAddress, (socklen_t*)&addressSize);
    }

    return Py_BuildValue("(is)y#", socketAddress.sa_family, socketAddress.sa_data,
                          receiveBuffer, packetSize);
}

static PyObject *PythonAx25Error;

//////////////////////////////////////////
// Define methods
//////////////////////////////////////////

static PyMethodDef python_ax25_functions[] = {
    {"config_load_ports", config_load_ports, METH_VARARGS, ""},
    {"config_get_first_port", config_get_first_port, METH_VARARGS, ""},
    {"config_get_next_port", config_get_next_port, METH_VARARGS, ""},
    {"config_get_port_name", config_get_port_name, METH_VARARGS, ""},
    {"config_get_address", config_get_address, METH_VARARGS, ""},
    {"config_get_device", config_get_device, METH_VARARGS, ""},
    {"config_get_window", config_get_window, METH_VARARGS, ""},
    {"config_get_packet_length", config_get_packet_length, METH_VARARGS, ""},
    {"config_get_baudrate", config_get_baudrate, METH_VARARGS, ""},
    {"config_get_description", config_get_description, METH_VARARGS, ""},
    {"network_to_ascii", ntoa, METH_VARARGS, ""},
    {"ascii_to_network", aton_entry, METH_VARARGS, ""},
    {"datagram_socket", datagram_socket, METH_VARARGS, ""},
    {"datagram_bind", datagram_bind, METH_VARARGS, ""},
    {"datagram_tx_digi", datagram_tx_digi, METH_VARARGS, ""},
    {"datagram_tx", datagram_tx, METH_VARARGS, ""},
    {"packet_socket", packet_socket, METH_VARARGS, ""},
    {"packet_rx", packet_rx, METH_VARARGS, ""},
    {"packet_tx", packet_tx, METH_VARARGS, ""},
    {"close_socket", close_socket, METH_VARARGS, ""},
    {NULL, NULL, 0, NULL}
};

// Initialize module
static struct PyModuleDef moduledef = {
  PyModuleDef_HEAD_INIT,
  "pythonax25",
  "This is a python module for ax.25",
  -1,
  python_ax25_functions,
  NULL,
  NULL,
  NULL,
  NULL,
};

PyMODINIT_FUNC

PyInit_pythonax25(void) {
  PyObject * m;
  m = PyModule_Create(&moduledef);

  if (m == NULL)
    return NULL;

  PythonAx25Error = PyErr_NewException("pythonax25.error", NULL, NULL);
  Py_INCREF(PythonAx25Error);
  PyModule_AddObject(m, "error", PythonAx25Error);

  return m;
}
