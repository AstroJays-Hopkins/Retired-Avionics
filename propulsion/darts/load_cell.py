import time
import serial  # PySerial documentation: https://pythonhosted.org/pyserial/index.html
try:
   import RPi.GPIO as GPIO  # RPi.GPIO documentation: https://sourceforge.net/p/raspberry-gpio-python/wiki/
except:
  print("Error importing RPi.GPIO!  This is probably because you need superuser privileges.  You can achieve this by using 'sudo' to run your script")

SELECT_GPIO_PIN_TUPLE = (8,10)  ## TEMPORARY VALUES.  CHANGE AS NEEDED.
DESELECT_TUPLE = (1,1)  ## Corresponds to C3 pins on the MUX, which are grounded and will read nothing.

## Set pin numbering scheme to correspond to numbers printed on board
GPIO.setmode(GPIO.BCM)

## Initialize load cell GPIO and serial
## Is putting it in a separate function necessary?
def begin():
    ## Set select pins to output
    GPIO.setup(SELECT_GPIO_PIN_TUPLE, GPIO.OUT)
    # Initialise serial
    try:                                                        # 1 s timeout
        ser = serial.Serial(port='/dev/ttyS0', baudrate=115200, timeout=1)
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
        self.last_string = ""
        self.serial_port = serport
        self.serial_port.write(b'c')

    def select(self):
        GPIO.output(SELECT_GPIO_PIN_TUPLE, self.select_tuple)

    def deselect(self):
        GPIO.output(SELECT_GPIO_PIN_TUPLE, DESELECT_TUPLE)

    # Following method adapted from A. Cornelio's readWeight1() in
    # "Avionics/Cold Flow/Wet_Dress/Wet_Dress.ino"
    def read_weight(self):
       try:
          # Select load cell
          self.select()
          # Empty serial buffer
          while (self.serial_port.in_waiting != 0):
             self.serial_port.read()
          # Trigger read by sending one arbitrary character
          self.serial_port.write(b'c')
          # Read to first comma (",") in response;
          # discard characters including comma (timestamp; not needed)
          start_listen_time = time.time()
          # while (self.serial_port.read() != ","):  ## do nothing in loop; reading done as part of condition checking
          #     pass
          ##  or ((time.time() - start_listen_time) < 1)
          #print(self.serial_port.read())
          weight_string = ""
          # Read weight and concatenate to string
          while True:
             incoming_char = self.serial_port.read()
             # print(incoming_char)
             if incoming_char != b'\n':
                weight_string += incoming_char.decode("utf-8")
             else:
                break  # Read to next newline, which indicates that weight has been completely read.  Leave while loop
          self.last_string = weight_string  ## <-- Mostly for debugging in case we are unable to parse the weight_string into a float.
          # print(weight_string)
          self.last_reading = -float(weight_string)  # convert to floating point number and store
          # Deselect load cell
          self.deselect()
          return self.last_reading
       except Exception as e:
          print("!!! Error reading load cell:")
          print(str(e))
          print("Returning \"A\"")
          return "A"


# Iterates through load cells objects, reads load cells, and returns array of measured weights
def read_load_cells(load_cell_list):
    weights = []
    for load_cell in load_cell_list:
        weights.append(load_cell.read_weight())
    return weights
