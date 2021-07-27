#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup()
{
	mlx.begin();
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //Start the OLED display
	display.clearDisplay();
	display.display();
}

void loop()
{
	display.clearDisplay();

	display.setTextSize(1);
	display.setTextColor(WHITE);
	display.setCursor(0, 4);
	display.println("Ambient");

	display.setTextSize(2);
	display.setCursor(50, 0);
	display.println(mlx.readAmbientTempC(), 1);

	display.setCursor(110, 0);
	display.println("C");

	display.setTextSize(1);
	display.setTextColor(WHITE);
	display.setCursor(0, 20);
	display.println("Target");

	display.setTextSize(2);
	display.setCursor(50, 17);
	display.println(mlx.readObjectTempC(), 1);

	display.setCursor(110, 17);
	display.println("C");

	display.display();

	delay(1000);
}