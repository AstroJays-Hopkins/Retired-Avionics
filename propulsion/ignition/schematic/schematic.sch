EESchema Schematic File Version 4
LIBS:schematic-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 3
Title "Revised Ignition System - Relay Box and Rocket"
Date "2019-07-04"
Rev "0.2.0"
Comp "JHU Astrojays Student Rocketry"
Comment1 "Drafted by Benjamin Huang."
Comment2 "Motorized ball valve relay circuits designed by Rob Raniszewski."
Comment3 "Hall effect sensor not depicted; would be connected to Rocket Arduino D2 and D3"
Comment4 "Schematic of system as it physically was as on 4 July 2019."
$EndDescr
$Comp
L MCU_Module:Arduino_UNO_R3 A1
U 1 1 5C5F244D
P 1500 2150
F 0 "A1" H 1500 3328 50  0000 C CNN
F 1 "Arduino_UNO_R3" H 1500 3237 50  0000 C CNN
F 2 "Module:Arduino_UNO_R3" H 1650 1100 50  0001 L CNN
F 3 "https://www.arduino.cc/en/Main/arduinoBoardUno" H 1300 3200 50  0001 C CNN
	1    1500 2150
	1    0    0    -1  
$EndComp
$Comp
L RF_AM_FM:RFM69HCW U1
U 1 1 5C5F2738
P 1500 4100
F 0 "U1" H 1500 4778 50  0000 C CNN
F 1 "RFM69HCW" H 1500 4687 50  0000 C CNN
F 2 "" H -1800 5750 50  0001 C CNN
F 3 "http://www.hoperf.com/upload/rf/RFM69HCW-V1.1.pdf" H -1800 5750 50  0001 C CNN
	1    1500 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	1000 2650 900  2650
Wire Wire Line
	900  2650 900  3900
Wire Wire Line
	900  3900 1000 3900
Wire Wire Line
	1000 2750 950  2750
Wire Wire Line
	950  2750 950  4000
Wire Wire Line
	950  4000 1000 4000
Wire Wire Line
	1000 2850 1000 2950
Connection ~ 1000 2950
Wire Wire Line
	1000 2950 1000 3800
Wire Wire Line
	1000 2550 850  2550
Wire Wire Line
	850  2550 850  4100
Wire Wire Line
	850  4100 1000 4100
Wire Wire Line
	1600 1150 1600 950 
Wire Wire Line
	1600 950  2250 950 
Wire Wire Line
	2250 950  2250 3600
Wire Wire Line
	2250 3600 1500 3600
$Comp
L Device:Antenna AE1
U 1 1 5C5F2E48
P 2650 2900
F 0 "AE1" H 2730 2891 50  0000 L CNN
F 1 "Antenna" H 2730 2800 50  0000 L CNN
F 2 "" H 2650 2900 50  0001 C CNN
F 3 "~" H 2650 2900 50  0001 C CNN
	1    2650 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	2000 3800 2650 3800
Wire Wire Line
	2650 3800 2650 3100
$Comp
L power:GND #PWR03
U 1 1 5C5F3111
P 1400 4700
F 0 "#PWR03" H 1400 4450 50  0001 C CNN
F 1 "GND" H 1405 4527 50  0000 C CNN
F 2 "" H 1400 4700 50  0001 C CNN
F 3 "" H 1400 4700 50  0001 C CNN
	1    1400 4700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR02
U 1 1 5C5F31D5
P 1200 3250
F 0 "#PWR02" H 1200 3000 50  0001 C CNN
F 1 "GND" H 1205 3077 50  0000 C CNN
F 2 "" H 1200 3250 50  0001 C CNN
F 3 "" H 1200 3250 50  0001 C CNN
	1    1200 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	1200 3250 1400 3250
Connection ~ 1400 3250
Wire Wire Line
	1400 3250 1500 3250
Connection ~ 1500 3250
Wire Wire Line
	1500 3250 1600 3250
$Comp
L Relay:G5LE-1 K1
U 1 1 5C5F391B
P 1500 5650
F 0 "K1" V 933 5650 50  0000 C CNN
F 1 "G5LE-1" V 1024 5650 50  0000 C CNN
F 2 "Relay_THT:Relay_SPDT_Omron-G5LE-1" H 1950 5600 50  0001 L CNN
F 3 "http://www.omron.com/ecb/products/pdf/en-g5le.pdf" H 1500 5250 50  0001 C CNN
	1    1500 5650
	0    1    1    0   
