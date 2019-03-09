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
P 7375 4625
F 0 "#PWR0103" H 7375 4375 50  0001 C CNN
F 1 "GND" H 7300 4450 50  0000 L CNN
F 2 "" H 7375 4625 50  0001 C CNN
F 3 "" H 7375 4625 50  0001 C CNN
	1    7375 4625
	1    0    0    -1  
$EndComp
Wire Wire Line
	6550 4525 6550 4575
Wire Wire Line
	5725 3650 5650 3650
Wire Wire Line
	6150 4325 5975 4325
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
P 6550 4125
F 0 "U2" H 6800 4475 50  0000 C CNN
F 1 "ACS723xMATR-10AB" H 6575 4650 50  0000 L CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 6900 4075 50  0001 L CIN
F 3 "http://www.allegromicro.com/~/media/Files/Datasheets/ACS723KMA-Datasheet.ashx?la=en" H 6550 4125 50  0001 C CNN
	1    6550 4125
	1    0    0    -1  
$EndComp
$Comp
L Jumper:SolderJumper_3_Bridged12 JP1
U 1 1 5AC9C3B0
P 7375 4125
F 0 "JP1" V 7329 4193 50  0000 L CNN
F 1 "BW_SEL" V 7420 4193 50  0000 L CNN
F 2 "Jumper:SolderJumper-3_P1.3mm_Bridged2Bar12_RoundedPad1.0x1.5mm_NumberLabels" H 7375 4125 50  0001 C CNN
F 3 "~" H 7375 4125 50  0001 C CNN
	1    7375 4125
	0    1    1    0   
$EndComp
Wire Wire Line
	6550 3675 7375 3675
Wire Wire Line
	7375 3675 7375 3925
Wire Wire Line
	6550 3675 6550 3725
Wire Wire Line
	7375 4575 7375 4325
Wire Wire Line
	6950 4025 7000 4025
$Comp
L Device:C_Small C1
U 1 1 5AC9C3C1
P 7775 4425
F 0 "C1" H 7683 4379 50  0000 R CNN
F 1 "0.1uF" H 7683 4470 50  0000 R CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 7775 4425 50  0001 C CNN
F 3 "~" H 7775 4425 50  0001 C CNN
	1    7775 4425
	1    0    0    1   
$EndComp
Wire Wire Line
	7375 4575 7775 4575
Wire Wire Line
	7775 4575 7775 4525
Connection ~ 7375 4575
Wire Wire Line
	7775 4325 7775 3675
Wire Wire Line
	7775 3675 7375 3675
Connection ~ 7375 3675
Wire Notes Line
	6425 2825 4325 2825
Wire Notes Line
	4325 2825 4325 4800
Wire Notes Line
	4325 4800 6425 4800
Wire Notes Line
	6425 2825 6425 4800
Text Notes 4400 4725 0    150  ~ 0
!!HIGH-VOLTAGE!!
Text HLabel 7000 4025 2    50   Output ~ 0
VIsense
Text HLabel 6375 2150 2    50   Input ~ 0
relay_enable
Text HLabel 5725 3650 2    50   UnSpc ~ 0
AC_N_Switched
Text HLabel 5975 4325 0    50   UnSpc ~ 0
AC_L_Switched
Text HLabel 4975 3550 0    50   UnSpc ~ 0
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
Wire Wire Line
	7375 4575 7375 4625
Wire Wire Line
	3925 4250 3850 4250
Wire Wire Line
	4800 4250 4725 4250
Text HLabel 4800 4250 2    50   UnSpc ~ 0
AC_N
Text HLabel 4800 4050 2    50   UnSpc ~ 0
AC_L
Wire Wire Line
	4800 4050 4725 4050
Wire Wire Line
	6950 4125 7225 4125
Wire Wire Line
	6550 4575 7375 4575
Text HLabel 7850 3675 2    50   UnSpc ~ 0
5v
Wire Wire Line
	7775 3675 7850 3675
Connection ~ 7775 3675
Text HLabel 3850 4050 0    50   UnSpc ~ 0
5v
Wire Wire Line
	3925 4050 3850 4050
Text HLabel 7850 4575 2    50   UnSpc ~ 0
gnd
Wire Wire Line
	7775 4575 7850 4575
Connection ~ 7775 4575
Text HLabel 6300 2500 2    50   UnSpc ~ 0
gnd
Wire Wire Line
	6250 2500 6300 2500
Connection ~ 6250 2500
Text HLabel 3850 4250 0    50   UnSpc ~ 0
gnd
Wire Wire Line
	5000 2000 5000 2500
$Comp
L Converter_ACDC:IRM-02-5 PS1
U 1 1 5C831918
P 4325 4150
F 0 "PS1" H 4325 4475 50  0000 C CNN
F 1 "IRM-02-5" H 4325 4384 50  0000 C CNN
F 2 "Converter_ACDC:Converter_ACDC_MeanWell_IRM-02-xx_THT" H 4325 3850 50  0001 C CNN
F 3 "http://www.meanwell.com/productPdf.aspx?i=675" H 4725 3800 50  0001 C CNN
	1    4325 4150
	-1   0    0    -1  
$EndComp
Wire Wire Line
	4975 3550 5050 3550
Wire Wire Line
	4700 3925 4700 3150
Wire Wire Line
	4700 3150 5050 3150
Wire Wire Line
	4700 3925 6150 3925
$EndSCHEMATC
