/**************************************************************
This code utilizes the Adafruit BME280 Sensor Package. It will
check the temperature every minute then take an average at
the specified interval and upload to the ubidots cloud.
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

#define TOKEN "token"  //Put your Ubidots token here

Ubidots ubidots(TOKEN);

int counter = 0; //Counter for 1min readings
int avgcounter = 0; //Counter for 30min averaging
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
    if (!bme.begin()) {  //Initialize the BME280 sensor package
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }
    float temp = bme.readTemperature(); //Reads in Celcius
    float pres = bme.readPressure()/1000; //Reads in kilo Pascals
    float hum = bme.readHumidity(); //Reads in %RH
    ubidots.add("Temperature",temp);
    ubidots.add("Pressure",pres);
    ubidots.add("Humidity",hum);
    ubidots.sendAll(); //Send all initialization data at once to the Ubidot Cloud
    Serial.println("Initial Send...");
}

void loop() { //Main Program Loop
    environmentals();
    delay(60000); //Wait 1 minute before checking again
}

void environmentals() { //Environmental Data Function
    tempArray[avgcounter] = bme.readTemperature(); //Reads in Celcius
    presArray[avgcounter] = bme.readPressure()/1000; //Reads in Pascals
    humArray[avgcounter] = bme.readHumidity(); //Reads in %RH
    Serial.println(tempArray[avgcounter]);
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
    ubidots.add("Temperature",tempavg);
    ubidots.add("Pressure",presavg);
    ubidots.add("Humidity",humavg);
    ubidots.sendAll();
    Serial.println("Sending......");
    avgcounter = 0; //Reset the average counter
    
}
