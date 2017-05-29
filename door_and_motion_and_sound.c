#define door_pin D6 //Pin for Door Sensor
#define led_pin D7  //LED Pin for troubleshooting
#define pir_pin D0 //Pin for PIR motion sensor
#define mic_pin A0 //Pin for sound sensor
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;



void setup() {
    pinMode(pir_pin, INPUT); //PIR Sensor input connection pin
    pinMode(door_pin, INPUT_PULLUP); //Reed switch sensor input connection pin utilizing the internal Pull-up resistor
    pinMode(led_pin, OUTPUT); //LED output for troubleshooting
    Serial.begin(9600);
}

void loop() {
    unsigned long startMillis= millis();  // Start of sample window
    unsigned int peakToPeak = 0;   // peak-to-peak level
    unsigned int signalMax = 0;
    unsigned int signalMin = 1024;
    Serial.println("Nothing Detected");
        if (digitalRead(door_pin) == HIGH) {   //Check to see if the door is open (HIGH)
        Serial.println("Door Alert Loop Saterted");
        Particle.publish("Door_Status", "open", 60, PRIVATE); //Push the alert to the particle cloud
        digitalWrite(led_pin, HIGH); //Visual check on Particle to see if the sensor has been triggered using onboard LED
        while (digitalRead(door_pin) == HIGH) { // hang tight here while the door is open
            Serial.println("Also Checking for Motion");
            if (digitalRead(pir_pin) == HIGH) {   //Check to see if the sensor is active (HIGH)
            Serial.println("Motion Detected as well");
            Particle.publish("MotionAlert", "dual", 60, PRIVATE); //Push the alert to the particle cloud
            while (digitalRead(pir_pin) == HIGH); // hang tight here until motion stops
            }
        delay(1000);
        }
        digitalWrite(led_pin, LOW); //Turn off the LED when the door closes
        Particle.publish("Door_Status", "closed", 60, PRIVATE); //Push the alert to the particle cloud
        Serial.println("Door Alert Over");
    }
    if (digitalRead(pir_pin) == HIGH) {   //Check to see if the sensor is active (HIGH)
        Serial.println("Motion Alert Loop Started");
        Particle.publish("MotionAlert", "high", 60, PRIVATE); //Push the alert to the particle cloud
        digitalWrite(led_pin, HIGH); //Visual check on Particle to see if the sensor has been triggered using onboard LED
        while (digitalRead(pir_pin) == HIGH); // hang tight here until motion stops
        digitalWrite(led_pin, LOW); //Turn off LED once motion event is over
        Serial.println("Motion ALert Over");
    }
    while (millis() - startMillis < sampleWindow) { //collect a 50ms sample
    sample = analogRead(mic_pin);
    if (sample < 1024) { // toss out spurious readings
        if (sample > signalMax) {
        signalMax = sample;
        }  // save just the max levels
        else if (sample < signalMin) {
        signalMin = sample;  // save just the min levels
        }
     }
   }
    peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
    if (peakToPeak < 1000) { //If the sound level is above a certain threshold send an alert
        Particle.publish("SoundAlert","loud",60,PRIVATE);
        digitalWrite(led_pin,HIGH);
        delay(2000);
        digitalWrite(led_pin,LOW);
        delay(5000);
    }
   delay(2000);
}
