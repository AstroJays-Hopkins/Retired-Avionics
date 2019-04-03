## LAST UPDATE 03/30/2019 AT 12:45 PM ##
## THIS IS UNFINISHED CODE PLS HELP ##
## TODO LIST: ##
# - function to read PTs -- ET TO FINISH
# - function to read load cells -- GPS TO FINISH
# - FUNCTION TO READ COMMANDS FROM WIFI AND PROCESS THEM - VENT / DISCONNECT !!!!!!!

from datetime import datetime
from csv import writer
import time
import load_cell as lc
import RocketThermocouple as tc
import pressure_transducer as pt #change this
import QuickDisconnect as qd
try:
    import RPi.GPIO as GPIO  # RPi.GPIO documentation: https://sourceforge.net/p/raspberry-gpio-python/wiki/
except:
    print("Error importing RPi.GPIO!  This is probably because you need superuser privileges.  You can achieve this by using 'sudo' to run your script")
    
### LIST OF UNDEFINED VARIABLES, CONSTANTS --- UPDATE as NEEDED ###
# LC_SEL_TUPLES, EMERG_MBVALVE_SHUTOFF_PIN, VENT_VALVE_SHUTOFF_PIN, PT_CHANNELS, TC_CS_PINS, MBVALVE_DETECT_PIN, 

CRIT_T = 309.5
CRIT_P = 7240

Is_Critical = False

### VARIABLES TO STORE SENSOR OBJECTS ###
data = []
QDSTATE = False # false for closed true for open
VENTVALVE = False # false for closed true for open

# Where load cell objects are stored
# To access the last reading of an individual load cell i,
# use LOAD_CELLS[i].last_reading
LOAD_CELLS = []
global Ser     # Serial port

# Place to put TC objects
TCs = []

# Place to put PT objects
PTs = []

def init():
    # Initialize load cell serial and GPIO, then instantiate load cell objects
    Ser = lc.begin()
    i = 0
    for select_tuple in LC_SEL_TUPLES:  ## soo... LC_SEL_TUPLES is supposed to come from that external configuration file, but I don't know how to do that yet...
        LOAD_CELLS[i] = lc.Load_Cell(select_tuple, Ser)
        i += 1

    # Initialize thermocouples
    i = 0
    for cs_pin in TC_CS_PINS:
        TCs[i] = tc.Thermocouple(cs_pin)
        i += 1

    # Initialize PTs
    i = 0
    for pt_chan in PT_CHANNELS:
        PTs[i] = pt.PressureTransducer(pt_chan)
        i += 1

    # Configure GPIO pin for telling the ignition computer to close the motorized ball valve in an emergency:
    GPIO.setup(EMERG_MBVALVE_SHUTOFF_PIN, GPIO.OUT)
    GPIO.setup(MBVALVE_DETECT_PIN, GPIO.IN)
    GPIO.output(EMERG_MBVALVE_SHUTOFF_PIN, False)


### FUNCTIONS TO ITERATE THROUGH ALL SENSORS ###
def collectData():
    data.append(tc.readThermocouples(TCs))
    ## change the critical checks to being a 2 state system so it doesnt # continuiously call emergency shutdown
    i = 0
    for thermocouple in TCs:
        if (thermocouple.last_reading > CRIT_T and Is_Critical == 0):
            emergency_shutdown()
            Is_Critical = 1
            print('EMERGENCY SHUTDOWN: Critical Temperature detected')
        i += 1
            
    data.append(pt.readPressureTransducers(PTs))
    i = 0
    for pt in PTs:
        if (pt.last_reading > CRIT_P and Is_Critical == 0):
            emergency_shutdown()
            Is_Critical = 1
            print('EMERGENCY SHUTDOWN: Critical Pressure detected')
        i += 1
        
    data.append(read_load_cells(LOAD_CELLS)) # change this 
    
    data.append(VENTVALVE) 
      
    data.append(QDSTATE)
    
    data.append(getBallValveState()) 
            
### OTHER FUNCTIONS ###

# Tell ignition computer to close motorized ball valve.
# Doesn't actually close the valve (that's the igcomp's job) --- hence the name
def emergency_shutdown():
    GPIO.output(EMERG_MBVALVE_SHUTOFF_PIN, True) #this needs to be a actuate ball valve function on its own
    Vent()
    
# Tell ignition computer to open venting solenoid
def Vent():
	GPIO.output(VENT_VALVE_SHUTOFF_PIN, True)
	VENTVALVE = True
	return VENTVALVE
## FIXME MAYBE?  Should we have a function to order the igcomp to open the ball valve?

## TODO##
def getDisconnectState():
    return qd.get_state()

def getBallValveState():
	return GPIO.input(MBVALVE_DETECT_PIN)
    
    #kjadsflk  
	
### DATA LOGGING AND TRANSMISSION ###

#Writes data
def writedata(args):
    packet = []
    packet.append(datetime.now())
    for i in range(0,len(args)):
        packet.append(args[i])
    data_writer.writerow(packet)

## There used to be an ' import csv ' statement here and ' writer(log) ' was ' csv.writer(log) '
with open('DATA1.csv','a',newline='') as log:
    data_writer = writer(log)

    #Header row so you know what you're looking at (change as necessary)
    data_writer.writerow(['Timestamp','TC1','TC2','TC3','TC4','TC5','TC6','PT1','PT1','PT3','PT4','LC1','LC2','VentValve','FuelValve','BallValve'])

    while True:
        #collect all data here and assign to variables
        #slam all that shite into the writedata function (or append to a list each time a sensor is read)
        writedata(data)
        time.sleep(0.01)
        data = []
