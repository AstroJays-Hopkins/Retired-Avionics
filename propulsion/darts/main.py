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

class DACCLoop:
    '''
    Logic for data aquision, command, and control from the flight computer side.
    Interfaces with all of the other engine systems and sensors to ensure a safe
    and controlled flight.
    '''
    def __init__(self):
        # Hoopefully we don't start off in critical condition
        global Is_Critical
        self.Is_Critical = False
        ### VARIABLES TO STORE SENSOR OBJECTS ###
        # Data stores one reading from each sensor, dumps it somehwere else and
        # is then reset. This acts like a buffer/shovel between sensors and storage
        self.data = []
        # Where load cell objects are stored
        # To access the last reading of an individual load cell i,
        # use LOAD_CELLS[i].last_reading
        self.LC_Reader = None
        # Place to put TC objects
        self.TC_Reader = None
        # Place to put PT objects
        self.PT_Reader = None
        # Engine controller interface
        self.ECI = None


    def init(self):
        '''
        Two phase init for greater flexibility. Consider consolidating everything
        into the constructor if this flexibility is unecessary.
        '''
        self.LC_Reader = LoadCellReader(["/dev/usbLC0", "/dev/usbLC1", "/dev/usbLC2"])
        # Initialize thermocouples
        print(Const.TC_CS_PINS)
        self.TC_Reader = ThermocoupleReader(Const.TC_CS_PINS)
        # Initialize PTs
        print(Const.PT_CHANNELS)
        self.PT_Reader = PressureTransducerReader(Const.PT_CHANNELS)
        # initialize engine controller communication interface
        self.ECI = EngineController()

    def collectData(self):
        """ Iterates through all sensors and collects data
        """
        try:
            # Initialize data array
            self.data = []

            #global Is_Critical
            # See here for why we need the above line: https://stackoverflow.com/questions/10851906/python-3-unboundlocalerror-local-variable-referenced-before-assignment#10852003

            # Adding thermocouple data
            tempList = self.TC_Reader.readTCs()
            self.data.extend(tempList)

            # Temperature Safety Check! Make sure tank isn't going to explode
            for temp in tempList:
                if (isinstance(temp, float) and temp > Const.CRIT_T and Is_Critical == False):
                    self.ECI.emergency_shutdown()
                    Is_Critical = True  ## this prevents Pi from continuously calling emergency_shutdown() during a critical condition --- just calls it once.
                    print('EMERGENCY SHUTDOWN: Critical Temperature detected')

            # Adding Pressure Transducer data
            pressureList = self.PT_Reader.readPTs()
            self.data.extend(pressureList)

            # Pressure Safety Check! Make sure tank isn't going to explode
            for pressure in pressureList:
                if (isinstance(pressure, float) and pressure > Const.CRIT_P and (Is_Critical == False)):
                    self.ECI.emergency_shutdown()
                    Is_Critical = True
                    print('EMERGENCY SHUTDOWN: Critical Pressure detected')
            # [*TC1 (Top), *TC2 (Bottom), TC3, TC4, TC5, TC6, *PT1 (Top),
            # *PT2 (Bottom), PT3, *LC1 (NOX mass), LC2 (Thrust), LC3,
            # *Sol1 (Fueling), *Sol2 (Venting), *Sol3 (Disconnect),
            # *Sol4 (Reset), *Ballvalve, *Ignition]

            self.data = self.data + self.LC_Reader.read_load_cells()
            # Adding whether or not solenoid is fueling
       #[*TC1 (Top), *TC2 (Bottom), TC3, TC4, TC5, TC6, *PT1 (Top), *PT2 (Bottom), PT3, *LC1 (NOX mass), LC2 (Thrust), LC3, *Sol1 (Fueling), *Sol2 (Venting), *Sol3 (Disconnect), *Sol4 (Reset), *Ballvalve, *Ignition]
            self.data.append(self.ECI.getFuelSolState())

            # Adding state of vent valve
            self.data.append(self.ECI.getVentState())

            # Adding disconnect state information
            # data.append(getDisconnectState())
            self.data.extend(["E[DIS]", "E[RES]"])   # pads out spot that would have been occupied with disconnect/reset states

            # Adding ball valve state information
            self.data.append(self.ECI.getBallValveState())

            # Adding ematch state information
            self.data.append(self.ECI.getEmatchState())
            # add ball valve movement state info
            self.data.append(self.ECI.getBallValveMovingState())

            # Return list of datals
            return self.data

        except IOError as e:
            print("!!! Error in colletData:")
            print(str(e))
            return self.data




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
    loop = DACCLoop()
    ## There used to be an ' import csv ' statement here and ' writer(log) ' was ' csv.writer(log) '
    filename = "data/DATA.csv"
    with open(filename,'w',newline='') as log:
        data_writer = writer(log)

        #Header row so you know what you're looking at (change as necessary)
        data_writer.writerow(['Timestamp','TC1','TC2','TC3','TC4','TC5','TC6','PT1','PT1','PT3','PT4','LC1','LC2','LC3','FuelValve','VentValve','Disconnect','Reset','BallValveState','Ematch', 'BallValveMoving'])
        
        # Initialize sensors
        try:
            loop.init()
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
            data = loop.collectData()
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

