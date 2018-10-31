EESchema Schematic File Version 4
LIBS:power_monitor-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 2
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Sheet
S 3675 3700 950  750 
U 5AC9BE52
F0 "Power Handling" 50
F1 "Power-handling.sch" 50
F2 "relay_enable" I R 4625 4075 50 
F3 "AC_N_Switched" U L 3675 4250 50 
F4 "AC_L_Switched" U L 3675 4350 50 
F5 "AC_N" U L 3675 3800 50 
F6 "AC_L" U L 3675 3900 50 
F7 "VIsense" O R 4625 3975 50 
F8 "5v" U R 4625 3800 50 
F9 "gnd" U R 4625 4350 50 
$EndSheet
Wire Wire Line
	3425 3800 3675 3800
Wire Wire Line
	3425 3900 3675 3900
Wire Wire Line
	3425 4250 3675 4250
Wire Wire Line
	3425 4350 3675 4350
$Comp
L power_monitor-rescue:Wemos-D1-mini-MCU_Module U1
U 1 1 5AC90283
P 6325 4100
F 0 "U1" H 6325 5025 50  0000 C CNN
F 1 "Wemos-D1-mini" H 6325 4934 50  0000 C CNN
F 2 "RF_Module:Wemos D1 Mini" H 6125 5050 50  0001 C CNN
F 3 "" H 6125 5050 50  0001 C CNN
	1    6325 4100
	-1   0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0101
U 1 1 5AC9BD26
P 6700 3325
F 0 "#PWR0101" H 6700 3175 50  0001 C CNN
F 1 "+5V" H 6715 3498 50  0000 C CNN
F 2 "" H 6700 3325 50  0001 C CNN
F 3 "" H 6700 3325 50  0001 C CNN
	1    6700 3325
	1    0    0    -1  
$EndComp
Wire Wire Line
	6625 3400 6700 3400
Wire Wire Line
	6700 3400 6700 3325
$Comp
L power:GND #PWR0102
U 1 1 5AC9BDF7
P 6700 4475
F 0 "#PWR0102" H 6700 4225 50  0001 C CNN
F 1 "GND" H 6705 4302 50  0000 C CNN
F 2 "" H 6700 4475 50  0001 C CNN
F 3 "" H 6700 4475 50  0001 C CNN
	1    6700 4475
	1    0    0    -1  
$EndComp
Wire Wire Line
	6625 4400 6700 4400
Wire Wire Line
	6700 4400 6700 4475
$Comp
L power_monitor-rescue:Screw_Terminal_01x02-Connector_Specialized J2
U 1 1 5ACA6825
P 3225 4250
F 0 "J2" H 3150 4450 50  0000 C CNN
F 1 "230V AC Output" H 3150 4375 50  0000 C CNN
F 2 "Connector_Phoenix_MSTB:PhoenixContact_MSTBA_2,5_2-G_1x02_P5.00mm_Horizontal" H 3225 4250 50  0001 C CNN
F 3 "~" H 3225 4250 50  0001 C CNN
	1    3225 4250
	-1   0    0    -1  
$EndComp
$Comp
L power_monitor-rescue:Screw_Terminal_01x02-Connector_Specialized J1
U 1 1 5ACA682C
P 3225 3900
F 0 "J1" H 3150 3625 50  0000 C CNN
F 1 "230V AC Input" H 3150 3700 50  0000 C CNN
F 2 "Connector_Phoenix_MSTB:PhoenixContact_MSTBA_2,5_2-G_1x02_P5.00mm_Horizontal" H 3225 3900 50  0001 C CNN
F 3 "~" H 3225 3900 50  0001 C CNN
	1    3225 3900
	-1   0    0    1   
$EndComp
$Comp
L Device:R_Small R2
U 1 1 5ACA1835
P 5600 3250
F 0 "R2" H 5659 3296 50  0000 L CNN
F 1 "6K8" H 5659 3205 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 5600 3250 50  0001 C CNN
F 3 "~" H 5600 3250 50  0001 C CNN
	1    5600 3250
	-1   0    0    -1  
$EndComp
Wire Wire Line
	5600 3100 5600 3150
