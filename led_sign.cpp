#include "led_sign.h"
#include "strings.h"

extern "C" {
#include "osapi.h"
#include "user_interface.h"
#include "espmissingincludes.h"
}

float static floating(uint8_t num)
{
	return static_cast<float>(num) / 16.0;
}

extern uint8_t _binary_font_dat_start[256][8];
#define font8x8_basic _binary_font_dat_start

#define DOUT 	5
#define CLK 	0
#define LATCH 	4
#define JP1 	10
#define JP2 	13
#define JP3 	12
#define JP4 	14

bool       LEDSign::mOn;
os_timer_t LEDSign::mTimer;
os_timer_t LEDSign::mBlinkTimer;

uint8_t    LEDSign::mLetterCount;
uint8_t    LEDSign::mCommandBuffer[LEDSign::MAX_COLUMNS * LEDSign::MAX_LETTERS];

uint8_t    LEDSign::mBlinkRate;
int8_t     LEDSign::mSlideRate;
uint8_t    LEDSign::mTextLength;
char       LEDSign::mMessage[Message::MESSAGE_SIZE + 1];

uint8_t    LEDSign::mLetterIndex;
uint8_t    LEDSign::mColumnIndex;

static void countWithModule(uint8_t &seconds, uint8_t maxSeconds, uint8_t &minutes, uint8_t maxMinutes, bool ascnd);

void ICACHE_FLASH_ATTR LEDSign::init()
{
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5); // Data Out
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0); // CLK
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4); // LATCH

	gpio_output_set(0, 0, (1 << DOUT) | (1 << CLK) | (1 << LATCH), 0);

	GPIO_OUTPUT_SET(LATCH, 1);
	GPIO_OUTPUT_SET(CLK, 0);
	GPIO_OUTPUT_SET(DOUT, 0);

	readJumpers();

	sendCommand(0x0F00); // No test mode
	sendCommand(0x0C00); // Shutdown
	sendCommand(0x0900); // No decode
	sendCommand(0x0A06); // Medium intensity
	sendCommand(0x0B07); // Scan all columns
	clearScreen();
	sendCommand(0x0C01); // Turn on

	os_timer_setfn(&mBlinkTimer, blinkTimerCallback, nullptr);
}

void LEDSign::readJumpers()
{
	// PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA3_U, FUNC_GPIO10);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);
	
	// PIN_PULLUP_EN(PERIPHS_IO_MUX_SD_DATA3_U);
	PIN_PULLUP_EN(PERIPHS_IO_MUX_MTCK_U);
	PIN_PULLUP_EN(PERIPHS_IO_MUX_MTDI_U);
	PIN_PULLUP_EN(PERIPHS_IO_MUX_MTMS_U);

	// GPIO_DIS_OUTPUT(JP1);
	GPIO_DIS_OUTPUT(JP2);
	GPIO_DIS_OUTPUT(JP3);
	GPIO_DIS_OUTPUT(JP4);

	mLetterCount = GPIO_INPUT_GET(JP2) << 1;
	mLetterCount |= GPIO_INPUT_GET(JP3) << 2;
	mLetterCount |= GPIO_INPUT_GET(JP4) << 3;
	mLetterCount = 2;
}

void LEDSign::clearScreen()
{
	for (uint8_t i = 1; i <= MAX_COLUMNS; i++)
		sendCommand((i << 8) | 0x0000);
}

void LEDSign::sendCommand(uint16_t command)
{
	GPIO_OUTPUT_SET(LATCH, 0);
	os_delay_us(1);
	
	for (uint8_t i = 0; i < mLetterCount; i++)
		shiftOut(command);
	
	os_delay_us(1);
	GPIO_OUTPUT_SET(LATCH, 1);
	os_delay_us(1);
}

void LEDSign::shiftOut(uint16_t word)
{
	for (int i = 0; i < 16; i++) {
		GPIO_OUTPUT_SET(CLK, 0);
		char bit = (word & 0x8000) >> 15;
		os_delay_us(1);
		GPIO_OUTPUT_SET(DOUT, bit);
		os_delay_us(1);
		GPIO_OUTPUT_SET(CLK, 1);
		os_delay_us(1);
		word *= 2;
	}
	GPIO_OUTPUT_SET(CLK, 0);
	os_delay_us(1);
}

void LEDSign::messageChanged(const char *text, uint8_t brate, int8_t srate)
{
	os_printf("Text: %s", text);

	os_timer_disarm(&mTimer);
	uint8_t stringLength = os_strlen(text);
	strcpy_s(mMessage, stringLength + 1, text);

	uint8_t maxSize = (stringLength > mLetterCount) ? stringLength : mLetterCount;

	for(int8_t j = stringLength; j < maxSize; j++)
		mMessage[j] = ' ';
	mMessage[maxSize] = 0;
	
	mBlinkRate = brate;
	mSlideRate = srate;
	mTextLength = maxSize;
	mLetterIndex = mLetterCount - 1;
	mColumnIndex = MAX_COLUMNS - 1;


	if (mSlideRate == 0) {
		timerCallback(nullptr);
	} else {
		os_timer_setfn(&mTimer, timerCallback, nullptr);
		srate = (srate < 0) ? -srate : srate;
		uint16_t period = static_cast<uint16_t>(1000.0 / floating(srate));
		os_timer_arm(&mTimer, period, true);
	}

	if (mBlinkRate) {
		uint16_t period = static_cast<uint16_t>(1000.0 / floating(brate)) / 2;
		os_timer_arm(&mBlinkTimer, period, true);
	} else {
		os_timer_disarm(&mBlinkTimer);
		setEnabled(true);
	}

}

void LEDSign::blinkTimerCallback(void *args)
{
	setEnabled(!mOn);
}

void LEDSign::timerCallback(void *args)
{
	uint8_t i = mColumnIndex;
	uint8_t I = mLetterIndex;

	// Arma el buffer con los datos a transmitir por serie
	for (uint8_t j = 0; j < mLetterCount * MAX_COLUMNS; j++) {
		mCommandBuffer[j] = font8x8_basic[mMessage[I]][i]; 

		countWithModule(i, MAX_COLUMNS, I, mTextLength, false);		
	}

	for (uint8_t j = 1; j <= MAX_COLUMNS; j++) {
		GPIO_OUTPUT_SET(LATCH, 0);
		os_delay_us(1);
		
		for (uint8_t k = 0; k < mLetterCount; k++) {
			uint16_t word = ((MAX_COLUMNS - (j - 1)) << 8) | mCommandBuffer[(j - 1) + k * MAX_COLUMNS];
			shiftOut(word);
		}
		
		os_delay_us(1);
		GPIO_OUTPUT_SET(LATCH, 1);
		os_delay_us(1);
	}
	if (mSlideRate)
		countWithModule(mColumnIndex, MAX_COLUMNS, mLetterIndex, mTextLength, (mSlideRate < 0));

}

static void countWithModule(uint8_t &seconds, uint8_t maxSeconds, uint8_t &minutes, uint8_t maxMinutes, bool ascnd)
{
	if (ascnd) {
		if (seconds < maxSeconds - 1) {
			seconds++;
		} else {
			seconds = 0;
			if (minutes < maxMinutes - 1)
				minutes++;
			else
				minutes = 0;
		}
	} else {
		if (seconds > 0) {
			seconds--;
		} else {
			seconds = maxSeconds - 1;
			if (minutes > 0)
				minutes--;
			else
				minutes = maxMinutes - 1;
		}
	}
}

void LEDSign::setEnabled(bool enabled)
{
	mOn = enabled;
	sendCommand((enabled) ?  0x0C01 : 0x0C00);
}
