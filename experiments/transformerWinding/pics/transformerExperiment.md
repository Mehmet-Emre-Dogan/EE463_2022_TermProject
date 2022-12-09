# DIY Transformer Winding Experiment
## Introduction 
&nbsp;&nbsp;&nbsp;&nbsp; To power our driver with only one supply, we need to decrease the supply voltage for the control circuitry. It is preferable to step down the AC input voltage before it is rectifed for the motor and rectify the low voltage afterward. This is because not only DC-DC buck converters have limited input voltage ratings but also isolating the motor from the control circuitry reduce the noise.

&nbsp;&nbsp;&nbsp;&nbsp; As a result, we tried to make our transformer using the cores and winding cables found at the laboratory.

## Toroidal Core
&nbsp;&nbsp;&nbsp;&nbsp; First we wound a transformer using a toroidal core. Since its number of turns were pretty less, the flux leakage was too high and the output voltage was way more than the expected.

<div><center>

### Winding
![tr](.\mutualIndTransf.jpeg)

### Inductance Measurement at Primary
![tr](.\measMutualIndTransf.jpeg)

### Inductance Measurement at Primary [Results]
![tr](.\mutualIndPrimary.jpeg)

<center></div>

As a result, this component is useless as a transformer. 

## Double E Core
&nbsp;&nbsp;&nbsp;&nbsp; Second, a transformer is wound by using the E cores. This time, the number of turns were increased, but the magnetizing inductance of the transformer was not enough, causing a lot of current to flow even at no load. However, the voltage input-output characteristics was consistent with the expectations.

<div><center>

### Winding
![tr](.\eCoreTransformer.jpeg)

### Test Setup
![setup](.\setup.jpeg)

### Primary Voltage
![tr](.\primary.jpeg)

### Secondary Voltage
![tr](.\secondary.jpeg)

### Primary Current
![tr](.\primaryCurrent.jpeg)

<center></div>

## Conclusion
&nbsp;&nbsp;&nbsp;&nbsp; DIY transformer winding idea failed. We need to buy a transformer.