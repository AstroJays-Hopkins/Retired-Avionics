import load_cell as lc
import RPi.GPIO as GPIO  # RPi.GPIO documentation: https://sourceforge.net/p/raspberry-gpio-python/wiki/

### LIST OF UNDEFINED VARIABLES, CONSTANTS --- UPDATE as NEEDED ###
# LC_MUXSEL_PINS, EMERG_MBVALVE_SHUTOFF_PIN


### VARIABLES TO STORE SENSOR OBJECTS ###

# Where load cell objects are stored
# To access the last reading of an individual load cell i,
# use LOAD_CELLS[i].last_reading
LOAD_CELLS = []


def init():
    # Initialize load cell serial port, then instantiate load cell objects
    lc.init_load_cell_serial_port()
    i = 0
    for muxsel_pin in LC_MUXSEL_PINS:  ## soo... LC_MUXSEL_PINS is supposed to come from that external configuration file, but I don't know how to do that yet...
        LOAD_CELLS[i] = Load_Cell(muxsel_pin)
        i++

    # Configure GPIO pin for telling the ignition computer to close the motorized ball valve in an emergency:
    GPIO.setup(EMERG_MBVALVE_SHUTOFF_PIN, GPIO.OUT)
    GPIO.output(EMERG_MBVALVE_SHUTOFF_PIN, False)



### FUNCTIONS TO ITERATE THROUGH ALL SENSORS ###

# Iterates through load cells objects, reads load cells, and returns array of measured weights
def read_load_cells():
    i = 0
    weights = []
    for load_cell in LOAD_CELLS:
        weights[i] = load_cell.read_weight()
        i++
    return weights







### OTHER FUNCTIONS ###

# Tell ignition computer to close motorized ball valve.
# Doesn't actually close the valve (that's the igcomp's job) --- hence the name
def emergency_shutdown():
    GPIO.output(EMERG_MBVALVE_SHUTOFF_PIN, True)
## FIXME MAYBE?  Should we have a function to order the igcomp to open the ball valve?