$EndComp
NoConn ~ 1800 5750
$Comp
L power:GND #PWR04
U 1 1 5C5F3CA9
P 1900 5450
F 0 "#PWR04" H 1900 5200 50  0001 C CNN
F 1 "GND" H 1905 5277 50  0000 C CNN
F 2 "" H 1900 5450 50  0001 C CNN
F 3 "" H 1900 5450 50  0001 C CNN
	1    1900 5450
	1    0    0    -1  
$EndComp
Wire Wire Line
	800  2450 1000 2450
$Comp
L Device:Battery BT1
U 1 1 5C5F432F
P 1200 6250
F 0 "BT1" H 1308 6296 50  0000 L CNN
F 1 "11.1V" H 1308 6205 50  0000 L CNN
F 2 "" V 1200 6310 50  0001 C CNN
F 3 "~" V 1200 6310 50  0001 C CNN
	1    1200 6250
	1    0    0    -1  
$EndComp
Wire Wire Line
	800  2450 800  5450
Wire Wire Line
	1800 5950 2500 5950
Wire Wire Line
	2500 5550 2900 5550
$Comp
L Device:Buzzer BZ1
U 1 1 5C5F7AC3
P 3000 5900
F 0 "BZ1" H 3153 5929 50  0000 L CNN
F 1 "Buzzer" H 3153 5838 50  0000 L CNN
F 2 "" V 2975 6000 50  0001 C CNN
F 3 "~" V 2975 6000 50  0001 C CNN
	1    3000 5900
	1    0    0    -1  
$EndComp
Wire Wire Line
	2600 6250 2900 6250
$Comp
L Switch:SW_DPST_x2 SW1
U 1 1 5C5F9E48
P 4000 5900
F 0 "SW1" V 3954 5998 50  0000 L CNN
F 1 "SW_DPST_x2" V 4045 5998 50  0000 L CNN
F 2 "" H 4000 5900 50  0001 C CNN
F 3 "" H 4000 5900 50  0001 C CNN
	1    4000 5900
	0    1    1    0   
$EndComp
Connection ~ 2900 5550
Wire Wire Line
	2900 6000 2900 6250
Connection ~ 2900 6250
$Comp
L Device:Buzzer BZ2
U 1 1 5C685C7C
P 8200 5900
F 0 "BZ2" H 8353 5929 50  0000 L CNN
F 1 "Buzzer" H 8353 5838 50  0000 L CNN
F 2 "" V 8175 6000 50  0001 C CNN
F 3 "~" V 8175 6000 50  0001 C CNN
	1    8200 5900
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D2
U 1 1 5C685E29
P 3550 5900
F 0 "D2" V 3588 5783 50  0000 R CNN
F 1 "LED" V 3497 5783 50  0000 R CNN
F 2 "" H 3550 5900 50  0001 C CNN
F 3 "~" H 3550 5900 50  0001 C CNN
	1    3550 5900
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D4
U 1 1 5C685EC2
P 8750 5900
F 0 "D4" V 8788 5783 50  0000 R CNN
F 1 "LED" V 8697 5783 50  0000 R CNN
F 2 "" H 8750 5900 50  0001 C CNN
F 3 "~" H 8750 5900 50  0001 C CNN
	1    8750 5900
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2900 6250 3550 6250
$Comp
L Switch:SW_DPST_x2 SW1
U 2 1 5C687F08
P 9300 5550
F 0 "SW1" H 9300 5785 50  0000 C CNN
F 1 "SW_DPST_x2" H 9300 5694 50  0000 C CNN
F 2 "" H 9300 5550 50  0001 C CNN
F 3 "" H 9300 5550 50  0001 C CNN
	2    9300 5550
	1    0    0    -1  
$EndComp
$Comp
L Device:Fuse F1
U 1 1 5C685ED0
P 9700 5900
F 0 "F1" H 9760 5946 50  0000 L CNN
F 1 "Fuse" H 9760 5855 50  0000 L CNN
F 2 "" V 9630 5900 50  0001 C CNN
F 3 "~" H 9700 5900 50  0001 C CNN
	1    9700 5900
	1    0    0    -1  
