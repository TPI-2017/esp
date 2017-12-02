#pragma once
#include "protocolo/Message.h"

extern "C" {
#include "osapi.h"
#include "user_interface.h"
#include "gpio.h"
#include "eagle_soc.h"
#include <stdint.h>
}

class LEDSign {
public:
	static void init();
	static void readJumpers();
	static void setEnabled(bool enabled);
	static void messageChanged(const char *text, uint8_t brate, int8_t srate);

private:

	// Limpia todos los carteles. En principio se debe llamar a éste método
	// antes de realizar un seteo de caracteres.
	static void clearScreen();

	// Se envia el mismo comando a todos los carteles. Se debe utilizar solo 
	// comandos para configurar los MAX7219 y no para setear letras ya que la 
	// misma se repetirá en cada cartel.
	static void sendCommand(uint16_t command);

	// Se envia una palabra hacia el primer MAX7219, en caso de que éste ya
	// tuviese un comando, lo que se hace es enviar su comando hacia el proximo
	// manejador y recibir el que se envia por parámetro. Para que este método
	// funcione, se debe bajar el LATCH antes de llamarlo y subirlo cuando
	// retorne.
	static void shiftOut(uint16_t word);

	static bool mOn;

	static void timerCallback(void *args);
	static void blinkTimerCallback(void *args);
	static os_timer_t mTimer;
	static os_timer_t mBlinkTimer;

	static constexpr uint8_t MAX_COLUMNS = 8;
	static constexpr uint8_t MAX_LETTERS = 15;

	static uint8_t mLetterCount;
	static uint8_t mCommandBuffer[MAX_COLUMNS * MAX_LETTERS];

	static uint8_t mBlinkRate;
	static int8_t mSlideRate;
	static uint8_t mTextLength;
	static char mMessage[Message::MESSAGE_SIZE + 1];
	
	static uint8_t mLetterIndex;
	static uint8_t mColumnIndex;
};
