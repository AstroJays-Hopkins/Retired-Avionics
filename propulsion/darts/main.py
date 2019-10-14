## LAST UPDATE 04/05/2019 AT 1:45 AM ##
## THIS IS UNFINISHED CODE PLS HELP ? ##
## TODO LIST: ##

from datetime import datetime
from csv import writer
import threading
import time
from constants import Const
from engine_controller import EngineController
from load_cell import LoadCellReader
from RocketThermocouple import ThermocoupleReader
from pressure_transducer import PressureTransducerReader
from SendToFlask import sendData
## import Server   # doesn't work


### LIST OF UNDEFINED VARIABLES, CONSTANTS --- UPDATE as NEEDED ###
## --> No undefined variables  :)


# Hoopefully we don't start off in critical condition                                                       
global Is_Critical
Is_Critical = False


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
TC_Reader = None

# Place to put PT objects
PT_Reader = None

# Engine controller interface
ECI = None


def init():
    global LC_Reader
    LC_Reader = LoadCellReader(["/dev/usbLC0", "/dev/usbLC1", "/dev/usbLC2"])
    
    # Initialize thermocouples
    print(Const.TC_CS_PINS)
    global TC_Reader
    TC_Reader = ThermocoupleReader(Const.TC_CS_PINS)

    # Initialize PTs
    print(Const.PT_CHANNELS)
    global PT_Reader
    PT_Reader = PressureTransducerReader(Const.PT_CHANNELS)

    # initialize engine controller communication interface
    global ECI
    ECI = EngineController()

def collectData():
    """ Iterates through all sensors and collects data
    """
    try:
        # Initialize data array
        data = []
        
        #global Is_Critical
        # See here for why we need the above line: https://stackoverflow.com/questions/10851906/python-3-unboundlocalerror-local-variable-referenced-before-assignment#10852003
        
        # Adding thermocouple data
        tempList = TC_Reader.readTCs()
        data.extend(tempList)
        
        # Temperature Safety Check! Make sure tank isn't going to explode
        for temp in tempList:
            if (isinstance(temp, float) and temp > Const.CRIT_T and Is_Critical == False):
                ECI.emergency_shutdown()
                Is_Critical = True  ## this prevents Pi from continuously calling emergency_shutdown() during a critical condition --- just calls it once.
                print('EMERGENCY SHUTDOWN: Critical Temperature detected')
        
        # Adding Pressure Transducer data
        pressureList = PT_Reader.readPTs()
        data.extend(pressureList)
        
        # Pressure Safety Check! Make sure tank isn't going to explode
        for pressure in pressureList:
            if (isinstance(pressure, float) and pressure > Const.CRIT_P and (Is_Critical == False)):
                ECI.emergency_shutdown()
                Is_Critical = True
                print('EMERGENCY SHUTDOWN: Critical Pressure detected')
        # [*TC1 (Top), *TC2 (Bottom), TC3, TC4, TC5, TC6, *PT1 (Top), 
        # *PT2 (Bottom), PT3, *LC1 (NOX mass), LC2 (Thrust), LC3, 
        # *Sol1 (Fueling), *Sol2 (Venting), *Sol3 (Disconnect), 
        # *Sol4 (Reset), *Ballvalve, *Ignition]
        
        data = data + LC_Reader.read_load_cells()
        # Adding whether or not solenoid is fueling
   #[*TC1 (Top), *TC2 (Bottom), TC3, TC4, TC5, TC6, *PT1 (Top), *PT2 (Bottom), PT3, *LC1 (NOX mass), LC2 (Thrust), LC3, *Sol1 (Fueling), *Sol2 (Venting), *Sol3 (Disconnect), *Sol4 (Reset), *Ballvalve, *Ignition]
        data.append(ECI.getFuelSolState())
        
        # Adding state of vent valve
        data.append(ECI.getVentState())
        
        # Adding disconnect state information
        # data.append(getDisconnectState())
        data.extend(["E[DIS]", "E[RES]"])   # pads out spot that would have been occupied with disconnect/reset states
        
        # Adding ball valve state information
        data.append(ECI.getBallValveState())
        
        # Adding ematch state information
        data.append(ECI.getEmatchState())
        # add ball valve movement state info
        data.append(ECI.getBallValveMovingState())
        
        # Return list of datals
        return data
        
    except IOError as e:
        print("!!! Error in colletData:")
        print(str(e))
        return data


   

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
                    writedata(data_writer, data)
                    sendData(data)
                    print(data)
                except Exception as e:
                    print(str(e))
                    print("!!! Error in main DART loop whilst recording or transmitting data:")
            time.sleep(DATA_READ_INTERVAL)
                
                

if __name__ == "__main__":
    main()

