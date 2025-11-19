#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

const int TRIGPIN = 9;
const int ECHOPIN = 10;
const int LED=2;
double duration, distance;
LiquidCrystal_I2C lcd(0x27, 16, 2);




// Speed of sound in cm/µs at 20°C, dry air (can be adjusted for temperature/humidity)
const double SPEED_OF_SOUND_CM_PER_US = 0.0343; // Source: physics, 343 m/s = 0.0343 cm/µs


/**
 * @brief Perform one-time initialization for the program and hardware.
 *
 * This function is intended to run once at startup to configure the
 * environment required by the rest of the application. Typical tasks
 * performed here include:
 *  - configuring GPIO pin modes and pull-ups/pull-downs
 *  - initializing serial/UART, SPI, I2C, or other communication interfaces
 *  - initializing sensors, actuators, and peripheral libraries
 *  - setting initial values for global variables or application state
 */
void setup() {
	pinMode(TRIGPIN, OUTPUT);  
	pinMode(ECHOPIN, INPUT);
	pinMode(LED, OUTPUT);
	lcd.init();
	lcd.backlight();
	lcd.setCursor(0,0);
	Serial.begin(9600);
}

/**
 * loop()
 *
 * Periodically triggers an ultrasonic distance measurement and reports the result.
 *
 * Operation:
 *  - Ensures trigPIN is LOW briefly to clear any previous pulse.
 *  - Sends a 10µs HIGH pulse on trigPIN to trigger the ultrasonic sensor.
 *  - Measures the echo pulse width on echoPIN using pulseIn (microseconds).
 *  - Converts the measured duration to distance in centimeters using:
 *      distance = (duration * 0.0343) / 2
 *    (0.0343 cm/µs approximates the speed of sound; divide by two for round trip).
 *  - Prints "Distance: " and the computed distance to the Serial monitor.
 *  - Waits 50 ms before repeating.
 */
void loop() {

 	digitalWrite(TRIGPIN, LOW);  
	delayMicroseconds(2);  
	digitalWrite(TRIGPIN, HIGH);  
	delayMicroseconds(10);
	digitalWrite(TRIGPIN, LOW);
	duration = pulseIn(ECHOPIN, HIGH, 30000); // 30ms timeout
	if (duration == 0) {
		Serial.println("Warning: No echo received (timeout)");
		distance = -1; // Indicate invalid reading
	} else {
		distance = (duration * SPEED_OF_SOUND_CM_PER_US) / 2;
	}
   
  
	if (distance < 0) {
	lcd.clear();
	lcd.print("No echo");
	digitalWrite(LED, LOW); 
	return;
	}

	lcd.clear();
	lcd.print(distance);
	lcd.print("cm");
		delay(50);

	if(distance>=100){
		digitalWrite(LED,LOW);
	}else if(distance<100 && distance >= 10){
		digitalWrite(LED, HIGH);
		delay(distance*10);
		digitalWrite(LED, LOW);
	}else{
		digitalWrite(LED, HIGH);
	}



}

