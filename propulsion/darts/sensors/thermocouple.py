#!/usr/bin/python
# Author: Courtney Schmitt
# Date Created: 03/17/2019
#
# Last Modified: 03/17/2019
#
# Creates object RocketThermocouple of Adafruit MAX31855 type extends readTempC
# Iterates through TC objects and takes each measurement, stores to list and returns# the list of temperature readings.

import board
import busio
import digitalio
import Adafruit_GPIO.SPI as SPI
import adafruit_max31855 as MAX31855


# Raspberry Pi software SPI configuration.
CLK = 25
CS  = 24
DO  = 18
# sensor = MAX31855.MAX31855(CLK, CS, DO)


# Raspberry Pi hardware SPI configuration.
#SPI_PORT   = 0
#SPI_DEVICE = 0
#sensor = MAX31855.MAX31855(spi=SPI.SpiDev(SPI_PORT, SPI_DEVICE))


class Thermocouple(MAX31855.MAX31855):
    def __init__(self, cs_pin, spi):
        super(Thermocouple, self).__init__(spi, digitalio.DigitalInOut(cs_pin))
        self.last_reading = self.temperature

    def readTempC(self):
        try:
            self.last_reading = self.temperature  # MAX31855.MAX31855.readTempC(self)
            return self.last_reading
        except Exception as e:
            print("!!! Error while reading TC: " + str(e))
            ## FIXME: What if the temperature was freezing?  Then -1
            # could be a valid temperature as well as an indicator of
            # an error...
            return "E"

class ThermocoupleReader():
    '''
    Class to be used for reading batches of thermocouples
    '''
    def __init__(self, tc_cs_pins):
        '''
        Initialize SPI bus and create thermocouple objects to prepare to read

        :param tc_cs_list: list of cs pins to use to select thermocouples.
                           The read output will be returned in this order
        '''
        self.spi = busio.SPI(board.SCK, MOSI=board.MOSI, MISO=board.MISO)
        self.TCs = []
        for pin in tc_cs_pins:
            try:
                self.TCs.append(Thermocouple(pin, self.spi))
            except Exception as e:
                print(e)
                # Kludge for when the TC doesn't exist
                self.TCs.append(None)

    def readTCs(self):
        '''
        Read Thermocouples in order specified in __init__

        :return: List of thermocouple readings in degC in smae order specified
                 in the tc_cs_pins argument of __init__
        '''
        data = []
        for TC in self.TCs:
            try:
                data.append(TC.readTempC())
            except Exception:
                data.append('E[TC]')
        return data

    def getLastReadings(self):
        '''
        Get the last readings from the TCs as a list in order specified
        when the object was created

        :return: list of previous thermocouple readings in degC
        '''
        data = []
        for TC in self.TCs:
            data.append(TC.last_reading)
        return data

def begin():
    """ returns SPI object to be passed into TC initialization method
        when creating thermocouple objects
    """
    spi = busio.SPI(board.SCK, MOSI=board.MOSI, MISO=board.MISO)
    return spi


def c_to_f(c):
    """ convert celsius to fahrenheit.
    """
    return (c * 9.0 / 5.0 + 32.0)
