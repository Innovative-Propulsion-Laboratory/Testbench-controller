#define flowSensorPin 35
volatile uint32_t flowPulseCount = 0;  // Shared with ISR, must be volatile
uint32_t lastMeasurementTime = 0;
uint32_t lastPulseCount = 0;
const uint32_t flowSensorReadRate = 1000;  // in ms, e.g. 1000 = every 1 second
const float pulsesPerLiter = 338.39;               // number of pulses per liter

void flowSensorISR() {
  flowPulseCount++;  // Just increment the counter
}

void setup() {
  pinMode(flowSensorPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(flowSensorPin), flowSensorISR, RISING);
  Serial.begin(115200);
}

void loop() {
  if (millis() - lastMeasurementTime >= flowSensorReadRate) {
      lastMeasurementTime = millis();
    noInterrupts();  // Prevent ISR from changing the value while we read
    unsigned long pulses = flowPulseCount;
    flowPulseCount = 0;  // Reset for next interval
    interrupts();

    float flowRate = (pulses * 1000.0) / (pulsesPerLiter * (flowSensorReadRate / 1000.0));
    Serial.print("Time: ");
    Serial.print(lastMeasurementTime);
    Serial.print("\t Flow rate: ");
    Serial.print(flowRate);
    Serial.println(" L/min");
  }
}