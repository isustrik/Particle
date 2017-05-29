#define led_pin D7  //LED Pin for troubleshooting
#define mic_pin A0 //Pin for sound sensor
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;

void setup() {
    pinMode(led_pin, OUTPUT); //LED output for troubleshooting
}

void loop() {
    unsigned long startMillis= millis();  // Start of sample window
    unsigned int peakToPeak = 0;   // peak-to-peak level
    unsigned int signalMax = 0;
    unsigned int signalMin = 1024;
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
}