$Comp
L Device:R_Small R3
U 1 1 5ACA1E9C
P 5600 3550
F 0 "R3" H 5659 3596 50  0000 L CNN
F 1 "15K" H 5659 3505 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 5600 3550 50  0001 C CNN
F 3 "~" H 5600 3550 50  0001 C CNN
	1    5600 3550
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR0109
U 1 1 5ACA2116
P 5600 3650
F 0 "#PWR0109" H 5600 3400 50  0001 C CNN
F 1 "GND" H 5605 3477 50  0000 C CNN
F 2 "" H 5600 3650 50  0001 C CNN
F 3 "" H 5600 3650 50  0001 C CNN
	1    5600 3650
	1    0    0    -1  
$EndComp
Wire Wire Line
	6025 3700 5950 3700
Wire Wire Line
	5950 3800 6025 3800
$Comp
L power:+3.3V #PWR0104
U 1 1 5ACA3789
P 6925 3325
F 0 "#PWR0104" H 6925 3175 50  0001 C CNN
F 1 "+3.3V" H 6940 3498 50  0000 C CNN
F 2 "" H 6925 3325 50  0001 C CNN
F 3 "" H 6925 3325 50  0001 C CNN
	1    6925 3325
	1    0    0    -1  
$EndComp
Wire Wire Line
	6625 3500 6925 3500
Wire Wire Line
	6925 3500 6925 3325
$Comp
L power_monitor-rescue:Screw_Terminal_01x04-Connector_Specialized J4
U 1 1 5ACA4C22
P 7950 3925
F 0 "J4" H 8075 3825 50  0000 C CNN
F 1 "RFID" H 8125 3925 50  0000 C CNN
F 2 "Connector_Phoenix_MC:PhoenixContact_MC_1,5_4-G-3.81_1x04_P3.81mm_Horizontal" H 7950 3925 50  0001 C CNN
F 3 "~" H 7950 3925 50  0001 C CNN
	1    7950 3925
	1    0    0    1   
$EndComp
$Comp
L power_monitor-rescue:Screw_Terminal_01x03-Connector_Specialized J5
U 1 1 5ACA4CDB
P 7950 4325
F 0 "J5" H 8075 4225 50  0000 C CNN
F 1 "WS2812" H 8175 4325 50  0000 C CNN
F 2 "Connector_Phoenix_MC:PhoenixContact_MC_1,5_3-G-3.81_1x03_P3.81mm_Horizontal" H 7950 4325 50  0001 C CNN
F 3 "~" H 7950 4325 50  0001 C CNN
	1    7950 4325
	1    0    0    1   
$EndComp
$Comp
L power_monitor-rescue:Screw_Terminal_01x04-Connector_Specialized J3
U 1 1 5ACA4D49
P 7950 3425
F 0 "J3" H 8075 3325 50  0000 C CNN
F 1 "OLED" H 8100 3425 50  0000 C CNN
F 2 "Connector_Phoenix_MC:PhoenixContact_MC_1,5_4-G-3.81_1x04_P3.81mm_Horizontal" H 7950 3425 50  0001 C CNN
F 3 "~" H 7950 3425 50  0001 C CNN
	1    7950 3425
	1    0    0    1   
$EndComp
$Comp
L power:GND #PWR0106
U 1 1 5ACA6079
P 7350 4475
F 0 "#PWR0106" H 7350 4225 50  0001 C CNN
F 1 "GND" H 7355 4302 50  0000 C CNN
F 2 "" H 7350 4475 50  0001 C CNN
F 3 "" H 7350 4475 50  0001 C CNN
	1    7350 4475
	1    0    0    -1  
$EndComp
Wire Wire Line
	7350 4475 7350 4425
Wire Wire Line
	7350 4425 7750 4425
Wire Wire Line
	7750 4225 7250 4225
$Comp
L power:+5V #PWR0107
U 1 1 5ACA6A11
P 7250 3100
F 0 "#PWR0107" H 7250 2950 50  0001 C CNN
F 1 "+5V" H 7265 3273 50  0000 C CNN
F 2 "" H 7250 3100 50  0001 C CNN
F 3 "" H 7250 3100 50  0001 C CNN
	1    7250 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	7250 3100 7250 3425
Connection ~ 7250 3425
Wire Wire Line
	7250 3425 7250 3925
Wire Wire Line
	7350 3525 7350 4025
Connection ~ 7350 4425
Wire Wire Line
	7350 3525 7750 3525
Wire Wire Line
	7250 3425 7750 3425
Wire Wire Line
	7750 4025 7350 4025
Connection ~ 7350 4025
Wire Wire Line
	7350 4025 7350 4425
