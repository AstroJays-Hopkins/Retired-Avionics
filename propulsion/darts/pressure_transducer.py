import board
import busio
import adafruit_ads1x15.ads1115 as ADS
from adafruit_ads1x15.analog_in import AnalogIn


class PressureTransducer:
    def __init__(self, Global_ADS, adc_channel):
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


class PressureTransducerReader:
    '''
    Batch reader for more than one pressure transducer. You probably want to
    use this.
    '''
    def __init__(self, adc_channels):
        '''
        Initialize i2c bus and create one PressureTransducer object per channel
        specified in pt_channels

        :param pt_channels: list of PT channels to use. Note that read will be
                            returned in this order
        '''
        self.PTs = []
        try:
            # Create the I2C bus
            self.i2c = busio.I2C(board.SCL, board.SDA)
            # Create the ADC object using the I2C bus
            self.ADS = ADS.ADS1115(self.i2c)
        except Exception as e:
            print(e)
        for channel in adc_channels:
            try:
                self.PTs.append(PressureTransducer(self.ADS, channel))
            except Exception as e:
                print(e)
                # kludge to take care of case where specified channel has no PT
                self.PTs.append(None)

    def readPTs(self):
        '''
        Read PTs specified when this reader class was initialized.

        :return: List of PT readings in order specified when this class was
                 initialized
        '''
        data = []
        for PT in self.PTs:
            try:
                data.append(PT.readPressure())
            except Exception as e:
                data.append("E[PT]")
                print(str(e))
            # data.append(sensor.chan.voltage)
            # data.append(sensor.chan.value)
        return data