$EndComp
Wire Wire Line
	9500 5550 9550 5550
Connection ~ 8750 5550
Connection ~ 8750 6250
Wire Wire Line
	8750 6250 9100 6250
Wire Wire Line
	8100 6250 8750 6250
Connection ~ 8100 5550
Wire Wire Line
	8100 5550 8750 5550
Connection ~ 3550 6250
Text Notes 5700 5850 0    50   ~ 0
<-- 50 Imperial Feet -->
Text Notes 5550 6000 0    50   ~ 0
<-- 15.24 Intelligent Meters -->
Wire Wire Line
	9550 5250 9550 5550
Connection ~ 9550 5550
Wire Wire Line
	9550 5550 9700 5550
$Comp
L pspice:DIODE D6
U 1 1 5C6AF6EA
P 9300 6250
F 0 "D6" H 9300 5985 50  0000 C CNN
F 1 "DIODE" H 9300 6076 50  0000 C CNN
F 2 "" H 9300 6250 50  0001 C CNN
F 3 "~" H 9300 6250 50  0001 C CNN
	1    9300 6250
	-1   0    0    1   
$EndComp
Wire Wire Line
	9500 6250 9700 6250
Wire Wire Line
	2350 2650 2350 850 
Wire Wire Line
	2350 850  900  850 
Wire Wire Line
	900  850  900  1750
Wire Wire Line
	900  1750 1000 1750
Wire Wire Line
	1400 1150 1050 1150
$Comp
L power:VCC #PWR01
U 1 1 5C6CD35A
P 1050 1150
F 0 "#PWR01" H 1050 1000 50  0001 C CNN
F 1 "VCC" H 1067 1323 50  0000 C CNN
F 2 "" H 1050 1150 50  0001 C CNN
F 3 "" H 1050 1150 50  0001 C CNN
	1    1050 1150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR06
U 1 1 5C6F43A0
P 2400 4900
F 0 "#PWR06" H 2400 4650 50  0001 C CNN
F 1 "GND" H 2405 4727 50  0000 C CNN
F 2 "" H 2400 4900 50  0001 C CNN
F 3 "" H 2400 4900 50  0001 C CNN
	1    2400 4900
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR05
U 1 1 5C6FEFF2
P 2400 4100
F 0 "#PWR05" H 2400 3950 50  0001 C CNN
F 1 "VCC" H 2417 4273 50  0000 C CNN
F 2 "" H 2400 4100 50  0001 C CNN
F 3 "" H 2400 4100 50  0001 C CNN
	1    2400 4100
	1    0    0    -1  
$EndComp
Text Notes 10550 6300 1    118  ~ 0
R O C K E T
Text Notes 2750 1650 0    118  ~ 0
R E L A Y   B O X
Text Notes 9750 6100 0    48   ~ 0
Ematch
Text Notes 3800 6400 0    48   ~ 0
Shunt switch
Text Notes 9050 5700 0    48   ~ 0
Safety switch
Text Notes 2750 5500 0    48   ~ 0
Voltage warning - relay box
Text Notes 7950 5500 0    48   ~ 0
Voltage warning - rocket
Text Notes 1750 6050 0    48   ~ 0
Normally open
Wire Wire Line
	2900 5550 2900 5800
Wire Wire Line
	2600 6250 2600 6450
Wire Wire Line
	3550 6250 4000 6250
Wire Wire Line
	4000 6100 4000 6250
Connection ~ 4000 6100
Connection ~ 4000 6250
Wire Wire Line
	4000 6250 8100 6250
Wire Wire Line
	4000 5550 4000 5700
Connection ~ 4000 5550
Wire Wire Line
	4000 5550 4350 5550
Connection ~ 4000 5700
Connection ~ 8100 6250
Wire Wire Line
	9700 6050 9700 6150
Wire Wire Line
	8100 6000 8100 6250
Wire Wire Line
	8100 5550 8100 5800
Wire Wire Line
	9700 5550 9700 5750
Wire Wire Line
	8750 6050 8750 6250
Wire Wire Line
	8750 5550 8750 5750
Wire Wire Line
	3550 6050 3550 6250
Wire Wire Line
	3550 5550 4000 5550
Wire Wire Line
	2900 5550 3550 5550
