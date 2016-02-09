#ifndef _MATRIXKEYBOARD_H_
#define _MATRIXKEYBOARD_H_

// Имя считывающего порта. Внимание, указывать только букву,
// например A, B  и тому подобное. По этому имени далее будут
// сформированы имена необходимых регистров.
#define KB_READ_PORT	F

// Количество считывающих линий.
#define KB_READ_LINES_COUNT	(4)

// Маска считывающих линий. Биты, установленные в 1, указывают на то,
// что соответствующая линия порта является считывающей. Например,
// если установлен третий бит, значит третья линия порта KB_READ_PORT
// является считывающей. И так далее. У меня считывающими будут линии
// с 0 до 3 включительно. Если KB_READ_PORT и KB_SCAN_PORT одинаковы,
// биты KB_READ_LINES_MASK и KB_SCAN_LINES_MASKS не должны пересекаться.
#define KB_READ_LINES_MASK	0b00001111

// Имя сканирующего порта. Внимание, указывать только букву,
// например A, B  и тому подобное. По этому имени далее будут
// сформированы имена необходимых регистров.
#define KB_SCAN_PORT	F

// Количество сканирующих линий.
#define KB_SCAN_LINES_COUNT	(4)

// Маска сканирующих линий. Биты, установленные в 1, указывают на то,
// что соответствующая линия порта является сканирующей. Например,
// если установлен третий бит, значит третья линия порта KB_SCAN_PORT
// является сканирующей. И так далее. У меня сканирующими будут линии
// с 4 до 7 включительно. Если KB_READ_PORT и KB_SCAN_PORT одинаковы,
// биты KB_READ_LINES_MASK и KB_SCAN_LINES_MASKS не должны пересекаться.
#define KB_SCAN_LINES_MASK	0b11110000


// Этот макрос позволяет выбрать оптимальный тип данных, возвращаемыц
// функцией ReadFromKeyboard. Это необходимо для оптимизации.
#if (KB_READ_LINES_COUNT * KB_SCAN_LINES_COUNT) <= 8
typedef unsigned char KEYS;
#elif (KB_READ_LINES_COUNT * KB_SCAN_LINES_COUNT) <= 16
typedef unsigned short KEYS;
#elif (KB_READ_LINES_COUNT * KB_SCAN_LINES_COUNT) <= 32
typedef unsigned long KEYS;
#elif (KB_READ_LINES_COUNT * KB_SCAN_LINES_COUNT) <= 64
typedef unsigned long long KEYS;
#else
// Этот код не поддерживает более 64 клавиш, но вы можете его
// модифицировать.
#error "Wow! This source code not supported more that 64 keys."
#endif


// Эта функция выполняет инициализацию регистров клавиатуры.
void InitializeKeyboard();

// Эта функция опрашивает клавиатуру и возвращает маску нажатых
// клавиш. То есть если в возвращенном значении установлен бит 0,
// значит была нажата клавиша с номером 0. И так далее.
KEYS ReadFromKeyboard();

#endif // _MATRIXKEYBOARD_H_
