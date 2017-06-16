#define door_pin D6
#define led_pin D7

void setup() {
    pinMode(door_pin, INPUT_PULLUP); //Sensor input connection pin
    pinMode(led_pin, OUTPUT); //LED Pin set as output
}

void loop() {
    if (digitalRead(door_pin) == HIGH) {   //Check to see if the door is open (HIGH)
        Particle.publish("Door_Status", "open", 60, PRIVATE); //Push the alert to the particle cloud
        digitalWrite(led_pin, HIGH); //Visual check on Particle to see if the sensor has been triggered using onboard LED
        while (digitalRead(door_pin) == HIGH); // hang tight here while the door is open
        digitalWrite(led_pin, LOW); //Turn off the LED when the door closes
        Particle.publish("Door_Status", "closed", 60, PRIVATE); //Push the alert to the particle cloud
    }
}
