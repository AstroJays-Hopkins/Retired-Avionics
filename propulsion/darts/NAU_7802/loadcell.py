import time
import board
import busio
import digitalio


class LDOValues:
    NAU7802_LDO_2V4 = 0b111
    NAU7802_LDO_2V7 = 0b110
    NAU7802_LDO_3V0 = 0b101
    NAU7802_LDO_3V3 = 0b100
    NAU7802_LDO_3V6 = 0b011
    NAU7802_LDO_3V9 = 0b010
    NAU7802_LDO_4V2 = 0b001
    NAU7802_LDO_4V5 = 0b000


class Registers:
    NAU7802_PU_CTRL = 0x00
    NAU7802_CTRL1 = 0x01
    NAU7802_CTRL2 = 0x02
    NAU7802_ADCO_B2 = 0x12
    NAU7802_ADC = 0x15
    NAU7802_PGA_PWR = 0x1C


class Bits:
    a = 0


class PuCTRLBits(Bits):
    NAU7802_PU_CTRL_RR = 0
    NAU7802_PU_CTRL_PUD = 1
    NAU7802_PU_CTRL_PUA = 2
    NAU7802_PU_CTRL_PUR = 3
    NAU7802_PU_CTRL_CS = 4
    NAU7802_PU_CTRL_CR = 5
    NAU7802_PU_CTRL_OSCS = 6
    NAU7802_PU_CTRL_AVDDS = 7


class Ctrl1Bits(Bits):
    NAU7802_CTRL1_GAIN = 2
    NAU7802_CTRL1_VLDO = 5
    NAU7802_CTRL1_DRDY_SEL = 6
    NAU7802_CTRL1_CRP = 7


class Ctrl2Bits(Bits):
    NAU7802_CTRL2_CALMOD = 0
    NAU7802_CTRL2_CALS = 2
    NAU7802_CTRL2_CAL_ERROR = 3
    NAU7802_CTRL2_CRS = 4
    NAU7802_CTRL2_CHS = 7


class PgaPwrBits(Bits):
    NAU7802_PGA_PWR_PGA_CURR = 0
    NAU7802_PGA_PWR_ADC_CURR = 2
    NAU7802_PGA_PWR_MSTR_BIAS_CURR = 4
    NAU7802_PGA_PWR_PGA_CAP_EN = 7


class SampleRates:
    NAU7802_SPS_320 = 0b111
    NAU7802_SPS_80 = 0b011
    NAU7802_SPS_40 = 0b010
    NAU7802_SPS_20 = 0b001
    NAU7802_SPS_10 = 0b000


class GainValues:
    NAU7802_GAIN_128 = 0b111
    NAU7802_GAIN_64 = 0b110
    NAU7802_GAIN_32 = 0b101
    NAU7802_GAIN_16 = 0b100
    NAU7802_GAIN_8 = 0b011
    NAU7802_GAIN_4 = 0b010
    NAU7802_GAIN_2 = 0b001
    NAU7802_GAIN_1 = 0b000


