import serial  # PySerial documentation: https://pythonhosted.org/pyserial/index.html
from time import sleep


try:                                                        # 1 s timeout
    global ser
    ser = serial.Serial(port="/dev/serial1", baudrate=9600,
                        timeout=0.1, parity=serial.PARITY_NONE,
                        stopbits=serial.STOPBITS_ONE,
                        bytesize=serial.EIGHTBITS)
#    return ser
except (OSError, serial.SerialException):
    print ("init_load_cell_serial_port(): Serial " + str(serialPort) + " did not connect")
#    return -1

while True:
    ser.write("Quack!")
    print("Transmitted...\n")
    sleep(1)
