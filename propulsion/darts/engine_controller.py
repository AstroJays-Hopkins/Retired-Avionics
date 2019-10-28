# Moved from `main.py` Check that file for version control history.
# import general purpose input/output
try:
    # RPi.GPIO documentation: https://sourceforge.net/p/raspberry-gpio-python/wiki/
    import RPi.GPIO as GPIO
except ImportError:
    print("Error importing RPi.GPIO! This is probably because you need"
          "superuser privileges.  You can achieve this by using 'sudo'"
          "to run your script")
from adafruit_bus_device.i2c_device import I2CDevice

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
    def __init__(self, i2c):
        '''
        Initialize mechanisms used by the ignition computer to interface with
        the flight computer.

        Right now, the coprocessors communicate over I2C due to the short rnage
        involved
        '''
        try:
            self._sc_device = I2CDevice(i2c, Const.I2C_ADDR_SC)
        except ValueError:
            print("[ECI] Error initializing SC device")
        try:
            self._ec_device = I2CDevice(i2c, Const.I2C_ADDR_EC)
        except ValueError:
            print("[ECI] Error initializing EC device")
        self._buf = bytearray(1)
        self.updateFailed = False
        self.SCVState = False
        self.ecCommand = 0b0
        self.state = {'MV_S1': None,
                      'MV_R1': None,
                      'MV_R1_moving': None,
                      'MV_G1': None,
                      'eMatch': None}

    def _writeI2C(self, device):
        '''
        Write the instance buffer _buf to the I2C device at `device`

        :param device: I2C device to write out to
        '''
        try:
            with device as dev:
                dev.write(self._buf)
        except Exception as e:
            print(str(e))
            print("[ECI] Error writing to device at address"
                  "{}".format(device.device_address))

    def emergency_shutdown(self):
        '''
        Carry out procedures needed to make the engine shut down
        '''
        # Vent() # Ask DAAAAAAAN
        self._buf[0] = 0b1
        self._writeI2C(self._ec_device)

    def setVent(self, state):
        """
        Tell ignition computer to open venting solenoid

        :param state: True to open the vent velve, False to close
        """
        if state == true:
            self.ecCommand |= 0b10 << 1
        if state == false:
            self.ecCommand &= ~(0b11 << 1)
            self.ecCommand |= 0b01 << 1 
        self._buf[0] = self.ecCommand
        self._writeI2C(self._ec_device)
        self.ecCommand &= ~(0b11 << 1)

    def update(self):
        '''
        Requests current engine and engine actuator states from the engine
        controller and buffers it in the state variable.

        This function should be called in every cycle before the "get" functions
        are called.
        '''
        self._buf[0] = 0
        try:
            with self._ec_device as dev:
                dev.readinto(self._buf)
        except Exception as e:
            print(str(e))
            print("Error with updating engine data")
            self.updateFailed = True

        self.state['MV_G1']        = bool((self._buf[0] >> 4) & 0b1)
        self.state['eMatch']       = bool((self._buf[0] >> 3) & 0b1)
        self.state['MV_R1']        = bool((self._buf[0] >> 2) & 0b1)
        self.state['MV_R1_moving'] = bool((self._buf[0] >> 1) & 0b1)
        self.state['MV_S1']        = bool(self._buf[0] & 0b1)

        if(self.state['MV_G1'] != self.SCVState):
            self._buf[0] = int(self.state['MV_G1']) + 1
            self._writeI2C(self.sc_device)
            self.SCVState = self.state['MV_G1']

    def getFuelSolState(self):
        '''
        Get the last state of the fueling valve. Like all valves, true is open
        and false is closed.
        '''
        if(self.updateFailed):
            return "E[FSS]"
        return self.state['MV_G1']

    def getVentState(self):
        '''
        Get the last state if the venting solenoid. Like all valves, true is
        open and false is closed
        '''
        if(self.updateFailed):
            return "E[VS]"
        return self.state['MV_S1']

    def getBallValveState(self):
        '''
        Get the last state of the rocket valve MV-R1. Like all valves, true is
        open and false is closed.
        '''
        if(self.updateFailed):
            return "E[BVS]"
        return self.state['MV_R1']

    def getEmatchState(self):
        if(self.updateFailed):
            return "E[EMS]"
        return self.state['eMatch']

    def getBallValveMovingState(self):
        """ Checks pin from Arduino indicating if the ball valve is being actuated.
        """
        if(self.updateFailed):
            return "E[BVMS]"
        return self.state['MV_R1_moving']
