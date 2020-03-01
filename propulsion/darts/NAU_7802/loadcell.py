import board
import busio
import digitalio


class LoadCell:
    registers = {"NAU7802_ADCO_B2": 0x12,
                 }

    def __init__(self, SCL, SDA, add=0x2A):
        self.address = 0x2A
        self.SCL = SCL
        self.SDA = SDA
        self.i2c_port = busio.I2C(digitalio.DigitalInOut(SCL), digitalio.DigitalInOut(SDA))

    def read_raw(self):
        buffer = [0]
        cur_data = 0
        if not self.available():
            return None
        try:
            self.i2c_port.writeto(self.address, self.registers["NAU7802_ADCO_B2"], stop=True)
        except:
            return 0

        for i in range(3):
            self.i2c_port.readfrom(self.address, buffer)
            cur_data |= (buffer[0] << 8)
        return cur_data

    def available(self):
        return None
