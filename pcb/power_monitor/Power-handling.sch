EESchema Schematic File Version 4
LIBS:power_monitor-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 2
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L power:GND #PWR0103
U 1 1 5AC9C331
P 7375 4350
F 0 "#PWR0103" H 7375 4100 50  0001 C CNN
F 1 "GND" H 7300 4175 50  0000 L CNN
F 2 "" H 7375 4350 50  0001 C CNN
F 3 "" H 7375 4350 50  0001 C CNN
	1    7375 4350
	1    0    0    -1  
$EndComp
Wire Wire Line
	6550 4250 6550 4300
Wire Wire Line
	5725 3650 5650 3650
Wire Wire Line
	6150 4050 5975 4050
$Comp
L Device:D_Small D1
U 1 1 5AC9C366
P 5350 2500
F 0 "D1" H 5350 1125 50  0000 C CNN
F 1 "1N4001" H 5350 2575 50  0000 C CNN
F 2 "Diode_SMD:D_SMA_Handsoldering" V 5350 2500 50  0001 C CNN
F 3 "~" V 5350 2500 50  0001 C CNN
	1    5350 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5250 2500 5000 2500
Wire Wire Line
	5000 2500 5000 2750
Wire Wire Line
	5000 2750 5050 2750
Wire Wire Line
	5450 2500 5700 2500
Wire Wire Line
	5700 2500 5700 2750
Wire Wire Line
	5700 2750 5650 2750
Connection ~ 5700 2500
$Comp
L power:+5V #PWR0105
U 1 1 5AC9C375
P 5000 2000
F 0 "#PWR0105" H 5000 1850 50  0001 C CNN
F 1 "+5V" H 5015 2173 50  0000 C CNN
F 2 "" H 5000 2000 50  0001 C CNN
F 3 "" H 5000 2000 50  0001 C CNN
	1    5000 2000
	1    0    0    -1  
$EndComp
Connection ~ 5000 2500
Wire Wire Line
	5700 2500 5800 2500
$Comp
L Device:R_Small R1
U 1 1 5AC9C391
P 6250 2325
F 0 "R1" H 6309 2371 50  0000 L CNN
F 1 "15k" H 6309 2280 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 6250 2325 50  0001 C CNN
F 3 "~" H 6250 2325 50  0001 C CNN
	1    6250 2325
	1    0    0    -1  
$EndComp
Wire Wire Line
	6200 2500 6250 2500
Wire Wire Line
	6000 2200 6000 2150
$Comp
L Device:Q_NMOS_GSD Q1
U 1 1 5AC9C39B
P 6000 2400
F 0 "Q1" V 5900 2475 50  0000 L CNN
F 1 "FDN339AN" V 6225 2250 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23_Handsoldering" H 6200 2500 50  0001 C CNN
F 3 "~" H 6000 2400 50  0001 C CNN
	1    6000 2400
	0    -1   1    0   
$EndComp
Wire Wire Line
	6250 2500 6250 2425
Wire Wire Line
	6250 2150 6250 2225
Wire Wire Line
	6000 2150 6250 2150
Wire Wire Line
	6250 2150 6375 2150
Connection ~ 6250 2150
$Comp
L power_monitor-rescue:ACS723xMATR-10AB-Sensor_Current U2
U 1 1 5AC9C3A9
P 6550 3850
F 0 "U2" H 6800 4200 50  0000 C CNN
F 1 "ACS723xMATR-10AB" H 6575 4375 50  0000 L CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 6900 3800 50  0001 L CIN
F 3 "http://www.allegromicro.com/~/media/Files/Datasheets/ACS723KMA-Datasheet.ashx?la=en" H 6550 3850 50  0001 C CNN
	1    6550 3850
	1    0    0    -1  
$EndComp
$Comp
L Jumper:SolderJumper_3_Bridged12 JP1
U 1 1 5AC9C3B0
P 7375 3850
F 0 "JP1" V 7329 3918 50  0000 L CNN
F 1 "BW_SEL" V 7420 3918 50  0000 L CNN
F 2 "Jumper:SolderJumper-3_P1.3mm_Bridged2Bar12_RoundedPad1.0x1.5mm_NumberLabels" H 7375 3850 50  0001 C CNN
F 3 "~" H 7375 3850 50  0001 C CNN
	1    7375 3850
	0    1    1    0   
$EndComp
Wire Wire Line
	6550 3400 7375 3400
Wire Wire Line
	7375 3400 7375 3650
