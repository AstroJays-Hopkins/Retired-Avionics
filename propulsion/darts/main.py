## LAST UPDATE 04/05/2019 AT 1:45 AM ##
## THIS IS UNFINISHED CODE PLS HELP ? ##
## TODO LIST: ##

from datetime import datetime
from csv import writer
from adafruit_ads1x15.ads1115 import P0,P1,P2,P3
import threading
import time
import board
from load_cell import LoadCellReader
import RocketThermocouple as tc
import pressure_transducer as pt
from SendToFlask import sendData
## import Server   # doesn't work
# import QuickDisconnect as qd


# import general purpose input/output
try:
    import RPi.GPIO as GPIO  # RPi.GPIO documentation: https://sourceforge.net/p/raspberry-gpio-python/wiki/
except:
    print("Error importing RPi.GPIO!  This is probably because you need superuser privileges.  You can achieve this by using 'sudo' to run your script")
   

### LIST OF UNDEFINED VARIABLES, CONSTANTS --- UPDATE as NEEDED ###
## --> No undefined variables  :)


### BLASTED PINS FOR SENSORS AND DETECTION WHATNOT ###


# Setting pin numbers (BCM Number) for various sensors
EMERG_MBVALVE_SHUTOFF_PIN = 4 # If things get too hot, this pin goes HIGH to tell arduino to shut off ball-valve
MBVALVE_DETECT_PIN = 12 # Moving ball valve
FUEL_SOLENOID_DETECT_PIN = 16 # Fueling solenoid
VENT_VALVE_DETECT_PIN = 25 # vent valve
EMATCH_DETECT_PIN = 13 # electronic match
# DISCONNECT_DETECT_PIN = 16 # detect whether or not fuel hose is still connected (Needs work)


# Setting inputs from arduino/raspberryPi
PT_CHANNELS = [P0,P1,P2] # List of pressure transducer pins
TC_CS_PINS = [board.D17, board.D27, board.D22, board.D5, 
    board.D6, board.D26] # List of thermocouple pins
LC_SEL_TUPLES = [(0, 0), (0, 1), (1, 0)] # Load cells
# LC_SEL_TUPLES = ((0, 0), (0, 1), (1, 0)) # Load cells # should this be a list of tuples?
# TC_CS_PINS = [17,27,22,5,6,13]
#TC_CS_PINS = [board.D17,board.D27]
#LC_SEL_TUPLES = ((0, 0), None)


# Set critical values
CRIT_T = 309.5 # Critical Temperature in Kelvin
# FIXME: Probably VERY WRONG 
CRIT_P = 7240.0 # Critical pressure in ??????????


# Hoopefully we don't start off in critical condition                                                       
global Is_Critical
Is_Critical = False
QDSTATE = False # Quick Disconnect State, false for closed true for open
VENTVALVE = False # Vent valve is open or closed, false for closed true for open


### VARIABLES TO STORE SENSOR OBJECTS ###
# Data stores one reading from each sensor, dumps it somehwere else and 
# is then reset. This acts like a buffer/shovel between sensors and storage
data = []


# Serial input port from arduino.
global Ser  


# Where load cell objects are stored
# To access the last reading of an individual load cell i,
# use LOAD_CELLS[i].last_reading
LC_Reader = None

# Place to put TC objects
TCs = []


# Place to put PT objects
PTs = []