Wire Wire Line
	7750 3925 7250 3925
Connection ~ 7250 3925
Wire Wire Line
	7250 3925 7250 4225
Wire Wire Line
	6025 4100 5950 4100
Wire Wire Line
	6025 4200 5950 4200
Wire Wire Line
	6025 4300 5950 4300
Text GLabel 5550 3100 0    50   Input ~ 0
VIsense
Wire Wire Line
	5550 3100 5600 3100
Wire Wire Line
	6025 4400 5950 4400
Text GLabel 5950 4400 0    50   Output ~ 0
Relay
Text GLabel 5950 4300 0    50   Output ~ 0
WS2812
Text GLabel 5950 4200 0    50   Input ~ 0
RFID_SW
Text GLabel 5950 4100 0    50   Input ~ 0
RFID
Text GLabel 5950 3700 0    50   Output ~ 0
SCL
Text GLabel 5950 3800 0    50   BiDi ~ 0
SDA
Text GLabel 7750 3325 0    50   Input ~ 0
SCL
Text GLabel 7750 3225 0    50   BiDi ~ 0
SDA
Text GLabel 7750 3725 0    50   Output ~ 0
RFID_SW
Text GLabel 7750 3825 0    50   Output ~ 0
RFID
Text GLabel 4725 3975 2    50   Output ~ 0
VIsense
Wire Wire Line
	4625 3975 4725 3975
Wire Wire Line
	4625 4075 4725 4075
Text GLabel 4725 4075 2    50   Input ~ 0
Relay
Text GLabel 7750 4325 0    50   Input ~ 0
WS2812
Wire Notes Line
	8350 4725 8350 2850
Wire Notes Line
	8350 2850 7150 2850
Wire Notes Line
	7150 2850 7150 4725
Wire Notes Line
	7150 4725 8350 4725
Wire Notes Line
	7100 4725 7100 2850
Wire Notes Line
	7100 2850 5150 2850
Wire Notes Line
	5150 2850 5150 4725
Wire Notes Line
	5150 4725 7100 4725
Text Notes 8325 2950 2    50   ~ 0
Connectors
Text Notes 7075 2950 2    50   ~ 0
Logic
$Comp
L power:GND #PWR0108
U 1 1 5ACC9B7F
P 4725 4450
F 0 "#PWR0108" H 4725 4200 50  0001 C CNN
F 1 "GND" H 4730 4277 50  0000 C CNN
F 2 "" H 4725 4450 50  0001 C CNN
F 3 "" H 4725 4450 50  0001 C CNN
	1    4725 4450
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0110
U 1 1 5ACCA29B
P 4725 3700
F 0 "#PWR0110" H 4725 3550 50  0001 C CNN
F 1 "+5V" H 4740 3873 50  0000 C CNN
F 2 "" H 4725 3700 50  0001 C CNN
F 3 "" H 4725 3700 50  0001 C CNN
	1    4725 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	4725 4450 4725 4350
Wire Wire Line
	4725 4350 4625 4350
Wire Wire Line
	4725 3700 4725 3800
Wire Wire Line
	4725 3800 4625 3800
Wire Notes Line
	5100 4725 5100 3350
Wire Notes Line
	5100 3350 2975 3350
Wire Notes Line
	2975 3350 2975 4725
Wire Notes Line
	2975 4725 5100 4725
Text Notes 5050 3450 2    50   ~ 0
Power handling
Wire Wire Line
	5600 3350 5600 3400
Wire Wire Line
	6025 3400 5600 3400
Connection ~ 5600 3400
Wire Wire Line
	5600 3400 5600 3450
$Comp
L Device:R_Small R4
U 1 1 5BD988D9
P 5400 4375
F 0 "R4" H 5459 4421 50  0000 L CNN
F 1 "15K" H 5459 4330 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 5400 4375 50  0001 C CNN
F 3 "~" H 5400 4375 50  0001 C CNN
	1    5400 4375
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR0111
U 1 1 5BD98B30
P 5400 4475
F 0 "#PWR0111" H 5400 4225 50  0001 C CNN
F 1 "GND" H 5405 4302 50  0000 C CNN
F 2 "" H 5400 4475 50  0001 C CNN
F 3 "" H 5400 4475 50  0001 C CNN
	1    5400 4475
	1    0    0    -1  
$EndComp
Text GLabel 5400 4275 1    50   Input ~ 0
RFID_SW
$EndSCHEMATC
