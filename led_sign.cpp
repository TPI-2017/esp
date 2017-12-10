#include "led_sign.h"
#include "strings.h"

extern "C" {
#include "osapi.h"
#include "user_interface.h"
#include "espmissingincludes.h"
}

extern uint8_t _binary_font_dat_start[256*8];
#define font8x8_basic _binary_font_dat_start

#define DOUT    5
#define CLK     0
#define LATCH   4
#define JP1    13
#define JP2    12
#define JP3    14

#define DELAY_US 1

#define HIGH 0
#define LOW  1

bool       LEDSign::mOn;
os_timer_t LEDSign::mTimer;
os_timer_t LEDSign::mBlinkTimer;

uint8_t    LEDSign::mLetterCount;
uint8_t    LEDSign::mBitmapBuffer[LEDSign::MAX_COLUMNS * LEDSign::MAX_LETTERS];

float      LEDSign::mBlinkRate;
float      LEDSign::mSlideRate;
uint8_t    LEDSign::mTextLength;
char       LEDSign::mMessage[Message::MESSAGE_SIZE + 1];

ModuloNumber<uint8_t> LEDSign::mColumnIndex(0,0);


void ICACHE_FLASH_ATTR LEDSign::init()
{
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5); // Data Out
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0); // CLK
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4); // LATCH

	// Configuramos estos pines como salida
	gpio_output_set(0, 0, (1 << DOUT) | (1 << CLK) | (1 << LATCH), 0);

	GPIO_OUTPUT_SET(LATCH, HIGH);
	GPIO_OUTPUT_SET(CLK, LOW);
	GPIO_OUTPUT_SET(DOUT, LOW);

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
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);
	
	PIN_PULLUP_EN(PERIPHS_IO_MUX_MTCK_U);
	PIN_PULLUP_EN(PERIPHS_IO_MUX_MTDI_U);
	PIN_PULLUP_EN(PERIPHS_IO_MUX_MTMS_U);

	GPIO_DIS_OUTPUT(JP1);
	GPIO_DIS_OUTPUT(JP2);
	GPIO_DIS_OUTPUT(JP3);

	mLetterCount = 1;

	os_printf("Utilizando %d carteles.\n", mLetterCount);
}

void LEDSign::clearScreen()
{
	for (uint8_t i = 1; i <= MAX_COLUMNS; i++)
		sendCommand((i << 8) | 0x0000);
}

void LEDSign::sendCommand(uint16_t command)
{
	GPIO_OUTPUT_SET(LATCH, LOW);
	os_delay_us(DELAY_US);
	
	for (uint8_t i = 0; i < mLetterCount; i++)
		shiftOut(command);
	
	os_delay_us(DELAY_US);
	GPIO_OUTPUT_SET(LATCH, HIGH);
	os_delay_us(DELAY_US);
}

void LEDSign::shiftOut(uint16_t word)
{
	for (int i = 0; i < 16; i++) {
		GPIO_OUTPUT_SET(CLK, LOW);
		char bit = (word & 0x8000) >> 15;
		os_delay_us(DELAY_US);
		if (bit)
			GPIO_OUTPUT_SET(DOUT, HIGH);
		else
			GPIO_OUTPUT_SET(DOUT, LOW);
		os_delay_us(DELAY_US);
		GPIO_OUTPUT_SET(CLK, HIGH);
		os_delay_us(DELAY_US);
		word *= 2;
	}
	GPIO_OUTPUT_SET(CLK, LOW);
	os_delay_us(DELAY_US);
}

void LEDSign::messageChanged(const char *text, float brate, float srate)
{

	mBlinkRate = brate;
	mSlideRate = srate;
	auto strLength = os_strlen(text);
	mTextLength = (strLength > mLetterCount) ? strLength : mLetterCount;
	uint8_t lastColumn = mTextLength * MAX_COLUMNS - 1;
	mColumnIndex = ModuloNumber<uint8_t>(lastColumn, lastColumn);
	os_strcpy(mMessage, text);

	// Rellenamos con espacios si fuera necesario
	for(int8_t j = strLength; j < mTextLength; j++)
		mMessage[j] = ' ';
	mMessage[mTextLength] = 0;

	// Configuramos el timer para el desplazamiento
	timerCallback(nullptr);
	os_timer_disarm(&mTimer);
	if (mSlideRate) {
		os_timer_setfn(&mTimer, timerCallback, nullptr);
		srate = (srate < 0.0) ? -1.0 * srate : srate;
		uint32_t period = static_cast<uint32_t>(1000.0 / srate);
		os_printf("Period: %u\n", period);
		os_timer_arm(&mTimer, period, true);
	}

	// Configuramos el timer para el parpadeo
	os_timer_disarm(&mBlinkTimer);
	if (mBlinkRate) {
		uint32_t period = static_cast<uint32_t>(500.0 / brate);
		os_printf("Period: %u\n", period);
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

// Copia las columnas tomandolas de derecha a izquierda y las almacena de manera
// que cuando se transmitan los valores por serie, se forme el mensaje de forma
// correcta en el cartel.
void LEDSign::generateBuffer()
{
	ModuloNumber<uint8_t> index = mColumnIndex;
	for (uint8_t j = 0; j < mLetterCount * MAX_COLUMNS; j++) {
		uint8_t letterIndex = mMessage[index.value() / MAX_COLUMNS];
		uint8_t columnIndex = index.value() % MAX_COLUMNS;
		mBitmapBuffer[j] = font8x8_basic[8 * letterIndex + columnIndex];
		index--;
	}
}

void LEDSign::updateMessage()
{
	for (uint8_t j = 1; j <= MAX_COLUMNS; j++) {
		GPIO_OUTPUT_SET(LATCH, LOW);
		os_delay_us(DELAY_US);
		
		for (uint8_t k = 0; k < mLetterCount; k++) {
			uint8_t regNumber = MAX_COLUMNS - (j - 1);
			uint8_t data = mBitmapBuffer[(j - 1) + k * MAX_COLUMNS];
			uint16_t word = regNumber << 8 | data;
			shiftOut(word);
		}
		
		os_delay_us(DELAY_US);
		GPIO_OUTPUT_SET(LATCH, HIGH);
		os_delay_us(DELAY_US);
	}

	if (mSlideRate > 0)
		mColumnIndex--;
	else if (mSlideRate < 0)
		mColumnIndex++;
}

void LEDSign::timerCallback(void *args)
{
	generateBuffer();
	updateMessage();
}

void LEDSign::setEnabled(bool enabled)
{
	mOn = enabled;
	sendCommand((enabled) ?  0x0C01 : 0x0C00);
}
