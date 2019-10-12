import time
from serial import Serial, SerialException  # PySerial documentation: https://pythonhosted.org/pyserial/index.html


# There is no Adafruit library for load cells, so there is no preexisting
# load cell class to subclass.  I've still made my own below to keep
# everything organized.
class LoadCell:
    '''
    This class to describes a single loadcell. If batch reading is required,
    the `LoadCellReader` class should probably be used instead.
    '''
    def __init__(self, serport, timeout):
        '''
        Initialize serial connection to the Openscale using the specified
        parameters.

        :param serport: String path to the serial device to use to communicate
                        with the openscale
        :param timeout: amount of time to wait for new data while reading. Note
                        that if the program is single threaded this will cause
                        the entire program to hang for that amount of time if
                        there is no new data
        '''
        self.last_read = None
        try:
            self.loadcell = Serial(serport, baudrate=115200, timeout=timeout)
            self.read_and_verify()
        except(OSError, SerialException):
            # kludge, please handle exceptions properly
            self.loadcell = None
            print ("Loadcell at {} did not connect".format(serport))

    # empty buffer until good data is present
    def read_and_verify(self):
        '''
        Read a line off the serial port and verify it's weight data. If it's nto
        weight data, read until weight data is returned.

        :return: floating point number of the current measured weight on scale
        '''
        line  = ''
        # discard all data that doesn't validate
        while len(line.split(',')) != 4:
            line = self.loadcell.readline().decode('utf-8')
        return float(line.split(',')[1])

    def _clear_buffer(self):
        '''
        Clears the serial read buffer
        '''
        while self.loadcell.in_waiting != 0:
            self.loadcell.read()

    def read_weight(self):
        '''
        Read the current weight transmitted over serial, or return the last
        weight read if there is no new data. Note that this only works when the
        read loop is faster than the serial transmit rate, otherwise the buffers
        will fill and data will get out of sync.

        :return: Current weight on scale if the openscale transmitted new data,
                 otherwise, the last transmitted weight.
        '''
        # if the loadloadcell serial object doesn't exist, immediately return an
        # error. This means that initialization failed probably because the LC
        # wasn't connected
        if not self.loadcell:
            return "E[LC]"
        # only collect data if there's something new on the port. No sense
        # blocking until we get a read if we haven't been sent anything
        # TODO: async function?
        if self.loadcell.in_waiting != 0:
            try:
                self._clear_buffer()
                self.last_read = self.read_and_verify()
            except (OSError, SerialError):
                print("Failed to read loadcell at {}".format(self.loadcell.port))
                self.last_read = "E[LC]"
        return self.last_read

class LoadCellReader:
    '''
    This class reads an arbitrary number of serial loadcells based on a
    preconfigured lsit of serial ports to monitor.
    '''
    def __init__(self, serports, timeout=0.1):
        '''
        Initialize a new Load_Cell_Reader object to read the load cells
        connected via serial interfaces.

        :param serports: list of serial port devices to connect to
        :param timeout: amount of tiem to wait for new data when reading the
                        serial port
        '''
        self.loadcells = []
        for port in serports:
            self.loadcells.append(LoadCell(port, timeout))

    def read_load_cells(self):
        '''
        Read all load cells specified when creating this reader object

        :return: list of loadcell reading in order they were initially specified
        '''
        weights = []
        for lc in self.loadcells:
            weights.append(lc.read_weight())
        return weights
