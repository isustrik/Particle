/**************************************************************************
 * This program monitors a DHT22 sensor for Temperature and Humidity.
 * The DHT Sensor is checked every 10 seconds for 1 minute and the
 * readings are then checked to ensure the readings are valid numbers
 * and if not it discards the reading and tries again. If the readings
 * are good, the values are averaged and then checked to see if the
 * average is between +10°C and 28°C. If the value is outside of this
 * range the program immediatly sends the values to Ubidots where
 * alerting via email and SMS will occur. If the readings are inside of
 * the acceptable range, the program continues to monitor the temperature,
 * uploading the 20 minute average every 20 minutes to Ubidots for
 * charting and logging.
**************************************************************************/
#include <Ubidots.h> // This #include statement was automatically added by the Particle IDE.
#include <Adafruit_DHT.h> // This #include statement was automatically added by the Particle IDE.

#define DHTPIN D6     // what pin we're connecting the DHT22 to
#define DHTTYPE DHT22		// DHT 22 (AM2302)
#define TOKEN "...."  //Put your Ubidots TOKEN here between the ""

Ubidots ubidots(TOKEN);


DHT dht(DHTPIN, DHTTYPE);
int i; //Integer for the 20min loop
int t; //Integer for the 1min loop
float temp[6]; //Array to store six temperature readings over the course of 1 minute
float hum[6]; //Array to store six humidity readings over the course of 1 minute
float hum20[20]; //Array to store the average humidity values over 20 minutes
float temp20[20]; //Array to store the average temperature values over 20 minutes 
float hum1avg; //Float for the humidity 1 minute average
float temp1avg; //Float for the temperature 1 minute average
float hum20avg; //Float for the humidity 20 minute average
float temp20avg; //Float for the temperature 20 minute average

void setup() {
    Serial.begin(9600);
	dht.begin();
	i = 0;
	hum20avg = 0.0;
	temp20avg = 0.0;
}

void loop() {
    hum1avg = 0.0;
	temp1avg = 0.0;
    for (int t=0; t<=5; t++) { //This loop takes 6 sensor readings 10sec apart
        delay(10000);
        temp[t] = dht.getTempCelcius();
        while (isnan(temp[t])) {  //Check to see if the temperature is a valid number, if not take it again until it is
            temp[t] = dht.getTempCelcius();
        }
        hum[t] = dht.getHumidity();
        while (isnan(hum[t])) {  //Check to see if the humidity is a valid number, if not take it again until it is
            hum[t] = dht.getHumidity();
        }
    }
    for (int a = 0; a<=5; a++) { //This loop extracts the 1 minute arrays to start the average calcs
        hum1avg = hum1avg + hum[a];
        temp1avg = temp1avg + temp[a];
    }
    
    hum1avg = hum1avg/6; //Create the 1 minute average
    hum20[i] = hum1avg; //Store the 1 minute average in the array for creating the 20min average
    temp1avg = temp1avg/6; //Create the 1 minute average
    temp20[i] = temp1avg; //Store the 1 minute average in the array for creating the 20min average
    
    if (i == 19) {  //upload a set of results every 20min
        for (int a=0; a<=19; a++) {
            hum20avg = hum20avg + hum20[a];
            temp20avg = temp20avg + temp20[a];
        }
	    hum20avg = hum20avg/20;
	    temp20avg = temp20avg/20;
	    ubidots.add("Temperature", temp20avg);
        ubidots.add("Humidity", hum20avg);
        ubidots.sendAll();
	    i = 0; //Reset the 20min counter
	    hum20avg = 0.0; //Reset the 20min average
	    temp20avg = 0.0; //Reset the 20min average
	    }
    
    if (temp1avg < 10.0 || temp1avg > 28.0) {  //Check if Temperature is below 10 or above 28
        ubidots.add("Temperature", temp1avg); //Upload data so Ubidots can alert as well.
        ubidots.add("Humidity", hum1avg);
        ubidots.sendAll();
        delay(1200000); //wait 20min before starting over to prevent alert spamming
        }
    
    i = i + 1; //Increment the 20min counter
}
