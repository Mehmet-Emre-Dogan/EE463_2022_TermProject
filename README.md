# EE463 2022 Term Project
2 kW Smart DC motor drive.
![image](./pics/demo/device/top.jpg)

# Circuit Diagrams

## Main Block Diagram
![image](./pics/blockDiag.png)

![image](./pics/blockDiag2.png)


## Main Buck Converter
![image](./pcbDesigns/buck/buck/pics/sch.jpg)

## Low Power Adapter
### Schematic
<div align="center"> 

![image](./pcbDesigns/mcuPower/mcuPower/pics/sch.jpg)
</div>

### PCB
<div align="center"> 

![image](./pcbDesigns/mcuPower/mcuPower/pics/board.jpg)

#### Top
![image](./pcbDesigns/mcuPower/mcuPower/pics/top.jpg)

#### Bottom
![image](./pcbDesigns/mcuPower/mcuPower/pics/bottom.jpg)
</div>

# Device
<div align="center"> 

## Top
![image](./pics/demo/device/top.jpg)

## Side
![image](./pics/demo/device/side.jpg)

## In
![image](./pics/demo/device/in.jpg)

## Line to Neutral
![image](./pics/demo/device/inWorking1.jpg)

## In [Wide Angle]
![image](./pics/demo/device/inWorking2.jpg)

</div>

# Analysis
## Power
### Rated Voltage [Loaded]
<div align="center">

![image](./pics/demo/power/rated.jpg) </div>

### Rated Power [Loaded]
<div align="center">

![image](./pics/demo/power/1.15Rated.jpg) </div>

## Waveforms

### Unloaded @v 25 % Duty
<div align="center">

![image](./pics/demo/scopeWvfs/noLoad25Duty.jpg) </div>

## Thermals

### IGBT
<div align="center">

![image](./pics/demo/thermal/igbt.jpg) </div>

### 3 Phase Diode Rectifier

<div align="center">

![image](./pics/demo/thermal/3PdiDiodeRect.jpg)</div>

# Miscellaneous Demos
 ## Webserver Demo
 Webdemo could be seen via following the link below:

  https://medogan.com/test_server_demo/463_esp_webdemo.html
## IP Scanning
![image](./pics/ipScan.jpg)
## Controller
![image](./pics/espControl.jpg)


# Demo Videos

 ## Motor Lab Demo
 https://www.youtube.com/watch?v=8uU4kF-YDas

 ## Motor Lab Demo [ WebGui]
 https://www.youtube.com/watch?v=bq63kfShjrk

 ## ESP Soft Starting Demo
 https://www.youtube.com/watch?v=6NDupYknA6s

 ## Optocoupler Demo
https://www.youtube.com/watch?v=Q2luOk-gndQ

 ## Testing IRFP460 N Channel MOSFET
  https://www.youtube.com/watch?v=HJyMbZKjLFo
  
 IRFP460 was not used in the project because it is not supplied by the University and its is expensive and we've left only one. [We had 2 but we burned one of them.]
 IXGH24N60C4D1 N Channel IGBT was used due to it is supplied by the University .