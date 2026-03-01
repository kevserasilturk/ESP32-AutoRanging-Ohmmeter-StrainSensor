ESP32 Auto-Ranging Ohmmeter for Strain SensorsProject OverviewThis project was developed as part of a Biomedical Engineering study at Ankara University to measure the resistance of custom-made flexible strain sensors. These sensors, often made from carbon-composite materials, exhibit significant resistance changes during mechanical tensile testing. This device automatically switches between three reference ranges to ensure high-resolution data collection without "out-of-range" errors.FeaturesAuto-Ranging Logic: Automatically switches between 3 resistance tiers based on the sensor's real-time load.Dynamic Calibration: Includes custom vRef values for each range to compensate for diode voltage drops.Signal Stability: Uses a 100-sample averaging filter and a hardware decoupling capacitor to eliminate electronic noise.Hardware ComponentsMicrocontroller: ESP32.Reference Resistors: $325.8 \Omega$, $990.0 \Omega$, and $4650.0 \Omega$.Diodes: 1N4148 (or similar) for switching isolation.Capacitor: 100nF for ADC stabilization.Circuit ConnectionsExcitation Pins: ESP32 Pins 25, 26, and 27 connected to diodes.Reference Arms: Diodes connected to the three precision resistors.Common Node: All resistor outputs joined at a single node connected to Pin 35 (ADC).Sensor (Rx): Connected between the Common Node and GND.Step 3: The Source Code (main.ino)Create a file named main.ino and paste the calibrated code we finalized:C++/*
 * ESP32 Precision Auto-Ranging Ohmmeter
 * Target Application: Flexible Strain Sensor Characterization
 * Developer: Kevser - Ankara University, Biomedical Engineering
 */

const int rangePins[] = {25, 26, 27};
const int NUM_RANGES = 3;
const float refResistors[] = {325.8, 990.0, 4650.0};

const int adcPin = 35;
const float adcResolution = 4095.0;
int activeRange = 1; 

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  
  for (int i = 0; i < NUM_RANGES; i++) {
    pinMode(rangePins[i], INPUT);
  }
}

void loop() {
  for (int i = 0; i < NUM_RANGES; i++) {
    if (i == activeRange) {
      pinMode(rangePins[i], OUTPUT);
      digitalWrite(rangePins[i], HIGH);
    } else {
      pinMode(rangePins[i], INPUT); 
    }
  }

  delay(15);

  long sumAdc = 0;
  int samples = 100;
  for (int i = 0; i < samples; i++) {
    sumAdc += analogRead(adcPin);
    delayMicroseconds(500);
  }
  float avgAdc = (float)sumAdc / samples;

  // Auto-Ranging Logic
  if (avgAdc < 800 && activeRange > 0) {
    activeRange--; 
    return; 
  }
  else if (avgAdc > 3300 && activeRange < NUM_RANGES - 1) {
    activeRange++; 
    return; 
  }

  float vOut = (avgAdc / adcResolution) * 3.3; 
  
  // Calibrated vRef per range (compensating for diode drop)
  float dynamicVRef;
  if (activeRange == 0)      dynamicVRef = 2.254; 
  else if (activeRange == 1) dynamicVRef = 2.376; 
  else                       dynamicVRef = 2.550; 

  float rx = 0.0;
  if (dynamicVRef - vOut > 0.01) {
    rx = refResistors[activeRange] * (vOut / (dynamicVRef - vOut));
  } else {
    rx = -1; 
  }

  Serial.print("Range: ");
  Serial.print(activeRange + 1);
  Serial.print(" | Vout: ");
  Serial.print(vOut, 3);
  Serial.print("V | Resistance: ");
  
  if (rx > 0) {
    Serial.print(rx, 2);
    Serial.println(" Ohm");
  } else {
    Serial.println("OPEN CIRCUIT");
  }

  delay(350); 
}
Step 4: License and TagsLicense: Choose MIT License (standard for open-source).Topics/Tags: biomedical-engineering, esp32, sensors, strain-gauge, arduino, electronics.Would you like me to help you write a professional LinkedIn post to share this GitHub link once you've uploaded it?# ESP32-AutoRanging-Ohmmeter-StrainSensor
A precision auto-ranging ohmmeter designed for characterizing flexible strain sensors in mechanical tensile testing, featuring dynamic calibration and signal filtering.
