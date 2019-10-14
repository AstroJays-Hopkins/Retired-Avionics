# Moved from `main.py` Check that file for version control history.
# import general purpose input/output
try:
    # RPi.GPIO documentation: https://sourceforge.net/p/raspberry-gpio-python/wiki/
    import RPi.GPIO as GPIO
except ImportError:
    print("Error importing RPi.GPIO! This is probably because you need"
          "superuser privileges.  You can achieve this by using 'sudo'"
          "to run your script")

from constants import Const

class EngineController:
    '''
    Class to interface with the ignition computer. currently an Arduino Mega,
    but subject to change. This is where we cross into the firm-time bound, so
    all communications methods chosen should be time-definite.

    NOTE: This "coprocessor" is also known as the "Engine Controller"
    '''
    ## FIXME MAYBE  Should we have a function to order the arduino to open the ball valve?


    # Tell ignition computer to close motorized ball valve.
    # Doesn't actually close the valve (that's the igcomp's job) --- hence the name
    def __init__(self):
        '''
        Initialize GPIOs and communication mechnisms used by the ignition
        computer to interface with the flight computer.

        See `constants.py` for a unified listing of all of these pins
        '''
        # Configure GPIO pin for telling the ignition computer to close the motorized ball valve in an emergency:
        GPIO.setup(Const.EMERG_MBVALVE_SHUTOFF_PIN, GPIO.OUT) # Makes ball-valve pin an output pin
        GPIO.output(Const.EMERG_MBVALVE_SHUTOFF_PIN, False) # Set pin outpuit to be false
        GPIO.setup(Const.MBVALVE_DETECT_PIN, GPIO.IN) # Read position of ball valve (open or close)

        # Configure vent GPIO pins
        GPIO.setup(Const.VENT_VALVE_DETECT_PIN, GPIO.IN) # Read position of vent valve (open or close)
        GPIO.setup(Const.FUEL_SOLENOID_DETECT_PIN, GPIO.IN) # Read whether opr not we are fueling (open or close)
        GPIO.setup(Const.EMATCH_DETECT_PIN, GPIO.IN) # Read if Ematch has or hasn't been lit

        ## GPIO.setup(MBVALVE_ACTUATING_DETECT_PIN, GPIO.IN)  ## USELESS?  Check belowdecks   avnoks  12 May 2019
        # GPIO.setup(DISCONNECT_DETECT_PIN, GPIO.IN)
        self.ventvalve = False

    def emergency_shutdown(self):
        GPIO.output(Const.EMERG_MBVALVE_SHUTOFF_PIN, True) #this needs to be a actuate ball valve function on its own
        # Vent() # Ask DAAAAAAAN

    def Vent(self):
        """ Tell ignition computer to open venting solenoid
        """
        GPIO.output(Const.VENT_VALVE_SHUTOFF_PIN, True)
        self.ventvalve = true
        return self.ventvalve

    def getFuelSolState(self):
        try:
            return GPIO.input(Const.FUEL_SOLENOID_DETECT_PIN)
        except Exception as e:
            print(str(e))
            print("Error with getFuelSolState")
            return "E[FSS]"

    def getVentState(self):
        try:
            return GPIO.input(Const.VENT_VALVE_DETECT_PIN)
        except Exception as e:
            print(str(e))
            print("Error with getVentState")
            return "E[VS]"

    def getBallValveState(self):
        try:
            return GPIO.input(Const.MBVALVE_DETECT_PIN)
        except Exception as e:
            print(str(e))
            print("Error with getBallValveState")
            return "E[BVS]"

    def getEmatchState(self):
        try:
            return GPIO.input(Const.EMATCH_DETECT_PIN)
        except Exception as e:
            print(str(e))
            print("Error with getEmatchState")
            return "E[EMS]"

    def getBallValveMovingState(self):
        """ Checks pin from Arduino indicating if the ball valve is being actuated.
        """
        try:
            ## return GPIO.input(MBVALVE_ACTUATING_DETECT_PIN)
            return 0
        except Exception as e:
            print(str(e))
            print("Error with getMovingBallValveMovingState")
            return "E[BVMS]"
