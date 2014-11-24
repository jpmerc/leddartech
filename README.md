leddartech
==========
This package depends on libmodbus5 and libmodbus-dev

To compile :
    g++ leddartech.cpp -o leddartech -I/usr/include/modbus  -lmodbus

To automatically get the suffix of the command (-I/usr/include/modbus  -lmodbus), enter this command in command line :
    pkg-config --libs --cflags libmodbus