Connection ~ 3550 5550
Wire Wire Line
	3550 5550 3550 5750
$Sheet
S 2550 4000 1350 1000
U 5C72E2A7
F0 "relay-box-continuity-tester" 48
F1 "relay-box-continuity-tester.sch" 48
$EndSheet
Text HLabel 3900 4800 0    48   Input ~ 0
RELAY_BOX_DETECT_COMMON
Text HLabel 3900 4650 0    48   Input ~ 0
SWITCH_PROBE
Text HLabel 3900 4200 0    48   Output ~ 0
ARDUINO_SWITCH_DETECT
Text HLabel 2550 4100 2    48   Input ~ 0
VDD
Text HLabel 2550 4900 2    48   Input ~ 0
GND
Wire Wire Line
	4050 4200 3900 4200
Wire Wire Line
	4050 5700 4050 4650
Wire Wire Line
	4050 4650 3900 4650
Wire Wire Line
	4050 5700 4000 5700
Wire Wire Line
	4200 4800 3900 4800
Wire Wire Line
	4000 6100 4200 6100
$Sheet
S 9200 4050 1500 900 
U 5C75A9F8
F0 "rocket-continuity-tester" 48
F1 "rocket-continuity-tester.sch" 48
$EndSheet
Text HLabel 9200 4350 2    48   Output ~ 0
IGNITION_BOARD_SWITCH_DETECT
Text HLabel 9200 4500 2    48   Input ~ 0
SWITCH_PROBE
Text HLabel 9200 4650 2    48   Input ~ 0
ROCKET_DETECT_COMMON
Text HLabel 10700 4150 0    48   Input ~ 0
VDD
Text HLabel 10700 4850 0    48   Input ~ 0
GND
Wire Wire Line
	9000 5550 9100 5550
Wire Wire Line
	8750 5550 9000 5550
Connection ~ 9000 5550
Wire Wire Line
	9050 4650 9200 4650
Wire Wire Line
	9050 5250 9550 5250
Wire Wire Line
	9000 4500 9200 4500
Wire Wire Line
	9000 4500 9000 5550
Text HLabel 9200 4800 2    48   Input ~ 0
EMATCH_PROBE
Wire Wire Line
	10050 5150 10050 6150
Wire Wire Line
	10050 6150 9700 6150
Connection ~ 9700 6150
Wire Wire Line
	9700 6150 9700 6250
$Comp
L MCU_Module:Arduino_UNO_R3 A2
U 1 1 5C92BDE3
P 10100 2050
F 0 "A2" H 10100 3228 50  0000 C CNN
F 1 "Arduino_UNO_R3" H 10100 3137 50  0000 C CNN
F 2 "Module:Arduino_UNO_R3" H 10250 1000 50  0001 L CNN
F 3 "https://www.arduino.cc/en/Main/arduinoBoardUno" H 9900 3100 50  0001 C CNN
	1    10100 2050
	1    0    0    -1  
$EndComp
Text HLabel 9200 4200 2    48   Output ~ 0
IGNITION_BOARD_EMATCH_DETECT
Wire Wire Line
	9200 4200 9150 4200
Wire Wire Line
	10700 4150 10750 4150
Wire Wire Line
	10700 4850 10450 4850
Wire Wire Line
	4050 2650 2350 2650
Wire Wire Line
	2550 4100 2400 4100
Wire Wire Line
	2400 4900 2550 4900
Wire Notes Line
	600  6650 4750 6650
Wire Notes Line
	600  600  4750 600 
Wire Wire Line
	9200 4800 9100 4800
Wire Wire Line
	9100 4800 9100 5150
Wire Wire Line
	9100 5150 10050 5150
Wire Wire Line
	9050 5250 9050 4650
$Comp
L RF_AM_FM:RFM69HCW U2
U 1 1 5C9E173C
P 8350 4550
F 0 "U2" H 8350 5228 50  0000 C CNN
F 1 "RFM69HCW" H 8350 5137 50  0000 C CNN
F 2 "" H 5050 6200 50  0001 C CNN
F 3 "http://www.hoperf.com/upload/rf/RFM69HCW-V1.1.pdf" H 5050 6200 50  0001 C CNN
	1    8350 4550
	1    0    0    -1  
$EndComp
Wire Wire Line
	7850 4450 7800 4450
Wire Wire Line
	7850 4350 7750 4350
