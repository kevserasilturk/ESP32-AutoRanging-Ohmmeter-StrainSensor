# ESP32 Precision Auto-Ranging Ohmmeter
### Targeted for Flexible Strain Sensor Characterization 

This repository contains the firmware and design principles for a high-precision, auto-ranging ohmmeter built using an **ESP32**. The system is specifically engineered for **Biomedical Engineering** applications, such as measuring the resistance changes of carbon-composite flexible strain sensors during mechanical tensile testing.



##  Key Features
* **Automatic Range Switching:** Seamlessly transitions between 3 resistance tiers ($325.8 \Omega$, $990.0 \Omega$, and $4650.0 \Omega$) to maintain high resolution.
* **Dynamic Calibration:** Implements specific `vRef` offsets for each range to compensate for non-linear diode voltage drops.
* **Signal Conditioning:** Features a 100-sample software averaging filter combined with hardware decoupling for rock-solid stability.
* **Real-time Monitoring:** Optimized for serial plotting of strain-resistance curves.

##  Hardware Architecture
* **Microcontroller:** ESP32 (30-pin or 38-pin variants).
* **Isolation:** 1N4148 Diodes for preventing reverse current between inactive GPIO pins.
* **Stability:** 100nF ceramic capacitor between ADC Pin (GPIO 35) and GND.
* **Reference Resistors:** Precisely measured $325.8 \Omega$, $990.0 \Omega$, and $4650.0 \Omega$.



##  Circuit Connection Map
| Component | ESP32 Pin | Connection Note |
| :--- | :--- | :--- |
| **Range 1 (325.8Ω)** | GPIO 25 | Connected via Diode (Cathode to Resistor) |
| **Range 2 (990.0Ω)** | GPIO 26 | Connected via Diode (Cathode to Resistor) |
| **Range 3 (4650.0Ω)** | GPIO 27 | Connected via Diode (Cathode to Resistor) |
| **Common Node** | GPIO 35 | Junction of all resistors + Filter Capacitor |
| **Sensor (Rx)** | Junction & GND | The flexible strain sensor under test |

##  Installation & Usage
1.  Clone this repository.
2.  Open `main.ino` in Arduino IDE.
3.  Install ESP32 Board Support (v2.0.x or higher).
4.  Upload to your device and open **Serial Plotter** at 115200 baud.

