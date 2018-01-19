// These #include statement were automatically added by the Particle IDE.
#include <Ubidots.h>
#include <Adafruit_DHT.h>
#include <HC_SR04.h>

#define DHTPIN D6
#define DHTTYPE DHT22
#define trigPin D1
#define echoPin D2
#define TOKEN "your token" //Your specific Ubitoken

Ubidots ubidots(TOKEN);

DHT dht(DHTPIN, DHTTYPE);

double inches2water = 0.0;
double tank = 0.0;
double temp = 0.0;
double hum = 0.0;
int uploadminutes = 5; //Number of minutes to average before uploading 0 to 60; if less frequent the two arrays following must be increased in kind
double tempArray[60]; //Hold temperature in Celcius
double humArray[60]; //Holds humidity in % Relative Humidity
int counter = 0;
int avgcounter = 0; //Take the average Temperature and Humidity as well as a Water Tank Reading


/*
Connect an HC-SR04 Range finder as follows:
Spark   HC-SR04
GND     GND
5V      VCC
D1     Trig
D2      Echo
 
The default usable rangefinder is 10cm to 250cm. Outside of that range -1 is returned as the distance.
 
You can change this range by supplying two extra parameters to the constructor of minCM and maxCM, like this:
 
HC_SR04 rangefinder = HC_SR04(trigPin, echoPin, 5.0, 300.0);
*/
 
HC_SR04 rangefinder = HC_SR04(trigPin, echoPin);
 
void setup() 
{
    Serial.begin(9600);
    dht.begin();
    delay(2000);
    initialize();
}
 
void loop() 
{
    counter++;
    if (counter==30) { //Every ~1min pull the numbers from the DHT Sensor package
        environmentals();
        counter = 0; //Reset the counter to start a new 30min count down
    }
    delay(2000);
}

void environmentals() { //Environmental Data Function
    tempArray[avgcounter] = dht.getTempCelcius(); //Reads in Celcius
    humArray[avgcounter] = dht.getHumidity(); //Reads in %RH
    if (avgcounter==(uploadminutes - 1)) {
        average();
    } else {
        avgcounter++;
    }
}

void average() {
    double tempavg = 0;
    double humavg = 0;
    for (int a = 0; a <= (uploadminutes - 1); a++){
        tempavg = tempArray[a] + tempavg;
        humavg = humArray[a] + humavg;
    }
    tempavg = round((tempavg/uploadminutes)*100)/100;
    humavg = round((humavg/uploadminutes)*100)/100;
    ubidots.add("WT_Temp",tempavg);
    ubidots.add("WT_Humidity",humavg);
    Particle.variable("Temp", tempavg);
    Particle.variable("Hum", humavg);
    watertank();  //Take a water tank reading
    ubidots.sendAll(); //Send all data at once to the Ubidot Cloud
    avgcounter = 0;
}

void watertank() {
    inches2water = rangefinder.getDistanceInch();
    inches2water = round(inches2water*10)/10;
    if (inches2water < 11.0) {
        tank = 100.0;
    }
        else if (inches2water > 45.0) {
            tank = 0.0;
        }
            else {
                tank = round((100-((inches2water/45)*100))*10)/10;
            } 
    ubidots.add("WT_Tank",tank);
    ubidots.add("WT_Inches",inches2water);
    Particle.variable("Tank", tank);
    Particle.variable("I2W", inches2water);
}

void initialize() {
    temp = round(dht.getTempCelcius()*100)/100; //Reads in Celcius
    hum = round(dht.getHumidity()*100)/100; //Reads in %RH
    Particle.variable("Temp", temp);
    Particle.variable("Hum", hum);
    ubidots.add("WT_Temp",temp);
    ubidots.add("WT_Humidity",hum);
    watertank();
    ubidots.sendAll(); //Send all data at once to the Ubidot Cloud
}
