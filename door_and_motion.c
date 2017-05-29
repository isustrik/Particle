#define door_pin D6 //Pin for Door Sensor
#define led_pin D7  //LED Pin for troubleshooting
#define pir_pin D0 //Pin for PIR motion sensor



void setup() {
    pinMode(pir_pin, INPUT); //PIR Sensor input connection pin
    pinMode(door_pin, INPUT_PULLUP); //Reed switch sensor input connection pin utilizing the internal Pull-up resistor
    pinMode(led_pin, OUTPUT); //LED output for troubleshooting
    Serial.begin(9600);
}

void loop() {
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
   delay(2000);
}
