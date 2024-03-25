/*
 * Keypad.c
 *
 *  Created on: Aug 5, 2023
 *      Author: ezhab
 */
#include "Keypad.h"
#include "stdatomic.h"

/*** Redefine if necessary ***/
const char keymap[LINESIZE][ROWSIZE] =
    {      // символы на клавиатуре
	    {
	    '1', '2', '3', 'A'
	    },
	    {
	    '4', '5', '6', 'B'
	    },
	    {
	    '7', '8', '9', 'C'
	    },
	    {
	    '*', '0', '#', 'D'
	    }
	};
volatile atomic_flag line_A_blocked = ATOMIC_FLAG_INIT
;
volatile atomic_flag line_B_blocked = ATOMIC_FLAG_INIT
;
volatile atomic_flag line_C_blocked = ATOMIC_FLAG_INIT
;
volatile atomic_flag line_D_blocked = ATOMIC_FLAG_INIT
;
/*** Redefine if necessary ***/

// вызвать перед циклом while
// настройка периода опроса таймера и старт таймера
void Keypad_timerInit()
    {
    TIM_ClockConfigTypeDef sClockSourceConfig =
	{
	0
	};
    TIM_MasterConfigTypeDef sMasterConfig =
	{
	0
	};

    HTIM_KEYPAD.Instance = TIM_KEYPAD;
    HTIM_KEYPAD.Init.Prescaler = TIMER_PRESCALER;
    HTIM_KEYPAD.Init.CounterMode = TIM_COUNTERMODE_UP;
    HTIM_KEYPAD.Init.Period = TIMER_PERIOD;
    HTIM_KEYPAD.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HTIM_KEYPAD.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Base_Init(&HTIM_KEYPAD);
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&HTIM_KEYPAD, &sClockSourceConfig);
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&HTIM_KEYPAD, &sMasterConfig);

    HAL_TIM_Base_Start_IT(&HTIM_KEYPAD);
    }

