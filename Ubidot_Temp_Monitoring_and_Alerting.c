#include <Ubidots.h> // This #include statement was automatically added by the Particle IDE.
#include <Adafruit_DHT.h> // This #include statement was automatically added by the Particle IDE.

#define DHTPIN D6     // what pin we're connected to
#define LEDPIN D7
#define DHTTYPE DHT22		// DHT 22 (AM2302)
#define TOKEN "<token>"  //Put your Ubidots token between the double quotes

Ubidots ubidots(TOKEN);

DHT dht(DHTPIN, DHTTYPE);
int i;

void setup() {
	Serial.begin(9600); //Used for watching to ensure correct DHT22 operation, can be disabled when unit is deployed
	dht.begin(); //Start the DHT22 Sensor package
	i = 0;
}

void loop() {
	i = i + 1;
	delay(10000); // Wait a few seconds between measurements.

	// Reading temperature or humidity takes about 250 milliseconds!
	// Sensor readings may also be up to 2 seconds 'old' (its a 
	// very slow sensor)

	double h = dht.getHumidity(); //Read humidity as %
	float t1 = dht.getTempCelcius(); // Read temperature as Celsius
	Serial.print("t1=");
	Serial.println(t1);
	delay(10000);
	float t2 = dht.getTempCelcius();
	Serial.print("t2=");
	Serial.println(t2);
	delay(10000);
	float t3 = dht.getTempCelcius();
	Serial.print("t3=");
	Serial.println(t3);
	delay(10000);
	float t4 = dht.getTempCelcius();
	Serial.print("t4=");
	Serial.println(t4);
	delay(10000);
	float t5 = dht.getTempCelcius();
	Serial.print("t5=");
	Serial.println(t5);
	delay(10000);
	float t6 = dht.getTempCelcius();
	Serial.print("t6=");
	Serial.println(t6);
	if (isnan(t1) || isnan(t2) || isnan(t3) || isnan(t4) || isnan(t5) || isnan(t6)) {
		return; // if any of the 6 reads fail, try again.
	}
	double t = ((t1+t2+t3+t4+t5+t6)/6);
	Serial.print("t=");
	Serial.println(t);
	Serial.print("i=");
	Serial.println(i);	
	
	if (i >= 20) {  //upload a set of results every 20min
		ubidots.add("Temperature", t);
		ubidots.add("Humidity", h);
		ubidots.sendAll();
		i = 0;
		Serial.print("i reset=");
		Serial.println(i);
	}

	if (t < 10.0) {  //Check if Temperature is below 10
        	Particle.publish("TempAlert", "low", 60, PRIVATE); //Send alert via Particle Cloud
        	ubidots.add("Temperature", t); //Upload data so Ubidots can alert as well.
        	ubidots.add("Humidity", h);
        	ubidots.sendAll();
        	delay(1200000); //wait 20min before starting over to prevent alert spamming
        }
	else {
        	if (t > 28.0) { //Check if Temperature is above 28
			Particle.publish("TempAlert", "high", 60, PRIVATE); //Send alert via Particle Cloud
			ubidots.add("Temperature", t); //Upload data so Ubidots can alert as well.
			ubidots.add("Humidity", h);
			ubidots.sendAll();
			delay(1200000); //wait 20min before starting over to prevent alert spamming
            	}
        }
}