/**************************************************************
This code utilizes multiple reed switches and the DHT22
Sensor Package. It will check the doors every two seconds and
upload any changes to the Ubicloud. It will also take the
temperature and humidity every twenty minutes and upload the
results to the Ubicloud.
**************************************************************/

//Make sure you manually link the following libraries to this project in the Particle IDE
#include <Ubidots.h> // This #include statement was automatically added by the Particle IDE.
#include <Adafruit_DHT.h> // This #include statement was automatically added by the Particle IDE.

#define DHTPIN D6
#define DHTTYPE DHT22

#define TOKEN "...."  //Put your UBI token here

Ubidots ubidots(TOKEN);

DHT dht(DHTPIN, DHTTYPE);

int doorPin[] = {C0,C1,C2,C3}; //C0=Man Door; C1=West Door; C2=Middle Door; C3=East Door; etc...
int doorStatus[4]; //Is the door open, 1 = Open; 0 = Closed
int pinCount = 4; //Number of door pins that are in use
int counter = 0;
int avgcounter = 0;
int sendFlag = 0;
int uploadminutes = 30; //Number of minutes to average before uploading 0 to 60; if less frequent the two arrays following must be increased in kind
float tempArray[60]; //Hold temperature in Celcius
float humArray[60]; //Holds humidity in % Relative Humidity

void setup() {
    Serial.begin(9600);
    for (int thisPin = 0; thisPin < pinCount; thisPin++) { //initialize all the reed switch pins and turn on their pullup resistors
        pinMode(doorPin[thisPin],INPUT_PULLUP);
        if (digitalRead(doorPin[thisPin]) == HIGH) { //This if-else loop sets the initial Status array based on the door position at boot
            doorStatus[thisPin] = 1; //Set the door to Open
            if (thisPin == 0) {
                ubidots.add("Man_Door",doorStatus[thisPin]);//Push that the door is open
            } else if (thisPin == 1) {
                ubidots.add("West_Door",doorStatus[thisPin]);
                } else if (thisPin == 2) {
                    ubidots.add("Middle_Door",doorStatus[thisPin]);
                    } else {
                        ubidots.add("East_Door",doorStatus[thisPin]);
                    }
        } else {
                doorStatus[thisPin] = 0; //Set the door to Closed
                if (thisPin == 0) {
                    ubidots.add("Man_Door",doorStatus[thisPin]);//Push that the door is open
                } else if (thisPin == 1) {
                    ubidots.add("West_Door",doorStatus[thisPin]);
                    } else if (thisPin == 2) {
                        ubidots.add("Middle_Door",doorStatus[thisPin]);
                        } else {
                            ubidots.add("East_Door",doorStatus[thisPin]);
                        }
            }
    }
    dht.begin();
    float temp = dht.getTempCelcius(); //Reads in Celcius
    float hum = dht.getHumidity(); //Reads in %RH
    ubidots.add("Temp",temp);
    ubidots.add("Humidity",hum);
    ubidots.sendAll(); //Send all data at once to the Ubidot Cloud
}

void loop() { //Main Program Loop
    door_scan();
    counter++;
    if (counter==30) { //Every ~1min pull the numbers from the BME280 Sensor package
        environmentals();
        counter = 0; //Reset the counter to start a new 30min count down
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
                sendFlag = 1;
            } else if (dPin == 1) {
                    Serial.println("West Door Open");
                    ubidots.add("West_Door",doorStatus[dPin]);    
                sendFlag = 1;
                } else if (dPin == 2) {
                    Serial.println("Middle Door Open");
                    ubidots.add("Middle_Door",doorStatus[dPin]);
                    sendFlag = 1;
                    } else {
                        Serial.println("East Door Open");
                        ubidots.add("East_Door",doorStatus[dPin]);
                        sendFlag = 1;
                    }
        }
            else if (digitalRead(doorPin[dPin]) == LOW && doorStatus[dPin] == 1) { //Check to see if the door that was previously opened is now closed
                doorStatus[dPin] = 0;
                if (dPin == 0) {
                    Serial.println("Man Door Closed");
                    ubidots.add("Man_Door",doorStatus[dPin]);//Push that the door is open
                    sendFlag = 1;
                } else if (dPin == 1) {
                    Serial.println("West Door Closed");
                    ubidots.add("West_Door",doorStatus[dPin]);
                    sendFlag = 1;
                    } else if (dPin == 2) {
                        Serial.println("Middle Door Closed");
                        ubidots.add("Middle_Door",doorStatus[dPin]);
                        sendFlag = 1;
                        } else {
                            Serial.println("East Door Closed");
                            ubidots.add("East_Door",doorStatus[dPin]);
                            sendFlag = 1;
                        }
            }
        
    }
    if (sendFlag == 1) {
        ubidots.sendAll(); //Send all data at once to the Ubidot Cloud if there are any door changes
        sendFlag = 0;
    }
}

void environmentals() { //Environmental Data Function
    tempArray[avgcounter] = dht.getTempCelcius(); //Reads in Celcius
    humArray[avgcounter] = dht.getHumidity(); //Reads in %RH
    while (isnan(tempArray[avgcounter]) || isnan(humArray[avgcounter])) { //If the sensor fails to return a valid number, try again until it does
        tempArray[avgcounter] = dht.getTempCelcius(); //Reads in Celcius
        humArray[avgcounter] = dht.getHumidity(); //Reads in %RH
    }
    if (avgcounter==(uploadminutes - 1)) {
        average();
    } else {
        avgcounter++;
    }
    
}

void average() {
    float tempavg = 0;
    float humavg = 0;
    for (int a = 0; a <= (uploadminutes - 1); a++){
        tempavg = tempArray[a] + tempavg;
        humavg = humArray[a] + humavg;
    }
    tempavg = tempavg/uploadminutes;
    humavg = humavg/uploadminutes;
    ubidots.add("Temp",tempavg);
    ubidots.add("Humidity",humavg);
    ubidots.sendAll(); //Send all data at once to the Ubidot Cloud if there are any door changes
    avgcounter = 0;
}