// опрос клавиатуры
// вызвать в обработчике прерывания по таймеру
char GetKey()
    {
    //atomic_flag_test_and_set(&line_B_blocked);
    char key = 0;
    static char oldKey;  // для предотвращения залипания/долгого нажатия

    // установка всех портов LINE на режим входа (reset state) [00]
    LINE_A_PORT->MODER &= ~(0b11 << (LINE_A_PIN * 2));         // сбрасываем бит
    LINE_B_PORT->MODER &= ~(0b11 << (LINE_B_PIN * 2));         // сбрасываем бит
    LINE_C_PORT->MODER &= ~(0b11 << (LINE_C_PIN * 2));         // сбрасываем бит
    LINE_D_PORT->MODER &= ~(0b11 << (LINE_D_PIN * 2));         // сбрасываем бит
    // опрос клавиатуры. Последовательно выставляем по одной строке в LOW

    /*----------- сканирование LINE A BEGIN ---------------------*/
    if (!atomic_flag_test_and_set(&line_A_blocked))
	{

	atomic_flag_test_and_set(&line_B_blocked);
	atomic_flag_test_and_set(&line_C_blocked);
	atomic_flag_test_and_set(&line_D_blocked);
	// LINE A в low
	// установка порта LINE A на режим выхода [01]
	LINE_A_PORT->MODER &= ~(0b11 << (LINE_A_PIN * 2));     // сбрасываем бит
	LINE_A_PORT->MODER |= (0b01 << (LINE_A_PIN * 2));   // устанавливаем бит
	// установка LINE A в ноль
	LINE_A_PORT->BSRR |= (1 << (LINE_A_PIN + 16));

	// считываем столбцы и "ловим" LOW. LOW - значит именно эта кнопка нажата
	if ((ROW_1_PORT->IDR & (1 << ROW_1_PIN)) == 0)
	    key = keymap[0][0];
	if ((ROW_2_PORT->IDR & (1 << ROW_2_PIN)) == 0)
	    key = keymap[0][1];
	if ((ROW_3_PORT->IDR & (1 << ROW_3_PIN)) == 0)
	    key = keymap[0][2];
	if ((ROW_4_PORT->IDR & (1 << ROW_4_PIN)) == 0)
	    key = keymap[0][3];

	// возвращаем порт LINE A в режим входа (reset state) [00]
	LINE_A_PORT->MODER &= ~(0b11 << (LINE_A_PIN * 2));     // сбрасываем бит
	atomic_flag_clear(&line_B_blocked);
	atomic_flag_clear(&line_C_blocked);
	atomic_flag_clear(&line_D_blocked);


	}
//    /*----------- сканирование LINE A END ---------------------*/
//
//    /*----------- сканирование LINE B BEGIN ---------------------*/
    if (!atomic_flag_test_and_set(&line_B_blocked))
	{

	atomic_flag_test_and_set(&line_A_blocked);
	atomic_flag_test_and_set(&line_C_blocked);
	atomic_flag_test_and_set(&line_D_blocked);
// LINE B в low, остальные строки отключаем
	// установка порта LINE на режим выхода [01]
	LINE_B_PORT->MODER &= ~(0b11 << (LINE_B_PIN * 2));     // сбрасываем бит
	LINE_B_PORT->MODER |= (0b01 << (LINE_B_PIN * 2));   // устанавливаем бит
	// установка LINE B в ноль
	LINE_B_PORT->BSRR |= (1 << (LINE_B_PIN + 16));

	// atomic_flag_test_and_set(&line_C_blocked);
	// atomic_flag_test_and_set(&line_D_blocked);

	// считываем столбцы и "ловим" LOW. LOW - значит именно эта кнопка нажата
	if ((ROW_1_PORT->IDR & (1 << ROW_1_PIN)) == 0)
	    key = keymap[1][0];
	if ((ROW_2_PORT->IDR & (1 << ROW_2_PIN)) == 0)
	    key = keymap[1][1];
	if ((ROW_3_PORT->IDR & (1 << ROW_3_PIN)) == 0)
	    key = keymap[1][2];
	if ((ROW_4_PORT->IDR & (1 << ROW_4_PIN)) == 0)
	    key = keymap[1][3];

	// возвращаем порт LINE B в режим входа (reset state) [00]
	LINE_B_PORT->MODER &= ~(0b11 << (LINE_B_PIN * 2));     // сбрасываем бит
	atomic_flag_clear(&line_A_blocked);
	atomic_flag_clear(&line_C_blocked);
	atomic_flag_clear(&line_D_blocked);

	}
    /*----------- сканирование LINE B END ---------------------*/

    /*----------- сканирование LINE C BEGIN ---------------------*/
    if (!atomic_flag_test_and_set(&line_C_blocked))
	{
	// LINE C в low, остальные строки отключаем
	// установка порта LINE C на режим выхода [01]
	atomic_flag_test_and_set(&line_A_blocked);
	atomic_flag_test_and_set(&line_B_blocked);
	atomic_flag_test_and_set(&line_D_blocked);
	LINE_C_PORT->MODER &= ~(0b11 << (LINE_C_PIN * 2));     // сбрасываем бит
	LINE_C_PORT->MODER |= (0b01 << (LINE_C_PIN * 2));   // устанавливаем бит
	// установка LINE C в ноль
	LINE_C_PORT->BSRR |= (1 << (LINE_C_PIN + 16));

	// считываем столбцы и "ловим" LOW. LOW - значит именно эта кнопка нажата
	if ((ROW_1_PORT->IDR & (1 << ROW_1_PIN)) == 0)
	    key = keymap[2][0];
	if ((ROW_2_PORT->IDR & (1 << ROW_2_PIN)) == 0)
	    key = keymap[2][1];
	if ((ROW_3_PORT->IDR & (1 << ROW_3_PIN)) == 0)
	    key = keymap[2][2];
	if ((ROW_4_PORT->IDR & (1 << ROW_4_PIN)) == 0)
	    key = keymap[2][3];

	// возвращаем порт LINE C в режим входа (reset state) [00]
	LINE_C_PORT->MODER &= ~(0b11 << (LINE_C_PIN * 2));     // сбрасываем бит
	atomic_flag_clear(&line_A_blocked);
	atomic_flag_clear(&line_B_blocked);
	atomic_flag_clear(&line_D_blocked);
	}
    /*----------- сканирование LINE C END ---------------------*/

    /*----------- сканирование LINE D BEGIN ---------------------*/
    if (!atomic_flag_test_and_set(&line_D_blocked))
	{
	// LINE D в low, остальные строки отключаем
	atomic_flag_test_and_set(&line_A_blocked);
	atomic_flag_test_and_set(&line_B_blocked);
	atomic_flag_test_and_set(&line_C_blocked);
	// установка порта LINE на режим выхода [01]
	LINE_D_PORT->MODER &= ~(0b11 << (LINE_D_PIN * 2));     // сбрасываем бит
	LINE_D_PORT->MODER |= (0b01 << (LINE_D_PIN * 2));   // устанавливаем бит
	// установка LINE D в ноль
	LINE_D_PORT->BSRR |= (1 << (LINE_D_PIN + 16));

	// считываем столбцы и "ловим" LOW. LOW - значит именно эта кнопка нажата
	if ((ROW_1_PORT->IDR & (1 << ROW_1_PIN)) == 0)
	    key = keymap[3][0];
	if ((ROW_2_PORT->IDR & (1 << ROW_2_PIN)) == 0)
	    key = keymap[3][1];
	if ((ROW_3_PORT->IDR & (1 << ROW_3_PIN)) == 0)
	    key = keymap[3][2];
	if ((ROW_4_PORT->IDR & (1 << ROW_4_PIN)) == 0)
	    key = keymap[3][3];

	// возвращаем порт LINE D в режим входа (reset state) [00]
	LINE_D_PORT->MODER &= ~(0b11 << (LINE_D_PIN * 2));     // сбрасываем бит
	atomic_flag_clear(&line_A_blocked);
	atomic_flag_clear(&line_C_blocked);
	atomic_flag_clear(&line_B_blocked);
	}
    /*----------- сканирование LINE D END ---------------------*/
    // предотвращение залипания/долгого нажатия
    if (key == oldKey)
	{
	return 0;
	}
    oldKey = key;

    return key;

    }
