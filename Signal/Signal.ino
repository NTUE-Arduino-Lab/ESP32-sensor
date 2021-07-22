#include <Arduino.h>
#define STARTPIN 12
#define MAXPIN 22
#define SENSOR 26
void signalLevel(int signal)
{
	for(int i = STARTPIN; i < signal + STARTPIN; i++)
	{
		if(i >= 20)
		{
			digitalWrite(i+1, HIGH);
			continue;
		}
		digitalWrite(i,HIGH);
	}
	for (int i = signal + STARTPIN; i < MAXPIN; i++)
	{
		if(i >= 20)
		{
			digitalWrite(i+1, LOW);
			continue;
		}
		digitalWrite(i,LOW);
	}
	
}
void setup()
{
	Serial.begin(115200);
	for(int i = STARTPIN; i < MAXPIN; i++)
	{
		if(i >= 20)
		{
			pinMode(i+1, OUTPUT);
			continue;
		}
		pinMode(i,OUTPUT);
	}
}

void loop()
{
	int sensorData = (analogRead(SENSOR) - 2400) / 120; //自行調整倍率
	Serial.println(sensorData);
	signalLevel(sensorData);

}