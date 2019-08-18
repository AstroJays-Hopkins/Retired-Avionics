EESchema Schematic File Version 4
LIBS:schematic-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 3
Title "Relay Box Continuity and Voltage Tester"
Date "2019-07-04"
Rev "0.2.0"
Comp "JHU Astrojays Student Rocketry"
Comment1 "Drafted by Benjamin Huang"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Comparator:LM393 U3
U 1 1 5C72E92A
P 5800 3600
F 0 "U3" H 5800 3967 50  0000 C CNN
F 1 "LM393" H 5800 3876 50  0000 C CNN
F 2 "" H 5800 3600 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/lm393-n.pdf" H 5800 3600 50  0001 C CNN
	1    5800 3600
	1    0    0    -1  
$EndComp
$Comp
L Comparator:LM393 U3
U 3 1 5C72E931
P 6350 3550
F 0 "U3" H 6308 3596 50  0000 L CNN
F 1 "LM393" H 6308 3505 50  0000 L CNN
F 2 "" H 6350 3550 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/lm393-n.pdf" H 6350 3550 50  0001 C CNN
	3    6350 3550
	1    0    0    -1  
$EndComp
Wire Notes Line
	6600 3100 6600 3950
Wire Notes Line
	3500 3950 3500 3100
$Comp
L pspice:R R2
U 1 1 5C72E93C
P 5150 3200
F 0 "R2" H 5218 3246 50  0000 L CNN
F 1 "50k" H 5218 3155 50  0000 L CNN
F 2 "" H 5150 3200 50  0001 C CNN
F 3 "~" H 5150 3200 50  0001 C CNN
	1    5150 3200
	1    0    0    -1  
$EndComp
$Comp
L pspice:R R3
U 1 1 5C72E943
P 5150 3900
F 0 "R3" H 5218 3946 50  0000 L CNN
F 1 "500" H 5218 3855 50  0000 L CNN
F 2 "" H 5150 3900 50  0001 C CNN
F 3 "~" H 5150 3900 50  0001 C CNN
	1    5150 3900
	1    0    0    -1  
$EndComp
$Comp
L pspice:R R1
U 1 1 5C72E94A
P 4850 3200
F 0 "R1" H 4918 3246 50  0000 L CNN
F 1 "50k" H 4918 3155 50  0000 L CNN
F 2 "" H 4850 3200 50  0001 C CNN
F 3 "~" H 4850 3200 50  0001 C CNN
	1    4850 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	6250 4200 6250 3850
Wire Wire Line
	6250 3250 6250 2950
Connection ~ 5150 2950
Wire Wire Line
	5150 2950 4850 2950
Wire Wire Line
	5150 4200 5650 4200
Connection ~ 5650 4200
Wire Wire Line
	5650 4200 6250 4200
Wire Wire Line
	6250 2950 5150 2950
Text Notes 4400 2000 0    50   ~ 0
Continuity Tester Circuit
Wire Wire Line
	6100 3600 6100 2750
$Comp
L Device:LED D1
U 1 1 5C72E968
P 5350 2600
F 0 "D1" V 5388 2483 50  0000 R CNN
F 1 "LED" V 5297 2483 50  0000 R CNN
F 2 "" H 5350 2600 50  0001 C CNN
F 3 "~" H 5350 2600 50  0001 C CNN
	1    5350 2600
	0    -1   -1   0   
$EndComp
Connection ~ 6250 2950
Text Notes 4400 2500 0    48   ~ 0
Shunt switch indicator
$Comp
L Device:R R4
U 1 1 5C72E979
P 5350 2300
F 0 "R4" H 5420 2346 50  0000 L CNN
F 1 "220" H 5420 2255 50  0000 L CNN
F 2 "" V 5280 2300 50  0001 C CNN
F 3 "~" H 5350 2300 50  0001 C CNN
	1    5350 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	5650 4200 5650 5000
Wire Wire Line
	6250 4200 6250 4450
Connection ~ 6250 4200
Wire Wire Line
	5350 2750 3550 2750
Text HLabel 4850 4600 3    48   Input ~ 0
SWITCH_PROBE
Text HLabel 5650 5000 3    48   Input ~ 0
RELAY_BOX_DETECT_COMMON
Text HLabel 6250 4450 3    48   Input ~ 0
GND
Text HLabel 6250 1750 1    48   Input ~ 0
VDD
Text HLabel 3550 2750 0    48   Output ~ 0
ARDUINO_SWITCH_DETECT
Wire Wire Line
	5150 3450 5150 3500
