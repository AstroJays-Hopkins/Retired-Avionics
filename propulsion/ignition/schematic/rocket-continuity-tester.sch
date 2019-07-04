EESchema Schematic File Version 4
LIBS:schematic-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 3 3
Title "Rocket Continuity Testers"
Date "2019-03-22"
Rev "0.1.0"
Comp "JHU Astrojays Student Rocketry"
Comment1 "Drafted by Benjamin Huang"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Comparator:LM393 U?
U 1 1 5C75B3BE
P 5650 4000
AR Path="/5C75B3BE" Ref="U?"  Part="1" 
AR Path="/5C75A9F8/5C75B3BE" Ref="U4"  Part="1" 
F 0 "U4" H 5650 4367 50  0000 C CNN
F 1 "LM393" H 5650 4276 50  0000 C CNN
F 2 "" H 5650 4000 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/lm393-n.pdf" H 5650 4000 50  0001 C CNN
	1    5650 4000
	1    0    0    -1  
$EndComp
$Comp
L Comparator:LM393 U?
U 3 1 5C75B3C5
P 6200 3950
AR Path="/5C75B3C5" Ref="U?"  Part="3" 
AR Path="/5C75A9F8/5C75B3C5" Ref="U4"  Part="3" 
F 0 "U4" H 6158 3996 50  0000 L CNN
F 1 "LM393" H 6158 3905 50  0000 L CNN
F 2 "" H 6200 3950 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/lm393-n.pdf" H 6200 3950 50  0001 C CNN
	3    6200 3950
	1    0    0    -1  
$EndComp
Wire Notes Line
	3200 4350 3200 3500
$Comp
L pspice:R R?
U 1 1 5C75B3CF
P 5000 3850
AR Path="/5C75B3CF" Ref="R?"  Part="1" 
AR Path="/5C75A9F8/5C75B3CF" Ref="R11"  Part="1" 
F 0 "R11" H 5068 3896 50  0000 L CNN
F 1 "50k" H 5068 3805 50  0000 L CNN
F 2 "" H 5000 3850 50  0001 C CNN
F 3 "~" H 5000 3850 50  0001 C CNN
	1    5000 3850
	1    0    0    -1  
$EndComp
$Comp
L pspice:R R?
U 1 1 5C75B3D6
P 5000 4350
AR Path="/5C75B3D6" Ref="R?"  Part="1" 
AR Path="/5C75A9F8/5C75B3D6" Ref="R12"  Part="1" 
F 0 "R12" H 5068 4396 50  0000 L CNN
F 1 "500" H 5068 4305 50  0000 L CNN
F 2 "" H 5000 4350 50  0001 C CNN
F 3 "~" H 5000 4350 50  0001 C CNN
	1    5000 4350
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 4100 5000 4100
Connection ~ 5000 4100
Wire Wire Line
	6100 4600 6100 4250
Wire Wire Line
	6100 3650 6100 3450
Wire Wire Line
	5000 4600 5250 4600
Wire Wire Line
	5250 4600 5250 4900
Connection ~ 5250 4600
Wire Wire Line
	5250 4600 6100 4600
Text Notes 4050 2300 0    50   ~ 0
Continuity Tester Circuits
$Comp
L Device:Buzzer BZ?
U 1 1 5C75B3E9
P 5450 2900
AR Path="/5C75B3E9" Ref="BZ?"  Part="1" 
AR Path="/5C75A9F8/5C75B3E9" Ref="BZ4"  Part="1" 
F 0 "BZ4" H 5603 2929 50  0000 L CNN
F 1 "Buzzer" H 5603 2838 50  0000 L CNN
F 2 "" V 5425 3000 50  0001 C CNN
F 3 "~" V 5425 3000 50  0001 C CNN
	1    5450 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 3000 5350 3250
$Comp
L Device:LED D?
U 1 1 5C75B3F1
P 5000 3000
AR Path="/5C75B3F1" Ref="D?"  Part="1" 
AR Path="/5C75A9F8/5C75B3F1" Ref="D5"  Part="1" 
F 0 "D5" V 5038 2883 50  0000 R CNN
F 1 "LED" V 4947 2883 50  0000 R CNN
F 2 "" H 5000 3000 50  0001 C CNN
F 3 "~" H 5000 3000 50  0001 C CNN
	1    5000 3000
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5350 2550 5000 2550
Connection ~ 5350 2550
Wire Wire Line
	5350 3250 5000 3250
Connection ~ 5350 3250
Wire Wire Line
	5000 3150 5000 3250
$Comp
L pspice:R R?
U 1 1 5C75B3FD
P 4700 3700
AR Path="/5C75B3FD" Ref="R?"  Part="1" 
AR Path="/5C75A9F8/5C75B3FD" Ref="R9"  Part="1" 
F 0 "R9" H 4768 3746 50  0000 L CNN
F 1 "50k" H 4768 3655 50  0000 L CNN
F 2 "" H 4700 3700 50  0001 C CNN
F 3 "~" H 4700 3700 50  0001 C CNN
	1    4700 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	4700 3450 5000 3450
Connection ~ 6100 3450
Wire Wire Line
	5000 3600 5000 3450
Connection ~ 5000 3450
Wire Wire Line
	5000 3450 6100 3450
Wire Wire Line
	6100 4600 6100 4750
Connection ~ 6100 4600
Wire Wire Line
	5950 4000 5950 3250
Wire Wire Line
	5350 3250 5950 3250
Wire Wire Line
	5350 2550 6100 2550
Wire Wire Line
	6100 2550 6100 3450
Wire Wire Line
	6100 2500 6100 2550
