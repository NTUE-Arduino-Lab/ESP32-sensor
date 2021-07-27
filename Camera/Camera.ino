#include <esp_camera.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>

#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

uint16_t dmaBuffer1[16 * 16]; // Toggle buffer for 16*16 MCU block, 512bytes
uint16_t dmaBuffer2[16 * 16]; // Toggle buffer for 16*16 MCU block, 512bytes
uint16_t *dmaBufferPtr = dmaBuffer1;
bool dmaBufferSel = 0;

TFT_eSPI tft = TFT_eSPI();

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap)
{
	if (y >= tft.height())
		return 0;
	if (dmaBufferSel)
		dmaBufferPtr = dmaBuffer2;
	else
		dmaBufferPtr = dmaBuffer1;
	dmaBufferSel = !dmaBufferSel;
	tft.pushImageDMA(x, y, w, h, bitmap, dmaBufferPtr);
	return 1;
}

void setup()
{
	Serial.begin(115200);
	Serial.setDebugOutput(true);
	Serial.println();
	camera_config_t config;
	config.ledc_channel = LEDC_CHANNEL_0;
	config.ledc_timer = LEDC_TIMER_0;
	config.pin_d0 = Y2_GPIO_NUM;
	config.pin_d1 = Y3_GPIO_NUM;
	config.pin_d2 = Y4_GPIO_NUM;
	config.pin_d3 = Y5_GPIO_NUM;
	config.pin_d4 = Y6_GPIO_NUM;
	config.pin_d5 = Y7_GPIO_NUM;
	config.pin_d6 = Y8_GPIO_NUM;
	config.pin_d7 = Y9_GPIO_NUM;
	config.pin_xclk = XCLK_GPIO_NUM;
	config.pin_pclk = PCLK_GPIO_NUM;
	config.pin_vsync = VSYNC_GPIO_NUM;
	config.pin_href = HREF_GPIO_NUM;
	config.pin_sscb_sda = SIOD_GPIO_NUM;
	config.pin_sscb_scl = SIOC_GPIO_NUM;
	config.pin_pwdn = PWDN_GPIO_NUM;
	config.pin_reset = RESET_GPIO_NUM;
	config.xclk_freq_hz = 10000000;
	config.pixel_format = PIXFORMAT_JPEG;
	config.frame_size = FRAMESIZE_QVGA;
	config.jpeg_quality = 10;
	config.fb_count = 2;

	// camera init
	esp_err_t err = esp_camera_init(&config);
	if (err != ESP_OK)
	{
		Serial.printf("Camera init failed with error 0x%x", err);
		return;
	}

	sensor_t *s = esp_camera_sensor_get();
	s->set_contrast(s, 2);	 // -2 to 2
	s->set_saturation(s, 1); // -2 to 2

	// Initialise
	tft.begin();
	tft.setTextColor(TFT_WHITE, TFT_BLACK);
	tft.fillScreen(TFT_BLACK);
	tft.setTextColor(TFT_ORANGE, TFT_BLACK);
	tft.setRotation(3); //1:landscape 3:inv. landscape

	tft.initDMA(); // To use SPI DMA you must call initDMA() to setup the DMA engine
	// The jpeg image can be scaled down by a factor of 1, 2, 4, or 8
	TJpgDec.setJpgScale(1);
	tft.setSwapBytes(true);
	TJpgDec.setCallback(tft_output);
}

void loop()
{
	camera_fb_t *fb = NULL;
	esp_err_t res = ESP_OK;
	fb = esp_camera_fb_get();
	if (!fb)
	{
		Serial.println("Camera capture failed");
		esp_camera_fb_return(fb);
		return;
	}

	size_t fb_len = 0;
	if (fb->format != PIXFORMAT_JPEG)
	{
		Serial.println("Non-JPEG data not implemented");
		return;
	}
	tft.startWrite();
	TJpgDec.drawJpg(0, 0, fb->buf, fb->len);
	tft.endWrite();
	esp_camera_fb_return(fb);
}