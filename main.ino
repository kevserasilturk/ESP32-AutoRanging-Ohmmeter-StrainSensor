// ESP32 3-Level (Auto-Ranging) Precision Multimeter
// Developed by: Kevser - Ankara University Biomedical Engineering

// GPIO Pins (Power applied sequentially through these pins)
const int stagePins[] = {25, 26, 27};
const int NUM_STAGES = 3;

// Reference resistor values for each stage (Ohms)
const float referenceResistors[] = {325.8, 990.0, 4650.0};

// Measurement and ADC Settings
const int adcPin = 35;        // Pin connected to the common node
const float adcResolution = 4095.0;

// INITIAL STAGE
int activeStage = 1; // Start at the 990 Ohm stage

void setup() {
  Serial.begin(115200);
  
  // Set ADC resolution to 12-bit (0-4095)
  analogReadResolution(12);
  
  // Initialize pins to HIGH-Z (input)
  for (int i = 0; i < NUM_STAGES; i++) {
    pinMode(stagePins[i], INPUT);
  }
  
  Serial.println("========================================");
  Serial.println("   PRECISION AUTO-RANGING MULTIMETER v2.0   ");
  Serial.println("   Ready for Mechanical Pull Test           ");
  Serial.println("========================================");
}

void loop() {
  // --- ACTIVATE CURRENT STAGE ---
  for (int i = 0; i < NUM_STAGES; i++) {
    if (i == activeStage) {
      pinMode(stagePins[i], OUTPUT);
      digitalWrite(stagePins[i], HIGH);
    } else {
      pinMode(stagePins[i], INPUT); 
    }
  }

  // Wait for voltage to settle
  delay(15);

  // --- NOISE FILTERING (100 Sample Average) ---
  long adcSum = 0;
  int samples = 100;
  for (int i = 0; i < samples; i++) {
    adcSum += analogRead(adcPin);
    delayMicroseconds(500);
  }
  float adcAverage = (float)adcSum / samples;

  // --- AUTO-RANGING LOGIC ---
  // If ADC is too low (low sensitivity), shift down
  if (adcAverage < 800 && activeStage > 0) {
    activeStage--; 
    return; 
  }
  // If ADC is too high (approaching 3.3V), shift up
  else if (adcAverage > 3300 && activeStage < NUM_STAGES - 1) {
    activeStage++; 
    return; 
  }

  // --- CALCULATION AND CALIBRATION ---
  float vOut = (adcAverage / adcResolution) * 3.3; 
  
  // Stage-specific voltage reference (includes diode drops)
  float dynamicVRef;
  if (activeStage == 0)      dynamicVRef = 2.254; // 325.8 Ohm stage calibration
  else if (activeStage == 1) dynamicVRef = 2.376; // 990.0 Ohm stage calibration
  else                       dynamicVRef = 2.550; // 4650.0 Ohm stage (approx.)

  float measuredResistor = 0.0;
  // Safety: ensure vOut does not exceed reference
  if (dynamicVRef - vOut > 0.01) {
    measuredResistor = referenceResistors[activeStage] * (vOut / (dynamicVRef - vOut));
  } else {
    measuredResistor = -1; // Open circuit or out of range
  }

  // --- PRINT RESULTS ---
  Serial.print("STAGE: ");
  Serial.print(activeStage + 1);
  Serial.print(" [Ref: ");
  Serial.print(referenceResistors[activeStage]);
  Serial.print(" Ohm] | Vout: ");
  Serial.print(vOut, 3);
  Serial.print("V | CALCULATED RESISTANCE: ");
  
  if (measuredResistor > 0) {
    Serial.print(measuredResistor, 2);
    Serial.println(" Ohm");
  } else {
    Serial.println("OPEN CIRCUIT");
  }

  delay(350); 
}