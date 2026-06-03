/*
 * uart_function.h - STM32 HAL Version
 *
 * Change Logs:
 * Date           Author           Notes
 * 2025-06-18     Bai Jiankang     Initial
 */

#ifndef APPLICATIONS_UART_H_
#define APPLICATIONS_UART_H_

#include "main.h"

#define CONCAT2(a, b)  a ## b
#define CONCAT(a, b)   CONCAT2(a, b)

// --- UART selection: change to any USART number ---
#define UART_SEL  1
// --------------------------------------------------

#define HUART             CONCAT(huart,    UART_SEL)
#define UART_PROCESS      CONCAT(uart,     CONCAT(UART_SEL, _process_char))
#define UART_CALLBACK     CONCAT(user_uart, CONCAT(UART_SEL, _callback))

typedef enum
{
    NUMBER = 0,
    STRING,
    NULL_TYPE
} recv_type_t;

extern recv_type_t g_type_of_recv;
extern unsigned char g_received_finished;
extern float g_received_float;
extern char g_received_string[40];

void uart_putstring(UART_HandleTypeDef *huart, const char *str);
void UART_PROCESS(char recv_char);

#endif /* APPLICATIONS_UART_H_ */