def init():
    global LC_Reader
    LC_Reader = LoadCellReader(["/dev/usbLC0", "/dev/usbLC1", "/dev/usbLC2"])
    
    spi = tc.begin();
    # Initialize thermocouples
    print(TC_CS_PINS)
    for cs_pin in TC_CS_PINS:
        try:
            TCs.append(tc.Thermocouple(cs_pin, spi))
        except Exception as e:
            print(e)
            print("TC Error" + " " + str(cs_pin))

    # Initialize PTs
    print(PT_CHANNELS)
    for pt_chan in PT_CHANNELS:
        try: 
            PTs.append(pt.PressureTransducer(pt_chan))
        except Exception as e:
            print(e)
            print("TC Error" + " " + str(pt_chan))

    # Configure GPIO pin for telling the ignition computer to close the motorized ball valve in an emergency:
    GPIO.setup(EMERG_MBVALVE_SHUTOFF_PIN, GPIO.OUT) # Makes ball-valve pin an output pin
    GPIO.output(EMERG_MBVALVE_SHUTOFF_PIN, False) # Set pin outpuit to be false
    GPIO.setup(MBVALVE_DETECT_PIN, GPIO.IN) # Read position of ball valve (open or close)
    
    # Configure vent GPIO pins
    GPIO.setup(VENT_VALVE_DETECT_PIN, GPIO.IN) # Read position of vent valve (open or close)
    GPIO.setup(FUEL_SOLENOID_DETECT_PIN, GPIO.IN) # Read whether opr not we are fueling (open or close)
    GPIO.setup(EMATCH_DETECT_PIN, GPIO.IN) # Read if Ematch has or hasn't been lit
    
    ## GPIO.setup(MBVALVE_ACTUATING_DETECT_PIN, GPIO.IN)  ## USELESS?  Check belowdecks   avnoks  12 May 2019
    # GPIO.setup(DISCONNECT_DETECT_PIN, GPIO.IN)
    


def collectData():
    """ Iterates through all sensors and collects data
    """
    try:
        # Initialize data array
        data = []
        
        #global Is_Critical
        # See here for why we need the above line: https://stackoverflow.com/questions/10851906/python-3-unboundlocalerror-local-variable-referenced-before-assignment#10852003
        
        # Adding thermocouple data 
        data.extend(tc.readThermocouples(TCs))
        
        # Temperature Safety Check! Make sure tank isn't going to explode
        for thermocouple in TCs:
            if (thermocouple.last_reading > CRIT_T and Is_Critical == False):
                emergency_shutdown()
                Is_Critical = True  ## this prevents Pi from continuously calling emergency_shutdown() during a critical condition --- just calls it once.
                print('EMERGENCY SHUTDOWN: Critical Temperature detected')
        
        # Adding Pressure Transducer data
        data.extend(pt.readPressureTransducers(PTs))
        
        # Pressure Safety Check! Make sure tank isn't going to explode
        for sensor in PTs:
            if ((sensor.last_reading > CRIT_P) and (Is_Critical == False)):
                emergency_shutdown()
                Is_Critical = True
                print('EMERGENCY SHUTDOWN: Critical Pressure detected')
        # [*TC1 (Top), *TC2 (Bottom), TC3, TC4, TC5, TC6, *PT1 (Top), 
        # *PT2 (Bottom), PT3, *LC1 (NOX mass), LC2 (Thrust), LC3, 
        # *Sol1 (Fueling), *Sol2 (Venting), *Sol3 (Disconnect), 
        # *Sol4 (Reset), *Ballvalve, *Ignition]
        
        data = data + LC_Reader.read_load_cells()
        # Adding whether or not solenoid is fueling
   #[*TC1 (Top), *TC2 (Bottom), TC3, TC4, TC5, TC6, *PT1 (Top), *PT2 (Bottom), PT3, *LC1 (NOX mass), LC2 (Thrust), LC3, *Sol1 (Fueling), *Sol2 (Venting), *Sol3 (Disconnect), *Sol4 (Reset), *Ballvalve, *Ignition]
        data.append(getFuelSolState())
        
        # Adding state of vent valve
        data.append(getVentState())
        
        # Adding disconnect state information
        # data.append(getDisconnectState())
        data.extend(["E[DIS]", "E[RES]"])   # pads out spot that would have been occupied with disconnect/reset states
        
        # Adding ball valve state information
        data.append(getBallValveState())
        
        # Adding ematch state information
        data.append(getEmatchState())
        
        # Return list of datals
        return data
        
    except IOError as e:
        print("!!! Error in colletData:")
        print(str(e))
        return data


