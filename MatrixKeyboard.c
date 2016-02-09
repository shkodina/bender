#include <avr/io.h>
#include "MatrixKeyboard.h"


// Ниже идут макросы полуавтоматического конфигурирования кода.
// Предполагается, что их для конфигурирования кода трогать не
// придется. Однако комменты к ним я на всякий случай накидаю.
// Так сказать, для лучшего понимания.


// Эти два макроса позволяют из буквы, определяющей имя порта
// (например A, B и т.п.) сформировать имя порта вида PORTA.
#define PRECOMP2_PORT(x)		PORT ## x
#define PRECOMP_PORT(x)			PRECOMP2_PORT(x)

// Эти два макроса позволяют из буквы, определяющей имя порта
// (например A, B и т.п.) сформировать имя регистра, управляющего
// направлением данных порта вида DDRA.
#define PRECOMP2_DDR(x)			DDR ## x
#define PRECOMP_DDR(x)			PRECOMP2_DDR(x)

// Эти два макроса позволяют из буквы, определяющей имя порта
// (например A, B и т.п.) сформировать имя регистра, из которого
// будут считываться сотояния входных линий, имеющего вид PINA.
#define PRECOMP2_PIN(x)			PIN ## x
#define PRECOMP_PIN(x)			PRECOMP2_PIN(x)


// Определяем имена регистров для считывающего порта.
#define KBI_READ_PORT			PRECOMP_PORT(KB_READ_PORT)
#define KBI_READ_DDR			PRECOMP_DDR(KB_READ_PORT)
#define KBI_READ_PIN			PRECOMP_PIN(KB_READ_PORT)

// Определяем имена регистров для сканирующего порта.
#define KBI_SCAN_PORT			PRECOMP_PORT(KB_SCAN_PORT)
#define KBI_SCAN_DDR			PRECOMP_DDR(KB_SCAN_PORT)


// Это мега извратный способ сформировать таблицу масок для
// сканирующего порта. Предположим, что маска KB_SCAN_LINES_MASK
// равна 0b11110000. Этот макрос сформирует из нее массив из
// следующих значений:
//    0b11100000
//    0b11010000
//    0b10110000
//    0b01110000
// Глаз режет, зато все формируется автоматом, что исключает
// ошибку со стороны замученного программиста.
unsigned char KBI_SCAN_LINES_MASKS[KB_SCAN_LINES_COUNT] = {

#if KB_SCAN_LINES_MASK & 0x01
	KB_SCAN_LINES_MASK - 0x01,
#endif

#if KB_SCAN_LINES_MASK & 0x02
	KB_SCAN_LINES_MASK - 0x02,
#endif

#if KB_SCAN_LINES_MASK & 0x04
	KB_SCAN_LINES_MASK - 0x04,
#endif

#if KB_SCAN_LINES_MASK & 0x08
	KB_SCAN_LINES_MASK - 0x08,
#endif

#if KB_SCAN_LINES_MASK & 0x10
	KB_SCAN_LINES_MASK - 0x10,
#endif

#if KB_SCAN_LINES_MASK & 0x20
	KB_SCAN_LINES_MASK - 0x20,
#endif

#if KB_SCAN_LINES_MASK & 0x40
	KB_SCAN_LINES_MASK - 0x40,
#endif

#if KB_SCAN_LINES_MASK & 0x80
	KB_SCAN_LINES_MASK - 0x80,
#endif

};



// Все. Прилюдия окончена. Теперь несколько коротких функций,
// ради которых мы стока всего подготовили.

// Эта функция выполняет инициализацию регистров клавиатуры.
void InitializeKeyboard()
{
	// Считывающие линии конфигурируем как вход.
	KBI_READ_DDR &= ~KB_READ_LINES_MASK;

	// Сканирующие линии конфигурируем как выход.
	KBI_SCAN_DDR |= KB_SCAN_LINES_MASK;

	// Подключаем подтягивающие резисторы к считывающим линиям.
	KBI_READ_PORT |= KB_READ_LINES_MASK;
}


// Эта функция опрашивает клавиатуру и возвращает маску нажатых
// клавиш. То есть если в возвращенном значении установлен бит 0,
// значит была нажата клавиша с номером 0. И так далее.
KEYS ReadFromKeyboard()
{
	KEYS result = 0;

	for (int i = 0; i < KB_SCAN_LINES_COUNT; i++)
	{
		KBI_SCAN_PORT &= ~(KB_SCAN_LINES_MASK);
		KBI_SCAN_PORT |= KBI_SCAN_LINES_MASKS[i];

		unsigned char tmpResult = (unsigned char)(~(KBI_READ_PIN & KB_READ_LINES_MASK) & KB_READ_LINES_MASK);
		result |= (KEYS)tmpResult << (KB_READ_LINES_COUNT * i);
	}

	return result;
}
