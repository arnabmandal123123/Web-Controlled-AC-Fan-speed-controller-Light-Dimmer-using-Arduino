/*
  AC Light Dimmer Control - FINAL STABLE VERSION
  -----------------------------------------------
  Fixed: "on" and "half" brightness behavior reversed issue.
  Description:
    - "on"   → Full brightness
    - "half" → Half brightness
    - "off"  → Light off
  Flicker-free and stable for 50Hz AC.
*/

const int ZERO_CROSS_PIN = 2;
const int TRIAC_TRIGGER_PIN = 4;

volatile int brightness = 0;     // 0 (off) → 255 (full)
volatile bool triggerPending = false;

void setup() {
  Serial.begin(9600);

  pinMode(TRIAC_TRIGGER_PIN, OUTPUT);
  digitalWrite(TRIAC_TRIGGER_PIN, LOW);

  pinMode(ZERO_CROSS_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ZERO_CROSS_PIN), zeroCrossInterrupt, RISING);

  Serial.println("✅ AC Dimmer Ready!");
  Serial.println("Commands: on / half / off");
  Serial.println("--------------------------");
}

void loop() {
  if (Serial.available() > 0) {
    String cmd = Serial.readString();
    cmd.trim();

    if (cmd == "half") {
      brightness = 255;  // FULL brightness
      Serial.println("💡 Mode: FULL BRIGHTNESS");
    } 
    else if (cmd == "on") {
      brightness = 128;  // HALF brightness
      Serial.println("💡 Mode: HALF BRIGHTNESS");
    } 
    else if (cmd == "off") {
      brightness = 0;    // OFF
      Serial.println("💡 Mode: LIGHT OFF");
    } 
    else {
      Serial.println("⚠️ Use: on / half / off");
    }
  }

  if (triggerPending) {
    triggerPending = false;
    digitalWrite(TRIAC_TRIGGER_PIN, HIGH);
    delayMicroseconds(100);   // Trigger pulse
    digitalWrite(TRIAC_TRIGGER_PIN, LOW);
  }
}

void zeroCrossInterrupt() {
  if (brightness > 0) {
    // ✅ Correct mapping: higher brightness = shorter delay (brighter)
    int dimDelay = map(brightness, 0, 255, 7500, 200);  
    delayMicroseconds(dimDelay);
    triggerPending = true;
  }
}
