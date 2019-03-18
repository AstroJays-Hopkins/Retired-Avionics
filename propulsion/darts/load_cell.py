import serial  # PySerial documentation: https://pythonhosted.org/pyserial/index.html
import RPi.GPIO as GPIO  # RPi.GPIO documentation: https://sourceforge.net/p/raspberry-gpio-python/wiki/

GPIO.setmode(GPIO.BOARD)

def init_load_cell_serial_port():
    global ser

    try:                                                        # 1 s timeout
        ser = serial.Serial(port="/dev/serial0", baudrate=9600, timeout=1)
    except (OSError, serial.SerialException):
        print "init_load_cell_serial_port(): Serial '" + str(serialPort) + "' did not connect"
        return -1



# There is no Adafruit library for load cells, so there is no preexisting
# load cell class to subclass.  I've still made my own below to keep
# everything organized.
class Load_Cell:
    # Just another access point for the serial port used to read load cell data
    serial_port = ser

    def __init__(self, muxsel_pin):
        self.muxsel_pin = muxsel_pin  # GPIO pin to raise in order to read load cell
        GPIO.setup(self.muxsel_pin, GPIO.OUT)  # set GPIO pin as output
        self.last_reading = -1

    # Following method adapted from A. Cornelio's readWeight1() in
    # "Avionics/Cold Flow/Wet_Dress/Wet_Dress.ino"
    def read_weight():
        # Select load cell
        GPIO.output(self.muxsel_pin, True)
        # Empty serial buffer
        while (ser.in_waiting != 0):
            ser.read()
        # Trigger read by sending one arbitrary character
        ser.write(".")
        # Read to first comma (",") in response;
        # discard characters including comma (timestamp; not needed)
        while (ser.read() != ","):  ## do nothing in loop; reading done as part of condition checking
            pass
        weight_string = ""
        # Read weight and concatenate to string
        while True:
            incoming_char = ser.read()
            if incoming_char != ",":
                weight_string += incoming_char
            else:
                break  # Read to next comma, which indicates that weight has been completely read.  Leave while loop
        self.last_reading = float(weight_string)  # convert to floating point number and store
        # Deselect load cell
        GPIO.output(self.muxsel_pin, False)