Wire Wire Line
	6550 3400 6550 3450
Wire Wire Line
	7375 4300 7375 4050
Wire Wire Line
	6950 3750 7000 3750
$Comp
L Device:C_Small C1
U 1 1 5AC9C3C1
P 7775 4150
F 0 "C1" H 7683 4104 50  0000 R CNN
F 1 "0.1uF" H 7683 4195 50  0000 R CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 7775 4150 50  0001 C CNN
F 3 "~" H 7775 4150 50  0001 C CNN
	1    7775 4150
	1    0    0    1   
$EndComp
Wire Wire Line
	7375 4300 7775 4300
Wire Wire Line
	7775 4300 7775 4250
Connection ~ 7375 4300
Wire Wire Line
	7775 4050 7775 3400
Wire Wire Line
	7775 3400 7375 3400
Connection ~ 7375 3400
Wire Notes Line
	6425 2825 4325 2825
Wire Notes Line
	4325 2825 4325 5500
Wire Notes Line
	4325 5500 6425 5500
Wire Notes Line
	6425 2825 6425 5500
Text Notes 4400 5425 0    150  ~ 0
!!HIGH-VOLTAGE!!
Text HLabel 7000 3750 2    50   Output ~ 0
VIsense
Text HLabel 6375 2150 2    50   Input ~ 0
relay_enable
Text HLabel 4975 3550 0    50   UnSpc ~ 0
AC_N_Switched
Text HLabel 5975 4050 0    50   UnSpc ~ 0
AC_L_Switched
Text HLabel 5725 3650 2    50   UnSpc ~ 0
AC_N
Text HLabel 5725 3250 2    50   UnSpc ~ 0
AC_L
Wire Wire Line
	5725 3250 5650 3250
$Comp
L power_monitor-rescue:G2RL-2A-Relay K1
U 1 1 5AC87461
P 5350 3150
F 0 "K1" H 5350 2383 50  0000 C CNN
F 1 "G2RL-2A-DC5" H 5350 3800 50  0000 C CNN
F 2 "Relay_THT:Relay_DPST_Schrack-RT2-FormA_RM5mm" V 5350 3150 50  0001 C CNN
F 3 "" V 5350 3150 50  0001 C CNN
	1    5350 3150
	-1   0    0    1   
$EndComp
$Comp
L power_monitor-rescue:HLK-PM01-Power_Management PS1
U 1 1 5AC93002
P 6425 5000
F 0 "PS1" H 6425 5365 50  0000 C CNN
F 1 "HLK-PM01" H 6425 5274 50  0000 C CNN
F 2 "Converter_ACDC:HLK-PM01" H 5275 4650 50  0001 C CNN
F 3 "" H 5275 4650 50  0001 C CNN
	1    6425 5000
	1    0    0    -1  
$EndComp
Wire Wire Line
	7375 4300 7375 4350
Wire Wire Line
	6825 5100 6900 5100
Wire Wire Line
	5950 5100 6025 5100
Text HLabel 5950 5100 0    50   UnSpc ~ 0
AC_N
Text HLabel 5950 4900 0    50   UnSpc ~ 0
AC_L
Wire Wire Line
	5950 4900 6025 4900
Wire Wire Line
	6950 3850 7225 3850
Wire Wire Line
	6550 4300 7375 4300
Wire Wire Line
	4975 3550 5050 3550
Wire Wire Line
	6150 3650 6075 3650
Wire Wire Line
	6075 3925 4775 3925
Wire Wire Line
	4775 3150 5050 3150
Wire Wire Line
	6075 3650 6075 3925
Wire Wire Line
	4775 3150 4775 3925
Text HLabel 7850 3400 2    50   UnSpc ~ 0
5v
Wire Wire Line
	7775 3400 7850 3400
Connection ~ 7775 3400
Text HLabel 6900 4900 2    50   UnSpc ~ 0
5v
Wire Wire Line
	6825 4900 6900 4900
Text HLabel 7850 4300 2    50   UnSpc ~ 0
gnd
Wire Wire Line
	7775 4300 7850 4300
Connection ~ 7775 4300
Text HLabel 6300 2500 2    50   UnSpc ~ 0
gnd
Wire Wire Line
	6250 2500 6300 2500
Connection ~ 6250 2500
Text HLabel 6900 5100 2    50   UnSpc ~ 0
gnd
Wire Wire Line
	5000 2000 5000 2500
$EndSCHEMATC
