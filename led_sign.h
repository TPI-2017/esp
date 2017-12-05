#pragma once
#include "protocolo/Message.h"
#include "modulo_number.h"

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

	// Se envía el mismo comando a todos los carteles. Se debe utilizar solo 
	// comandos para configurar los MAX7219 y no para setear letras ya que la 
	// misma se repetirá en cada cartel.
	static void sendCommand(uint16_t command);

	// Se envía una palabra hacia el primer MAX7219, en caso de que éste ya
	// tuviese un comando, lo que se hace es enviar su comando hacia el próximo
	// manejador y recibir el que se envia por parámetro. Para que este método
	// funcione, se debe bajar el LATCH antes de llamarlo y subirlo cuando
	// retorne.
	static void shiftOut(uint16_t word);

	// Genera el bitmap que se va a mostrar en el cartel. Se tiene en cuenta
	// el desplazamiento actual al generar mapa de bits.
	static void generateBuffer();

	// Plasma el contenido del buffer en el cartel.
	static void updateMessage();

	static bool mOn;

	static void timerCallback(void *args);
	static void blinkTimerCallback(void *args);
	static os_timer_t mTimer;
	static os_timer_t mBlinkTimer;

	static constexpr uint8_t MAX_COLUMNS = 8;
	static constexpr uint8_t MAX_LETTERS = 15;

	static uint8_t mLetterCount;

	// Buffer con los datos de cada columna del cartel. Está ordenado de forma
	// que el primer byte corresponde a la columna 8 de la matriz mas lejana
	// al módulo maestro.
	static uint8_t mBitmapBuffer[MAX_COLUMNS * MAX_LETTERS];

	static uint8_t mBlinkRate;
	static int8_t mSlideRate;
	static uint8_t mTextLength;
	static char mMessage[Message::MESSAGE_SIZE + 1];
	
	// Índice de la columna del mensaje que se dibuja en la columna más a
	// izquierda del cartel.
	// Este índice se va a cambiando para lograr el desplazamiento
	static ModuloNumber<uint8_t> mColumnIndex;
};
