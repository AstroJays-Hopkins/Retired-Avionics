from datetime import datetime
from csv import writer
import time
import load_cell as lc
import RocketThermocouple as tc
try:
    import RPi.GPIO as GPIO  # RPi.GPIO documentation: https://sourceforge.net/p/raspberry-gpio-python/wiki/
except:
    print("Error importing RPi.GPIO!  This is probably because you need superuser privileges.  You can achieve this by using 'sudo' to run your script")

### LIST OF UNDEFINED VARIABLES, CONSTANTS --- UPDATE as NEEDED ###
# LC_SEL_TUPLES, EMERG_MBVALVE_SHUTOFF_PIN

CRIT_T = 309.5
CRIT_P = 7240

### VARIABLES TO STORE SENSOR OBJECTS ###

# Where load cell objects are stored
# To access the last reading of an individual load cell i,
# use LOAD_CELLS[i].last_reading
LOAD_CELLS = []

# initialize TC objects and data
TC = [] // ?
TC_DATA = []

# initialize PT objects and data
PT = [] // ?
PT_DATA = []

def init():
    # Initialize load cell serial and GPIO, then instantiate load cell objects
    lc.begin()
    i = 0
    for select_tuple in LC_SEL_TUPLES:  ## soo... LC_SEL_TUPLES is supposed to come from that external configuration file, but I don't know how to do that yet...
        LOAD_CELLS[i] = Load_Cell(select_tuple)
        i++

    # Configure GPIO pin for telling the ignition computer to close the motorized ball valve in an emergency:
    GPIO.setup(EMERG_MBVALVE_SHUTOFF_PIN, GPIO.OUT)
    GPIO.output(EMERG_MBVALVE_SHUTOFF_PIN, False)


### FUNCTIONS TO ITERATE THROUGH ALL SENSORS ###
def collectData():
    TC_DATA = tc.readThermocouples()
    // change the critical checks to being a 2 state system so it doesnt continuiously call emergency shutdown
    i = 0
    for temp in TC_DATA:
        if (temp > CRIT_T)
            emergency_shutdown()
            print('EMERGENCY SHUTDOWN: Critical Temperature detected')
        i++
            
    PT_DATA = readPressureTransducers()
    i = 0
    for pressure in PT_DATA:
        if (pressure > CRIT_P)
            emergency_shutdown()
            print('EMERGENCY SHUTDOWN: Critical Pressure detected')
        i++
            
### OTHER FUNCTIONS ###

# Tell ignition computer to close motorized ball valve.
# Doesn't actually close the valve (that's the igcomp's job) --- hence the name
def emergency_shutdown():
    GPIO.output(EMERG_MBVALVE_SHUTOFF_PIN, True)
## FIXME MAYBE?  Should we have a function to order the igcomp to open the ball valve?



### DATA LOGGING AND TRANSMISSION ###

#Writes data
def writedata(args):
    data = []
    data.append(datetime.now())
    for i in range(0,len(args)):
        data.append(args[i])
    data_writer.writerow(data)

## There used to be an ' import csv ' statement here and ' writer(log) ' was ' csv.writer(log) '
with open('DATA1.csv','a',newline='') as log:
    data_writer = writer(log)

    #Header row so you know what you're looking at (change as necessary)
    data_writer.writerow(['Timestamp','TC1','TC2','TC3','TC4','TC5','TC6','PT1','PT1','PT3','PT4','LC1','LC2','VentValve','FuelValve','BallValve'])

    while True:
        #collect all data here and assign to variables
        #slam all that shite into the writedata function (or append to a list each time a sensor is read)
        writedata([0,1,2,3])
        time.sleep(0.01)
