#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Ticker.h>
#define WIDTH_3 3072.0
#define SCREEN_WIDTH 128												  // OLED display width, in pixels
#define SCREEN_HEIGHT 64												  // OLED display height, in pixels
#define OLED_RESET -1													  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //Declaring the display name (display)
struct tmpData
{
	double pmData;
};
struct tmpData data;
double bandWidth = WIDTH_3; //位寬調整(訊號佔據的寬度)
float coef = 700;			//係數誤差調整
int incomeByte[7];
int sensorData;
int z = 0;
int sum;
void doMainProcess()
{
	while (Serial.available() > 0)
	{

		sensorData = Serial.read();
		
		if (sensorData == 170)
		{
			z = 0;
			incomeByte[z] = sensorData;
		}
		else
		{
			z++;
			incomeByte[z] = sensorData;
		}
		if (z == 6)
		{
			sum = incomeByte[1] + incomeByte[2] + incomeByte[3] + incomeByte[4];

			if (incomeByte[5] == sum && incomeByte[6] == 255)
			{
				for (int k = 0; k < 7; k++)
				{
					Serial.print(incomeByte[k]);
					Serial.print("|");
				}

				Serial.print(" Vo=");
				float vo = (incomeByte[1] * 256.0 + incomeByte[2]) / bandWidth * 5.00;
				Serial.print(vo, 3);
				Serial.print("v | ");
				float c = vo * coef;

				Serial.print(" PM2.5 = ");
				Serial.print(c, 2);
				Serial.print("ug/m3 ");
				Serial.println();
				data.pmData = (double)c;
			}
			else
			{
				z = 0;
				Serial.flush();
				sensorData = 0;
				for (int m = 0; m < 7; m++)
				{
					incomeByte[m] = 0;
				}
			}
			z = 0;
		}
	}
}
void setup()
{
	Serial.begin(2400);
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //Start the OLED display
	display.display();
	delay(3000);
}
void loop()
{
	display.clearDisplay();
	doMainProcess();

	display.setTextSize(1);
	display.setTextColor(WHITE);
	display.setCursor(0, 4);
	display.println("PM2.5:");

	display.setTextSize(2);
	display.setCursor(50, 0);
	display.println(data.pmData, 1);

	display.display();

	delay(1000);
}