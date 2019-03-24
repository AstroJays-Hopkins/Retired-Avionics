#!/usr/bin/python
# Author: Courtney Schmitt 
# Date Created: 03/17/2019
#
# Last Modified: 03/17/2019 
#
# Creates object RocketThermocouple of Adafruit MAX31855 type extends readTempC
# Iterates through TC objects and takes each measurement, stores to list and returns# the list of temperature readings.

import time

import Adafruit_GPIO.SPI as SPI
import Adafruit_MAX31855.MAX31855 as MAX31855


# Define a function to convert celsius to fahrenheit.
def c_to_f(c):
        return c * 9.0 / 5.0 + 32.0



# Raspberry Pi software SPI configuration.
CLK = 25
CS  = 24
DO  = 18
sensor = MAX31855.MAX31855(CLK, CS, DO)

# Raspberry Pi hardware SPI configuration.
#SPI_PORT   = 0
#SPI_DEVICE = 0
#sensor = MAX31855.MAX31855(spi=SPI.SpiDev(SPI_PORT, SPI_DEVICE))


# Define a function that returns the list of thermocouple measurements 
def readThermocouples():
    for each (RocketThermocouple sensor in rocketThermocouples) {
        data[i] = sensor.readTempC()
    }
    return data 