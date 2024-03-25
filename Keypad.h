/*
 * Keypad.h
 *
 *  Created on: Aug 5, 2023
 *      Author: ezhab
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

#include "stm32f4xx_hal.h"

/*** Redefine if necessary ***/
// антидребезг, мс. От него зависит период опроса keypad
#define DEBONCE                 50  // мс
#define TIM_KEYPAD              TIM1
#define HTIM_KEYPAD        	htim1
// системная частота, МГц
#define MYSYSCLOCK              16

// размер keypad. Раскладка клавиатуры задается в файле Keypad.c
#define LINESIZE           	4
#define ROWSIZE                 4

// пины keypad
#define LINE_A_PORT         GPIOC
#define LINE_A_PIN              0
#define LINE_B_PORT         GPIOC
#define LINE_B_PIN              1
#define LINE_C_PORT         GPIOC
#define LINE_C_PIN              2
#define LINE_D_PORT         GPIOC
#define LINE_D_PIN              3
#define ROW_1_PORT          GPIOC
#define ROW_1_PIN               5
#define ROW_2_PORT          GPIOC
#define ROW_2_PIN               6
#define ROW_3_PORT          GPIOC
#define ROW_3_PIN               8
#define ROW_4_PORT          GPIOC
#define ROW_4_PIN               9
/*** Redefine if necessary ***/

// настройка таймера для keypad - переполнение каждые DEBONCE мс
#define TIMER_PRESCALER MYSYSCLOCK * 100 - 1 // см. таблицу расчета частоты, не более 2^16: 65536
#define TIMER_PERIOD DEBONCE * 10 - 1         // см. таблицу расчета частоты, не более 2^16: 65536

extern TIM_HandleTypeDef HTIM_KEYPAD; // таймер для опроса кнопок

extern const char keymap[LINESIZE][ROWSIZE];
// Инициализация и настройка таймера keypad
void Keypad_timerInit();
char GetKey();
#endif /* KEYPAD_H_ */