Connection ~ 6100 2550
Wire Wire Line
	5000 3250 4850 3250
Connection ~ 5000 3250
Text Notes 4000 2900 0    48   ~ 0
Safety switch indicators
$Comp
L Device:R R?
U 1 1 5C75B415
P 5000 2700
AR Path="/5C75B415" Ref="R?"  Part="1" 
AR Path="/5C75A9F8/5C75B415" Ref="R10"  Part="1" 
F 0 "R10" H 5070 2746 50  0000 L CNN
F 1 "220" H 5070 2655 50  0000 L CNN
F 2 "" V 4930 2700 50  0001 C CNN
F 3 "~" H 5000 2700 50  0001 C CNN
	1    5000 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 2550 5350 2800
Text HLabel 6100 2500 1    50   Input ~ 0
VDD
Text HLabel 4850 3250 0    50   Output ~ 0
IGNITION_BOARD_SWITCH_DETECT
Text HLabel 4700 4900 3    48   Input ~ 0
SWITCH_PROBE
Text HLabel 5250 4900 3    48   Input ~ 0
ROCKET_DETECT_COMMON
Text HLabel 6100 4750 3    48   Input ~ 0
GND
Wire Notes Line
	6450 3500 6450 4350
$Comp
L Comparator:LM393 U4
U 2 1 5C923FB2
P 3650 4000
F 0 "U4" H 3650 3633 50  0000 C CNN
F 1 "LM393" H 3650 3724 50  0000 C CNN
F 2 "" H 3650 4000 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/lm393-n.pdf" H 3650 4000 50  0001 C CNN
	2    3650 4000
	-1   0    0    1   
$EndComp
$Comp
L pspice:R R?
U 1 1 5C9243A8
P 4100 4200
AR Path="/5C9243A8" Ref="R?"  Part="1" 
AR Path="/5C75A9F8/5C9243A8" Ref="R7"  Part="1" 
F 0 "R7" H 4168 4246 50  0000 L CNN
F 1 "500" H 4168 4155 50  0000 L CNN
F 2 "" H 4100 4200 50  0001 C CNN
F 3 "~" H 4100 4200 50  0001 C CNN
	1    4100 4200
	1    0    0    -1  
$EndComp
$Comp
L pspice:R R?
U 1 1 5C92460C
P 4100 3700
AR Path="/5C92460C" Ref="R?"  Part="1" 
AR Path="/5C75A9F8/5C92460C" Ref="R6"  Part="1" 
F 0 "R6" H 4168 3746 50  0000 L CNN
F 1 "50k" H 4168 3655 50  0000 L CNN
F 2 "" H 4100 3700 50  0001 C CNN
F 3 "~" H 4100 3700 50  0001 C CNN
	1    4100 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 3900 4700 3900
Wire Wire Line
	4100 3900 4100 3950
Connection ~ 4100 3950
$Comp
L pspice:R R?
U 1 1 5C925596
P 4400 3850
AR Path="/5C925596" Ref="R?"  Part="1" 
AR Path="/5C75A9F8/5C925596" Ref="R8"  Part="1" 
F 0 "R8" H 4468 3896 50  0000 L CNN
F 1 "50k" H 4468 3805 50  0000 L CNN
F 2 "" H 4400 3850 50  0001 C CNN
F 3 "~" H 4400 3850 50  0001 C CNN
	1    4400 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	3950 3900 4100 3900
Wire Wire Line
	4100 3450 4400 3450
Connection ~ 4700 3450
Wire Wire Line
	4400 3450 4400 3600
Connection ~ 4400 3450
Wire Wire Line
	4400 3450 4700 3450
Wire Wire Line
	3950 4100 4400 4100
Wire Wire Line
	4400 4100 4400 4900
Connection ~ 4400 4100
Wire Wire Line
	4100 4450 4100 4900
Text HLabel 4100 4900 3    48   Input ~ 0
GND
Text HLabel 4400 4900 3    48   Input ~ 0
EMATCH_PROBE
Wire Wire Line
	4700 3900 4700 3950
Connection ~ 4700 3950
Wire Wire Line
	4700 3950 4700 4900
$Comp
L Device:LED D?
U 1 1 5C9298D9
P 3350 3000
AR Path="/5C9298D9" Ref="D?"  Part="1" 
AR Path="/5C75A9F8/5C9298D9" Ref="D3"  Part="1" 
F 0 "D3" V 3388 2883 50  0000 R CNN
F 1 "LED" V 3297 2883 50  0000 R CNN
F 2 "" H 3350 3000 50  0001 C CNN
F 3 "~" H 3350 3000 50  0001 C CNN
	1    3350 3000
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R?
U 1 1 5C9298E1
P 3350 2700
AR Path="/5C9298E1" Ref="R?"  Part="1" 
AR Path="/5C75A9F8/5C9298E1" Ref="R5"  Part="1" 
F 0 "R5" H 3420 2746 50  0000 L CNN
F 1 "220" H 3420 2655 50  0000 L CNN
F 2 "" V 3280 2700 50  0001 C CNN
F 3 "~" H 3350 2700 50  0001 C CNN
	1    3350 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3350 3150 3350 4000
Wire Wire Line
	5000 2550 3350 2550
Connection ~ 5000 2550
Text Notes 2200 2900 0    48   ~ 0
Ematch continuity indicator
Wire Wire Line
	2950 4000 3350 4000
Connection ~ 3350 4000
Text HLabel 2950 4000 0    48   Input ~ 0
IGNITION_BOARD_EMATCH_DETECT
Wire Notes Line
	3200 3500 6450 3500
Wire Notes Line
	3200 4350 6450 4350
$EndSCHEMATC
