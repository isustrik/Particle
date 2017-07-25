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

Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK);

#define led_pin D7

#define TOKEN "...."  // Put your Specific Token Ubidots token here

Ubidots ubidots(TOKEN);

int doorPin[] = {C0,C1}; //C0=Front Door; C1=Rear Door; etc
int doorStatus[2]; //Is the door open, 1 = Open; 0 = Closed
int pinCount = 2; //Number of door pins that are in use
int counter = 0; //Counter for 1min readings
int avgcounter = 0; //Counter for 20min readings
int sendFlag = 0;
int uploadInterval = 30; //Number of minutes to average before uploading; 1 to 60; if higher adjust the 3 arrays following to match
float tempArray[60]; //Array to hold [uploadInterval] temperature readings for averaging
float tempavg; //Average Temperature over uploadInterval min
float humArray[60]; //Array to hold uploadInterval humidity readings for averaging
float humavg; //Average Humidity over uploadInterval min
float presArray[60]; //Array to hold uploadInterval pressure readings for averaging
float presavg; //Average Pressure over uploadInterval min


void setup() {
    pinMode(led_pin, OUTPUT); //LED Pin set as output
    Serial.begin(9600);
    for (int thisPin = 0; thisPin < pinCount; thisPin++) { //initialize all the reed switch pins and turn on their pullup resistors
        pinMode(doorPin[thisPin],INPUT_PULLUP);
        if (digitalRead(doorPin[thisPin]) == HIGH) { //This if-else loop sets the initial Status array based on the door position at boot
            doorStatus[thisPin] = 1; //Set the door to Open
            if (thisPin == 0) {
                ubidots.add("Front_Door",doorStatus[thisPin]);//Push that the door is open
                } else {
                    ubidots.add("Rear_Door",doorStatus[thisPin]);
                }
        } else {
                doorStatus[thisPin] = 0; //Set the door to Closed
                if (thisPin == 0) {
                    ubidots.add("Front_Door",doorStatus[thisPin]);//Push that the door is open
                    } else {
                        Serial.println("Rear Door Init Closed");
                        ubidots.add("Rear_Door",doorStatus[thisPin]);
                    }
            }
    }
    if (!bme.begin()) {  //Initialize the BME280 sensor package
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }
    float temp = bme.readTemperature(); //Reads in Celcius
    float pres = bme.readPressure()/1000; //Reads in kilo Pascals
    float hum = bme.readHumidity(); //Reads in %RH
    ubidots.add("Temp",temp);
    ubidots.add("Pressure",pres);
    ubidots.add("Humidity",hum);    
    ubidots.sendAll(); //Send all initialization data at once to the Ubidot Cloud
}

void loop() { //Main Program Loop
    door_scan();
    counter++;
    if (counter==30) { //Every 1min get stats from the BME280 Sensor package
        environmentals();
        counter = 0; //Reset the counter to start a new 20min count down
    }
    delay(2000); //Wait 2 seconds before checking again
}

void door_scan() { //Door Check Function checks to see in a door has been opened or closed
    for (int dPin = pinCount - 1; dPin >=0; dPin--) {
        if (digitalRead(doorPin[dPin]) == HIGH && doorStatus[dPin] == 0) {   //Check to see if the door is open (HIGH) and was not previously open
            doorStatus[dPin] = 1;
            if (dPin == 0) {
                Serial.println("Front Door Open");
                ubidots.add("Front_Door",doorStatus[dPin]);//Push that the door is open
                sendFlag = 1;
              } else {
                    Serial.println("Rear Door Open");
                    ubidots.add("Rear_Door",doorStatus[dPin]);
                    sendFlag = 1;
                    }
        }
            else if (digitalRead(doorPin[dPin]) == LOW && doorStatus[dPin] == 1) { //Check to see if the door that was previously opened is now closed
                doorStatus[dPin] = 0;
                if (dPin == 0) {
                    Serial.println("Front Door Closed");
                    ubidots.add("Front_Door",doorStatus[dPin]);//Push that the door is open
                    sendFlag = 1;
                } else {
                        Serial.println("Rear Door Closed");
                        ubidots.add("Rear_Door",doorStatus[dPin]);
                        sendFlag = 1;
                }
            }
        
    }
    if (sendFlag==1) { //If any of the door statuses change, upload all changes to the Ubi Cloud
        ubidots.sendAll();
        sendFlag = 0;
    }
}

void environmentals() { //Environmental Data Function
    tempArray[avgcounter] = bme.readTemperature(); //Reads in Celcius
    presArray[avgcounter] = bme.readPressure()/1000; //Reads in Pascals
    humArray[avgcounter] = bme.readHumidity(); //Reads in %RH
    if (avgcounter == (uploadInterval - 1)) {
        average();
    } else {
        avgcounter++;
    }
}

void average() {
    tempavg=0;
    humavg=0;
    presavg=0;
    for (int a=0; a <= (uploadInterval - 1); a++) {
        tempavg = tempavg + tempArray[a];
        humavg = humavg + humArray[a];
        presavg = presavg + presArray[a];
    }
    tempavg = tempavg/uploadInterval;
    humavg = humavg/uploadInterval;
    presavg = presavg/uploadInterval;
    ubidots.add("Temp",tempavg);
    ubidots.add("Pressure",presavg);
    ubidots.add("Humidity",humavg);
    ubidots.sendAll();
    avgcounter = 0; //Reset the average counter
    
}