Wire Wire Line
	7850 4550 7700 4550
Wire Wire Line
	7150 3850 7150 3800
$Comp
L Device:Antenna AE2
U 1 1 5CA0E359
P 7150 3600
F 0 "AE2" H 7230 3591 50  0000 L CNN
F 1 "Antenna" H 7230 3500 50  0000 L CNN
F 2 "" H 7150 3600 50  0001 C CNN
F 3 "~" H 7150 3600 50  0001 C CNN
	1    7150 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	10000 3150 10100 3150
Connection ~ 10100 3150
Wire Wire Line
	10100 3150 10200 3150
Wire Wire Line
	7850 3600 9600 3600
Wire Wire Line
	8950 4250 8950 3850
Wire Wire Line
	8850 4250 8950 4250
Wire Wire Line
	7700 3450 7700 4550
Wire Wire Line
	7750 3500 7750 4350
Wire Wire Line
	7800 3550 7800 4450
Wire Wire Line
	7850 3600 7850 4250
Wire Wire Line
	9600 2450 9450 2450
Wire Wire Line
	9600 2550 9500 2550
Wire Wire Line
	9600 2650 9550 2650
Wire Wire Line
	9550 2650 9550 3550
Wire Wire Line
	9600 3600 9600 2850
Connection ~ 9600 2850
Wire Wire Line
	9600 2850 9600 2750
Wire Wire Line
	9450 2450 9450 3450
Wire Wire Line
	9500 2550 9500 3500
Wire Wire Line
	7700 3450 9450 3450
Wire Wire Line
	7750 3500 9500 3500
Wire Wire Line
	7800 3550 9550 3550
Wire Wire Line
	9150 4200 9150 3400
Wire Wire Line
	9150 3400 9400 3400
Wire Wire Line
	9400 2350 9600 2350
Wire Wire Line
	9100 4350 9200 4350
Wire Wire Line
	9100 4350 9100 3350
Wire Wire Line
	9100 3350 9350 3350
Wire Wire Line
	9350 2250 9600 2250
Wire Wire Line
	9350 2250 9350 3350
Wire Wire Line
	9400 2350 9400 3400
$Comp
L Device:Battery_Cell BT3
U 1 1 5CAD33C5
P 10550 3500
F 0 "BT3" V 10295 3550 50  0000 C CNN
F 1 "Battery_Cell" V 10386 3550 50  0000 C CNN
F 2 "" V 10550 3560 50  0001 C CNN
F 3 "~" V 10550 3560 50  0001 C CNN
	1    10550 3500
	0    1    1    0   
$EndComp
Wire Wire Line
	10100 3150 10100 3500
Wire Wire Line
	10450 3500 10450 3950
Wire Wire Line
	10100 3500 10450 3500
Connection ~ 10450 3500
Wire Wire Line
	10750 4150 10750 3900
Wire Wire Line
	10750 1000 10000 1000
Wire Wire Line
	10000 1000 10000 1050
Connection ~ 10750 3500
Wire Wire Line
	10750 3500 10750 1000
Wire Wire Line
	8250 5150 8350 5150
Wire Wire Line
	8900 5150 8900 3950
Wire Wire Line
	8900 3950 10450 3950
Connection ~ 8350 5150
Wire Wire Line
	8350 5150 8450 5150
Connection ~ 8450 5150
Wire Wire Line
	8450 5150 8900 5150
Connection ~ 10450 3950
Wire Wire Line
	10450 3950 10450 4850
Wire Wire Line
	10750 3900 8350 3900
Wire Wire Line
	8350 3900 8350 4050
Connection ~ 10750 3900
Wire Wire Line
	10750 3900 10750 3500
Wire Wire Line
	9300 2150 9600 2150
Wire Wire Line
	9300 2050 9600 2050
Wire Wire Line
	4200 4800 4200 6100
Text HLabel 3900 4500 0    48   Input ~ 0
VOLTAGE_PROBE
Text HLabel 3900 4350 0    48   Output ~ 0
ARDUINO_VOLTAGE_DETECT
Wire Wire Line
	4050 2650 4050 4200
Wire Wire Line
	3900 4500 4350 4500
Wire Wire Line
	4350 4500 4350 5550
Connection ~ 4350 5550
Wire Wire Line
	4350 5550 8100 5550