Wire Wire Line
	6250 1750 6250 2150
$Comp
L Device:Buzzer BZ?
U 1 1 5C91B979
P 5800 2450
AR Path="/5C91B979" Ref="BZ?"  Part="1" 
AR Path="/5C75A9F8/5C91B979" Ref="BZ?"  Part="1" 
AR Path="/5C72E2A7/5C91B979" Ref="BZ3"  Part="1" 
F 0 "BZ3" H 5953 2479 50  0000 L CNN
F 1 "Buzzer" H 5953 2388 50  0000 L CNN
F 2 "" V 5775 2550 50  0001 C CNN
F 3 "~" V 5775 2550 50  0001 C CNN
	1    5800 2450
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 2150 5700 2150
Connection ~ 6250 2150
Wire Wire Line
	6250 2150 6250 2950
Wire Wire Line
	5350 2750 5700 2750
Connection ~ 5350 2750
Wire Wire Line
	5700 2350 5700 2150
Connection ~ 5700 2150
Wire Wire Line
	5700 2150 6250 2150
Wire Wire Line
	5700 2550 5700 2750
Connection ~ 5700 2750
Wire Wire Line
	5700 2750 6100 2750
$Comp
L Comparator:LM393 U3
U 2 1 5CCBEADA
P 3850 3600
F 0 "U3" H 3850 3233 50  0000 C CNN
F 1 "LM393" H 3850 3324 50  0000 C CNN
F 2 "" H 3850 3600 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/lm393-n.pdf" H 3850 3600 50  0001 C CNN
	2    3850 3600
	-1   0    0    1   
$EndComp
Connection ~ 4850 2950
Wire Wire Line
	4250 3700 4250 4600
Text HLabel 3400 3600 0    48   Output ~ 0
ARDUINO_VOLTAGE_DETECT
Text HLabel 4250 4600 3    48   Input ~ 0
RELAY_CONT_PROBE
Wire Wire Line
	4850 3450 4850 3700
Wire Wire Line
	5500 3500 5150 3500
Connection ~ 5150 3500
Wire Wire Line
	5150 3500 5150 3650
Wire Wire Line
	5150 4150 5150 4200
Wire Wire Line
	5500 3700 4850 3700
Connection ~ 4850 3700
Wire Wire Line
	4850 3700 4850 4600
Wire Wire Line
	3400 3600 3550 3600
$Comp
L pspice:R R?
U 1 1 5D54AB9F
P 4550 3200
F 0 "R?" H 4618 3246 50  0000 L CNN
F 1 "50k" H 4618 3155 50  0000 L CNN
F 2 "" H 4550 3200 50  0001 C CNN
F 3 "~" H 4550 3200 50  0001 C CNN
	1    4550 3200
	1    0    0    -1  
$EndComp
$Comp
L pspice:R R?
U 1 1 5D54AF3C
P 4250 3200
F 0 "R?" H 4318 3246 50  0000 L CNN
F 1 "50k" H 4318 3155 50  0000 L CNN
F 2 "" H 4250 3200 50  0001 C CNN
F 3 "~" H 4250 3200 50  0001 C CNN
	1    4250 3200
	1    0    0    -1  
$EndComp
$Comp
L pspice:R R?
U 1 1 5D54B313
P 4550 3750
F 0 "R?" H 4618 3796 50  0000 L CNN
F 1 "500" H 4618 3705 50  0000 L CNN
F 2 "" H 4550 3750 50  0001 C CNN
F 3 "~" H 4550 3750 50  0001 C CNN
	1    4550 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	4550 3450 4550 3500
Wire Wire Line
	4550 3500 4150 3500
Connection ~ 4550 3500
Wire Wire Line
	4250 3450 4250 3700
Wire Wire Line
	4250 3700 4150 3700
Connection ~ 4250 3700
Wire Wire Line
	4550 4000 4550 4200
Wire Wire Line
	4550 4200 5150 4200
Connection ~ 5150 4200
Wire Wire Line
	4250 2950 4850 2950
Wire Notes Line
	3500 3100 6600 3100
Wire Notes Line
	3500 3950 6600 3950
$EndSCHEMATC