########################################################################
### OTHER FUNCTIONS ####################################################
########################################################################

## FIXME MAYBE  Should we have a function to order the arduino to open the ball valve?


# Tell ignition computer to close motorized ball valve.
# Doesn't actually close the valve (that's the igcomp's job) --- hence the name
def emergency_shutdown():
    GPIO.output(EMERG_MBVALVE_SHUTOFF_PIN, True) #this needs to be a actuate ball valve function on its own
    # Vent() # Ask DAAAAAAAN


def Vent():
    """ Tell ignition computer to open venting solenoid
    """
    GPIO.output(VENT_VALVE_SHUTOFF_PIN, True)
    VENTVALVE = True
    return VENTVALVE
     

def getFuelSolState():
    try: 
        return GPIO.input(FUEL_SOLENOID_DETECT_PIN)
    except Exception as e:
        print(str(e))
        print("Error with getFuelSolState")
        return "E[FSS]"

def getVentState():
    try: 
        return GPIO.input(VENT_VALVE_DETECT_PIN)
    except Exception as e:
        print(str(e))
        print("Error with getVentState")
        return "E[VS]"


# def getDisconnectState():
#     return GPIO.input(DISCONNECT_DETECT_PIN)


def getBallValveState():
    try: 
        return GPIO.input(MBVALVE_DETECT_PIN)
    except Exception as e:
        print(str(e))
        print("Error with getBallValveState")
        return "E[BVS]"


def getEmatchState():
    try: 
        return GPIO.input(EMATCH_DETECT_PIN)
    except Exception as e:
        print(str(e))
        print("Error with getEmatchState")
        return "E[EMS]"
    

def getBallValveMovingState():
    """ Checks pin from Arduino indicating if the ball valve is being actuated.
    """
    try: 
        ## return GPIO.input(MBVALVE_ACTUATING_DETECT_PIN)
        return 0
    except Exception as e:
        print(str(e))
        print("Error with getMovingBallValveMovingState")
        return "E[BVMS]"
    

########################################################################
### DATA LOGGING AND TRANSMISSION ######################################
########################################################################


def writedata(data_writer, args):
    """ Write data to file (currently DATA1.csv)
    """
    packet = [datetime.now()]
    packet.extend(args)
    data_writer.writerow(packet)


def main(DATA_READ_INTERVAL=0.01):
    
    ## There used to be an ' import csv ' statement here and ' writer(log) ' was ' csv.writer(log) '
    filename = "data/DATA.csv"
    with open(filename,'w',newline='') as log:
        data_writer = writer(log)

        #Header row so you know what you're looking at (change as necessary)
        data_writer.writerow(['Timestamp','TC1','TC2','TC3','TC4','TC5','TC6','PT1','PT1','PT3','PT4','LC1','LC2','LC3','FuelValve','VentValve','Disconnect','Reset','BallValveState','Ematch', 'BallValveMoving'])
        
        # Initialize sensors
        try:
            init()
        except Exception as e:
            print(e)
            print("Error in initialization")
        
        # Print out a reminder (maybe remove this)
        print("!!!!!!!!!!!!!!!!DO BE AWARE that there is an extra value"
            "printed to the terminal that indicates whether or not the "
            "ball valve is actuating.  This extra value is also "
            "recorded to the Pi's data file.  This extra value IS NOT "
            "TRANSMITTED to the Flask server or GUI!!!!!!!!!!!!!!!!")
        
        
        while True:
            #collect all data here and assign to variables
            #slam all that shite into the writedata function (or append to a list each time a sensor is read)
            #import pdb; pdb.set_trace()
            data = collectData()
            if data is not None:
                try:
                    data.append(getBallValveMovingState())
                    writedata(data_writer, data)
                    sendData(data)
                    print(data)
                except Exception as e:
                    print(str(e))
                    print("!!! Error in main DART loop whilst recording or transmitting data:")
            time.sleep(DATA_READ_INTERVAL)
                
                

if __name__ == "__main__":
    main()