Wire Wire Line
	3900 4350 4200 4350
Wire Wire Line
	4200 4350 4200 2500
Wire Wire Line
	4200 2500 2500 2500
Wire Wire Line
	2500 2500 2500 700 
Wire Wire Line
	2500 700  750  700 
Wire Wire Line
	750  700  750  1850
Wire Wire Line
	750  1850 1000 1850
Wire Notes Line
	7600 850  10900 850 
Wire Notes Line
	7600 850  7600 6450
Wire Notes Line
	10900 850  10900 6450
Wire Notes Line
	7600 6450 10900 6450
Wire Wire Line
	1800 5450 1900 5450
Wire Wire Line
	800  5450 1200 5450
Wire Wire Line
	2500 5950 2500 5550
Wire Wire Line
	1200 6450 2600 6450
Wire Wire Line
	1200 5850 1200 6050
Wire Notes Line
	600  6650 600  600 
Wire Notes Line
	4750 600  4750 6650
Wire Wire Line
	9300 1950 9300 2050
Wire Wire Line
	9300 3150 9300 2150
Wire Wire Line
	8900 3150 9300 3150
Wire Wire Line
	8250 3700 8300 3700
Wire Wire Line
	8300 3150 8300 3700
Connection ~ 8900 3950
Wire Wire Line
	8900 3950 8900 3700
Connection ~ 8300 3700
Wire Wire Line
	8900 3700 8300 3700
$Comp
L Motor:Motor_DC M1
U 1 1 5CBCE40C
P 8950 2250
F 0 "M1" H 9108 2246 50  0000 L CNN
F 1 "Motor_DC" H 9108 2155 50  0000 L CNN
F 2 "" H 8950 2160 50  0001 C CNN
F 3 "~" H 8950 2160 50  0001 C CNN
	1    8950 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	8250 1950 8300 1950
Wire Wire Line
	8250 1950 8250 3700
$Comp
L Device:Battery BT2
U 1 1 5CAFFAFE
P 7950 2200
F 0 "BT2" V 7705 2200 50  0000 C CNN
F 1 "12V" V 7796 2200 50  0000 C CNN
F 2 "" V 7950 2260 50  0001 C CNN
F 3 "~" V 7950 2260 50  0001 C CNN
	1    7950 2200
	0    -1   -1   0   
$EndComp
$Comp
L Relay:G5LE-1 K3
U 1 1 5CA3B46E
P 8600 2950
F 0 "K3" V 9167 2950 50  0000 C CNN
F 1 "G5LE-1" V 9076 2950 50  0000 C CNN
F 2 "Relay_THT:Relay_SPDT_Omron-G5LE-1" H 9050 2900 50  0001 L CNN
F 3 "http://www.omron.com/ecb/products/pdf/en-g5le.pdf" H 8600 2550 50  0001 C CNN
	1    8600 2950
	0    -1   -1   0   
$EndComp
$Comp
L Relay:G5LE-1 K2
U 1 1 5CA3AEF0
P 8600 1750
F 0 "K2" V 9167 1750 50  0000 C CNN
F 1 "G5LE-1" V 9076 1750 50  0000 C CNN
F 2 "Relay_THT:Relay_SPDT_Omron-G5LE-1" H 9050 1700 50  0001 L CNN
F 3 "http://www.omron.com/ecb/products/pdf/en-g5le.pdf" H 8600 1350 50  0001 C CNN
	1    8600 1750
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8900 1950 9300 1950
Wire Wire Line
	8900 2750 8950 2750
Wire Wire Line
	8950 2750 8950 2550
Wire Wire Line
	8950 2050 8950 1550
Wire Wire Line
	8950 1550 8900 1550
Wire Wire Line
	8300 2850 8150 2850
Wire Wire Line
	8150 2850 8150 2200
Wire Wire Line
	8150 1650 8300 1650
Connection ~ 8150 2200
Wire Wire Line
	8150 2200 8150 1650
Wire Wire Line
	7750 1450 7750 2200
Wire Wire Line
	7750 2650 8300 2650
Wire Wire Line
	7750 1450 8300 1450
Connection ~ 7750 2200
Wire Wire Line
	7750 2200 7750 2650
Wire Wire Line
	7150 3850 8950 3850
$EndSCHEMATC
