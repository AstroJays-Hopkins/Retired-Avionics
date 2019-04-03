import serial  # PySerial documentation: https://pythonhosted.org/pyserial/index.html
try:
    import RPi.GPIO as GPIO  # RPi.GPIO documentation: https://sourceforge.net/p/raspberry-gpio-python/wiki/
except:
    print("Error importing RPi.GPIO!  This is probably because you need superuser privileges.  You can achieve this by using 'sudo' to run your script")

SELECT_GPIO_PIN_TUPLE = (23,24)  ## TEMPORARY VALUES.  CHANGE AS NEEDED.
DESELECT_TUPLE = (1,1)  ## Corresponds to C3 pins on the MUX, which are grounded and will read nothing.

## Set pin numbering scheme to correspond to numbers printed on board
GPIO.setmode(GPIO.BOARD)

## Initialize load cell GPIO and serial
## Is putting it in a separate function necessary?
def begin():
    ## Set select pins to output
    GPIO.setup(SELECT_GPIO_PIN_TUPLE, GPIO.OUT)
    GPIO.setup(self.muxsel_pin, GPIO.OUT)  # set GPIO pin as output
    # Initialise serial
    try:                                                        # 1 s timeout
        ser = serial.Serial(port="/dev/serial0", baudrate=9600, timeout=1)
        return ser
    except (OSError, serial.SerialException):
        print ("init_load_cell_serial_port(): Serial " + str(serialPort) + " did not connect")
        return -1

def end():
    GPIO.cleanup()

# There is no Adafruit library for load cells, so there is no preexisting
# load cell class to subclass.  I've still made my own below to keep
# everything organized.
class Load_Cell:
    # Just another access point for the serial port used to read load cell data
    serial_port = False

    # select_tuple is a tuple of bits written to A and B of the MUX to select the Tx/Rx lines of the the load cell
    def __init__(self, select_tuple, serport):
        self.select_tuple = select_tuple  # GPIO pin to raise in order to read load cell
        self.last_reading = -1
        serial_port = serport

    def select(self):
        GPIO.output(SELECT_GPIO_PIN_TUPLE, self.select_tuple)

    def deselect(self):
        GPIO.output(SELECT_GPIO_PIN_TUPLE, DESELECT_TUPLE)

    # Following method adapted from A. Cornelio's readWeight1() in
    # "Avionics/Cold Flow/Wet_Dress/Wet_Dress.ino"
    def read_weight():
        # Select load cell
        self.select()
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
        self.deselect()


# Iterates through load cells objects, reads load cells, and returns array of measured weights
def read_load_cells(load_cell_list):
    i = 0
    weights = []
    for load_cell in load_cell_list:
        weights[i] = load_cell.read_weight()
        i += 1
    return weights