class LoadCell:
    registers = Registers()
    gain_values = GainValues()
    LDO_values = LDOValues()
    PU_CTRL_bits = PuCTRLBits()
    CTRL1_bits = PuCTRLBits()
    CTRL2_bits = Ctrl2Bits()
    PGA_PWR_bits = PgaPwrBits()
    sample_rates = SampleRates()
    """
    PU_CTRL_Bits = {
        "NAU7802_PU_CTRL_RR": 0,
        "NAU7802_PU_CTRL_PUD": 1,
        "NAU7802_PU_CTRL_PUA": 2,
        "NAU7802_PU_CTRL_PUR": 3,
        "NAU7802_PU_CTRL_CS": 4,
        "NAU7802_PU_CTRL_CR": 5,
        "NAU7802_PU_CTRL_OSCS": 6,
        "NAU7802_PU_CTRL_AVDDS": 7,
    }

    CTRL1_bits = {
        "NAU7802_CTRL1_GAIN": 2,
        "NAU7802_CTRL1_VLDO": 5,
        "NAU7802_CTRL1_DRDY_SEL": 6,
        "NAU7802_CTRL1_CRP ": 7,
    }

    CTRL2_bits = {
        "NAU7802_CTRL2_CALMOD": 0,
        "NAU7802_CTRL2_CALS": 2,
        "NAU7802_CTRL2_CAL_ERROR": 3,
        "NAU7802_CTRL2_CRS": 4,
        "NAU7802_CTRL2_CHS": 7,
    }
    """

    def __init__(self, SCL, SDA, add=0x2A, init=True):
        self.address = add
        self.SCL = SCL
        self.SDA = SDA
        self.i2c_port = busio.I2C(digitalio.DigitalInOut(SCL), digitalio.DigitalInOut(SDA))
        self.offset0 = 0
        self.cal_factor = 0

        result = 1
        if init:
            result &= ~(self.reset())
            result &= ~(self.power_up())
            result &= ~(self.set_LDO(self.LDO_values.NAU7802_LDO_3V3))
            result &= ~(self.set_gain(self.gain_values.NAU7802_GAIN_128))
            result &= ~(self.set_sample_rate(self.sample_rates.NAU7802_SPS_80))
            result &= ~(self.write_register(self.registers.NAU7802_ADC, 0x30))
            result &= ~(self.set_bit(self.registers.NAU7802_PGA_PWR, self.PGA_PWR_bits.NAU7802_PGA_PWR_PGA_CAP_EN))

    def available(self):
        return self.get_bit(self.registers.NAU7802_PU_CTRL, self.PU_CTRL_bits.NAU7802_PU_CTRL_CR)

    def set_channel(self, chan):
        """
        Takes in a channel number, 1/2, and sets relevant channels.
        :param chan: int
        :return:
        """
        if chan == 2:
            self.set_bit(self.registers.NAU7802_CTRL2, self.CTRL2_bits.NAU7802_CTRL2_CHS)
        else:
            self.clear_bit(self.registers.NAU7802_CTRL2, self.CTRL2_bits.NAU7802_CTRL2_CHS)

    def reset(self):
        self.set_bit(self.registers.NAU7802_PU_CTRL, self.PU_CTRL_bits.NAU7802_PU_CTRL_RR)
        time.sleep(1)
        return self.clear_bit(self.registers.NAU7802_PU_CTRL, self.PU_CTRL_bits.NAU7802_PU_CTRL_RR)

    def power_up(self):
        """
        Power up the chip.
        :return:
        """
        self.set_bit(self.registers.NAU7802_PU_CTRL, self.PU_CTRL_bits.NAU7802_PU_CTRL_PUD)
        self.set_bit(self.registers.NAU7802_PU_CTRL, self.PU_CTRL_bits.NAU7802_PU_CTRL_PUA)

        # Waiting for powering up to be set properly.
        count = 0
        while self.get_bit(self.registers.NAU7802_PU_CTRL, self.PU_CTRL_bits.NAU7802_PU_CTRL_PUR) != 1:
            count += 1
            if count > 1000:
                return 1
        return 0

    def power_down(self):
        self.set_bit(self.registers.NAU7802_PU_CTRL, self.PU_CTRL_bits.NAU7802_PU_CTRL_PUD)
        self.set_bit(self.registers.NAU7802_PU_CTRL, self.PU_CTRL_bits.NAU7802_PU_CTRL_PUA)

    def set_LDO(self, ldo):
        if ldo > 0b111:
            ldo = 0b111
        val = self.read_register(self.registers.NAU7802_CTRL1)
        val &= 0b11000111
        val |= ldo << 3
        self.write_register(self.registers.NAU7802_CTRL1, val)
        return self.set_bit(self.registers.NAU7802_PU_CTRL, self.PU_CTRL_bits.NAU7802_PU_CTRL_AVDDS)

    def set_gain(self, gain):
        if gain > 0b111:
            gain = 0b111
        val = self.read_register(self.registers.NAU7802_CTRL1)
        val &= 0b11111000
        val |= gain
        return self.write_register(self.registers.NAU7802_CTRL1, val)

    def set_sample_rate(self, rate):
        if rate > 0b111:
            rate = 0b111
        val = self.read_register(self.registers.NAU7802_CTRL2)
        val &= 0b10001111
        val |= rate << 4
        return self.write_register(self.registers.NAU7802_CTRL2, val)

    def read_raw(self):
        cur_data = self.read_register(self.registers.NAU7802_ADCO_B2, 3)
        return cur_data

    def read_register(self, reg_1, b_size=1):
        """
        If b_size = 1, reads the value stored in a register.
        Otherwise, reads a sequence of registers incrementally, incrementally, starting with reg_1.
        The bytes from all registers is treated collectively, where the register with the lowest address is
        the most significant byte.
        :param reg_1: string
        :param b_size: int
        :return: int
        """
        buffer = bytearray(b_size)
        cur_data = 0
        if not self.available():
            return None
        try:
            self.i2c_port.writeto(self.address, bytearray([reg_1]), stop=True)
        except:
            return 1
        self.i2c_port.readfrom_into(self.address, buffer)
        for i in range(3):
            cur_data |= (buffer[3-1-i] << 8*i)  # convert value taken separately from the 3 registers into a single int
        return cur_data

    def write_register(self, reg_1, byte):
        """
        Writes a sequence of bytes to a sequence registers incrementally, starting with reg_1,
        :param reg_1: int
        :param byte: byte
        :return: integer
        """
        try:
            self.i2c_port.writeto(self.address, reg_1, stop=False)
            self.i2c_port.writeto(self.address, byte, stop=True)
        except:
            return 1
        return 0

    def set_bit(self, reg, bit_num):
        """
        Set the bit indicated by bit_num in reg as 1, with every other bit retained
        :param reg: int
        :param bit_num: int
        :return:
        """
        val = self.read_register(reg)
        val |= (1 << bit_num)
        return self.write_register(reg, val)

    def clear_bit(self, reg, bit_num):
        """
        Set the bit indicated by bit_num in reg as 0, with every other bit retained
        :param reg: int
        :param bit_num: integer
        :return: integer
        """
        val = self.read_register(reg)
        val &= ~(1 << bit_num)
        return self.write_register(reg, val)

    def get_bit(self, reg, bit_num):
        """
        Gets a particular bit from the value of a particular register.
        :param reg: int
        :param bit_num: integer
        :return: byte
        """
        val = self.read_register(reg) & (1 << bit_num)
        return val


LDO_VALUES = LDOValues()
