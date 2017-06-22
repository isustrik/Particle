/**************************************************************
This code utilizes multiple reed switches and the Adafruit
BME280 Sensor Package. It will check the doors every two
seconds and upload any changes to the Ubicloud. It will also
take the temperature, humidity and barometric pressure every
twenty minutes and upload the results to the Ubicloud
**************************************************************/

//Make sure you manually link the following libraries to this project in the Particle IDE
#include <Adafruit_Sensor.h> // This #include statement was automatically added by the Particle IDE.
#include <Adafruit_BME280.h> // This #include statement was automatically added by the Particle IDE.
#include <Ubidots.h> // This #include statement was automatically added by the Particle IDE.

#define BME_SCK D4
#define BME_MISO D3
#define BME_MOSI D2
#define BME_CS D5

Adafruit_BME280 bme;

#define led_pin D7

#define TOKEN "...."  // Put your specific Ubidots TOKEN here

Ubidots ubidots(TOKEN);

int doorPin[] = {C0,C1,C2,C3}; //C0=Man Door; C1=West Door; C2=Middle Door; C3=East Door;
int doorStatus[4]; //Is the door open, 1 = Open; 0 = Closed
int pinCount = 4; //Number of door pins that are in use
int counter = 0;
float temp; //Hold temperature in Celcius
float pres; //Holds pressure in Pascals
float hum; //Holds humidity in % Relative Humidity


void setup() {
    for (int thisPin = 0; thisPin < pinCount; thisPin++) { //initialize all the reed switch pins and turn on their pullup resistors
        pinMode(doorPin[thisPin],INPUT_PULLUP);
        if (digitalRead(doorPin[thisPin]) == HIGH) { //This if-else loop sets the initial Status array based on the door position at boot
            doorStatus[thisPin] = 1; //Set the door to Open
            if (thisPin == 0) {
                ubidots.add("Man_Door",doorStatus[thisPin]);//Push that the door is open
            } else if (thisPin == 1) {
                ubidots.add("East_Door",doorStatus[thisPin]);
                } else if (thisPin == 2) {
                    ubidots.add("Middle_Door",doorStatus[thisPin]);
                    } else {
                        ubidots.add("West_Door",doorStatus[thisPin]);
                    }
        } else {
                doorStatus[thisPin] = 0; //Set the door to Closed
                if (thisPin == 0) {
                    ubidots.add("Man_Door",doorStatus[thisPin]);//Push that the door is open
                } else if (thisPin == 1) {
                    ubidots.add("East_Door",doorStatus[thisPin]);
                    } else if (thisPin == 2) {
                        ubidots.add("Middle_Door",doorStatus[thisPin]);
                        } else {
                            ubidots.add("West_Door",doorStatus[thisPin]);
                        }
            }
    }
    pinMode(led_pin, OUTPUT); //LED Pin set as output
    Serial.begin(9600);
    Adafruit_BME280 bme; // I2C
    if (!bme.begin()) {  //Initialize the BME280 sensor package
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }
    environmentals();
    ubidots.sendAll(); //Send all data at once to the Ubidot Cloud
}

void loop() { //Main Program Loop
    door_scan();
    counter++;
    if (counter==600) { //Every 20min check and upload the numbers from the BME280 Sensor package
        environmentals();
        ubidots.sendAll(); //Send all data at once to the Ubidot Cloud
        counter = 0; //Reset the counter to start a new 20min count down
    }
    delay(2000); //Wait 2 seconds before checking again
}

void door_scan() { //Door Check Function checks to see in a door has been opened or closed
    for (int dPin = pinCount - 1; dPin >=0; dPin--) {
        if (digitalRead(doorPin[dPin]) == HIGH && doorStatus[dPin] == 0) {   //Check to see if the door is open (HIGH) and was not previously open
            doorStatus[dPin] = 1;
            if (dPin == 0) {
                Serial.println("Man Door Open");
                ubidots.add("Man_Door",doorStatus[dPin]);//Push that the door is open
                ubidots.sendAll(); //Send all data at once to the Ubidot Cloud
            } else if (dPin == 1) {
                Serial.println("East Door Open");
                ubidots.add("East_Door",doorStatus[dPin]);
                ubidots.sendAll(); //Send all data at once to the Ubidot Cloud
                } else if (dPin == 2) {
                    Serial.println("Middle Door Open");
                    ubidots.add("Middle_Door",doorStatus[dPin]);
                    ubidots.sendAll(); //Send all data at once to the Ubidot Cloud
                    } else {
                        Serial.println("West Door Open");
                        ubidots.add("West_Door",doorStatus[dPin]);
                        ubidots.sendAll(); //Send all data at once to the Ubidot Cloud
                    }
        }
            else if (digitalRead(doorPin[dPin]) == LOW && doorStatus[dPin] == 1) { //Check to see if the door that was previously opened is now closed
                doorStatus[dPin] = 0;
                if (dPin == 0) {
                    Serial.println("Man Door Closed");
                    ubidots.add("Man_Door",doorStatus[dPin]);//Push that the door is open
                    ubidots.sendAll(); //Send all data at once to the Ubidot Cloud
                } else if (dPin == 1) {
                    Serial.println("East Door Closed");
                    ubidots.add("East_Door",doorStatus[dPin]);
                    ubidots.sendAll(); //Send all data at once to the Ubidot Cloud
                    } else if (dPin == 2) {
                        Serial.println("Middle Door Closed");
                        ubidots.add("Middle_Door",doorStatus[dPin]);
                        ubidots.sendAll(); //Send all data at once to the Ubidot Cloud
                        } else {
                            Serial.println("West Door Closed");
                            ubidots.add("West_Door",doorStatus[dPin]);
                            ubidots.sendAll(); //Send all data at once to the Ubidot Cloud
                        }
            }
        
    }
}

void environmentals() { //Environmental Data Function
    temp = bme.readTemperature(); //Reads in Celcius
    pres = bme.readPressure(); //Reads in Pascals
    hum = bme.readHumidity(); //Reads in %RH
    ubidots.add("Grg_Temp",temp);
    ubidots.add("Grg_Pressure",pres);
    ubidots.add("Grg_Humidity",hum);
}
