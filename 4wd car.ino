#include <LiquidCrystal_I2C.h>
#include <Wire.h>	// letiltja a PWM-et D9-en és D10-en

#include <Servo.h>

//LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
Servo servo;

#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5
#define ENA 11
#define ENB 6

#define TRIGGER 9
#define ECHO 7
#define SERVO 8

float speedOfSound = 280.0;		// m/s
float pingTime;
unsigned distance;

bool criticalDistance = false;
unsigned distances_tmp[36];
unsigned distances[5];
unsigned maxDistance;
unsigned maxAngle;

// ==============================================================================================
void setup() {
//	Serial.begin(19200);

	pinMode(IN1, OUTPUT);
	pinMode(IN2, OUTPUT);
	pinMode(IN3, OUTPUT);
	pinMode(IN4, OUTPUT);

	pinMode(TRIGGER, OUTPUT);
	pinMode(ECHO, INPUT);

	servo.attach(SERVO);
	servo.write(90);
//	lcd.begin(16, 2);
}

// ============================================================================================
void start(){
	analogWrite(ENA, 94);
	analogWrite(ENB, 94);

	digitalWrite(IN1, LOW);
	digitalWrite(IN2, HIGH);
	digitalWrite(IN3, LOW);
	digitalWrite(IN4, HIGH);

	analogWrite(A0, 0);
	analogWrite(A1, 255);
}
void stop(){
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, LOW);
	digitalWrite(IN3, LOW);
	digitalWrite(IN4, LOW);

	analogWrite(A0, 0);
	for (int i = 0; i < 3; ++i)	{
		analogWrite(A1, 0);
		delay(120);
		analogWrite(A1, 255);
		delay(120);
	}
}
void error(){
	analogWrite(A1, 0);
	while(1){
		analogWrite(A0, 255);
	    delay(500);
	    analogWrite(A0, 0);
	    delay(500);
	}
}

unsigned measureDistance(){
	digitalWrite(TRIGGER, LOW);
	delay(2);
	digitalWrite(TRIGGER, HIGH);
	delayMicroseconds(10);
	digitalWrite(TRIGGER, LOW);

	pingTime = pulseIn(ECHO, HIGH);		// us
	return ((speedOfSound * pingTime) / 1000000 / 2 * 100);
}

unsigned array_average(){
	unsigned a = 0;

	for (int i = 0; i < 36; i++) {
		a += distances_tmp[i];
	}

	return (a / 36);
}

unsigned array_max(){
	unsigned m = distances[0];

	for (int i = 1; i < 5; i++)	{
		if (distances[i] > m) {
			m = distances[i];
		}
	}

	return m;
}

unsigned array_max_id(){
	unsigned max_id = 0;

	for (int i = 1; i < 5; i++)	{
		if (distances[i] > distances[max_id]) {
			max_id = i;
		}
	}

	return max_id;
}

// =============================================================================================
void loop() {
	delay(100);
	start();
//	distance = measureDistance();
	if (analogRead(A2) < 500) {
		criticalDistance = true;
	}

	if (criticalDistance == true /*distance <= 40*/) {
		criticalDistance = false;
		stop();

		delay(500);
		servo.write(180);
		delay(500);

		int j = 0;
		int k = 0;
		for (int i = 179; i >= 0; i--) {
			servo.write(i);

			distances_tmp[j] = measureDistance();
			j++;

			if (j == 36) {
				j = 0;
				distances[k] = array_average();
				k++;
			}
		}

		delay(200);
		servo.write(90);
		delay(500);

		if (array_max() < 15) {
			error();
		}
		else {
			analogWrite(ENA, 180);
			analogWrite(ENB, 180);

			switch (array_max_id()) {
				case 0:
					digitalWrite(IN1, HIGH);
					digitalWrite(IN2, LOW);
					delay(700);
					stop();
					break;
				case 1:
					digitalWrite(IN1, HIGH);
					digitalWrite(IN2, LOW);
					delay(350);
					stop();
					break;
				case 3:
					digitalWrite(IN3, HIGH);
					digitalWrite(IN4, LOW);
					delay(350);
					stop();
					break;
				case 4:
					digitalWrite(IN3, HIGH);
					digitalWrite(IN4, LOW);
					delay(700);
					stop();
					break;
				default:
					error();
					break;
			}
		}

	}	
	
	delay(250);
}