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

def begin():
    """ returns SPI object to be passed into TC initialization method 
        when creating thermocouple objects
    """
    spi = busio.SPI(board.SCK, MOSI=board.MOSI, MISO=board.MISO)
    return spi


def c_to_f(c):
    """ convert celsius to fahrenheit.
    """
    return c * 9.0 / 5.0 + 32.0


# Define a function that returns the list of thermocouple measurements 
def readThermocouples(rocketThermocouples):
    data = []
    for sensor in rocketThermocouples:
        data.append(sensor.readTempC())
    return data 
