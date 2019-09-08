import board
import busio
import adafruit_ads1x15.ads1115 as ADS
from adafruit_ads1x15.analog_in import AnalogIn

# Create the I2C bus
i2c = busio.I2C(board.SCL, board.SDA)

# Create the ADC object using the I2C bus
Global_ADS = ADS.ADS1115(i2c)

# Global_ADS.gain = 2

class PressureTransducer:
    def __init__(self, adc_channel):
        self.chan = AnalogIn(Global_ADS, adc_channel)
        self.last_reading = -1
    # PT produces maximum of 4.5V at 5076.32 PSI
    # and minimum of 0.5V at 0 PSI
    def readPressure(self):
        try:
            self.last_reading = (self.chan.voltage - 0.5) * (5076.32 / 4)
            return self.last_reading
        except Exception as e:
            print("!!! Error whilst reading PTs:")
            print(str(e))
            print("Returning \"E\"")
            return "E"


def readPressureTransducers(PTArray):
    data = []
    # for sensor in PTArray:
    for sensor in range(4):
        # data.append(sensor.readPressure())
        data.append("E [PT]")
        
        
        # data.append(sensor.chan.voltage)
        # data.append(sensor.chan.value)
    return data 
