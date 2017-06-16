void setup() {
    pinMode(D0, INPUT); //Sensor output connection pin
    pinMode(D7, OUTPUT);
}

void loop() {
    if (digitalRead(D0) == HIGH) {   //Check to see if the sensor is active (HIGH)
        Particle.publish("MotionAlert", "high", 60, PRIVATE); //Push the alert to the particle cloud
        digitalWrite(D7, HIGH); //Visual check on Particle to see if the sensor has been triggered using onboard LED
        while (digitalRead(D0) == HIGH); // hang tight here until motion stops
    }
}
