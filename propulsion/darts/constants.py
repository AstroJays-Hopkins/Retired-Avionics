from adafruit_ads1x15.ads1115 import P0,P1,P2,P3
import board

class Const:
    '''
    Standard constant for the darts system.
    '''
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
    # TC_CS_PINS = [board.D17,board.D27]
    # LC_SEL_TUPLES = ((0, 0), None)

    # Set critical values
    CRIT_T = 309.5 # Critical Temperature in Kelvin
    CRIT_P = 7240.0 # Critical pressure in ??????????
