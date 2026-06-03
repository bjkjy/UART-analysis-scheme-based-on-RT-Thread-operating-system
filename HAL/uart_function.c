/*
 * uart_function.c - STM32 HAL Version
 *
 * Change Logs:
 * Date           Author           Notes
 * 2025-06-18     Bai Jiankang     Initial
 */

#include "usart.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include "uart_function.h"

static char uart_recv_buffer[40] = {0};
static unsigned char uart_index = 0;

recv_type_t g_type_of_recv = NULL_TYPE;
unsigned char g_received_finished = 0;
float g_received_float = 0.0f;
char g_received_string[40] = {0};

const char *returns_error_style = "Format error!\r\n";
const char *returns_null = "None entered!\r\n";

void uart_putstring(UART_HandleTypeDef *huart, const char *str)
{
    HAL_UART_Transmit(huart, (unsigned char*)str, strlen(str), HAL_MAX_DELAY);
}

void UART_CALLBACK(void)
{
    if(g_type_of_recv == STRING)
    {
        uart_putstring(&HUART, "STRING: ");
        uart_putstring(&HUART, g_received_string);
        uart_putstring(&HUART, "\n");
        if(strcmp((char*)g_received_string, "Run200") == 0)
        {
            uart_putstring(&HUART, "RUN SPEED 200...\n");
        }
        else if(strcmp((char*)g_received_string, "Stop") == 0)
        {
            uart_putstring(&HUART, "STOP CAR...\n");
        }
        else
        {
            uart_putstring(&HUART, "Command error!\n");
        }
    }
    else if (g_type_of_recv == NUMBER)
    {
        char num_buf[40];
        snprintf(num_buf, sizeof(num_buf), "Num: %f\n", g_received_float);
        uart_putstring(&HUART, num_buf);
    }
}

void UART_PROCESS(char recv_char)
{
    static recv_type_t type_of_recv = NULL_TYPE;
    static signed int recv_data_B = 0;
    static signed int recv_data_F = 0;
    static unsigned char point_position = 0;
    static unsigned char decimal_places = 0;
    static unsigned char is_negative = 0;

    if (recv_char != '\r' && recv_char != '\n')
    {
        g_received_finished = 0;

        if (uart_index < sizeof(uart_recv_buffer) - 1)
        {
            uart_recv_buffer[uart_index++] = recv_char;
        }

        if (type_of_recv == STRING)
        {
            // pass
        }
        else if (type_of_recv == NUMBER)
        {
            if (isdigit((unsigned char)recv_char))
            {
                if (point_position == 0)
                {
                    if (recv_data_B > (INT32_MAX / 10)) { type_of_recv = STRING; }
                    else { recv_data_B = recv_data_B * 10 + (recv_char - '0'); }
                }
                else
                {
                    if (recv_data_F > (INT32_MAX / 10)) { type_of_recv = STRING; }
                    else { recv_data_F = recv_data_F * 10 + (recv_char - '0'); }
                    decimal_places++;
                }
            }
            else if (recv_char == '.' && point_position == 0)
            {
                point_position = 1;
            }
            else
            {
                type_of_recv = STRING;
            }
        }
        else
        {
            if (isdigit((unsigned char)recv_char))
            {
                type_of_recv = NUMBER;
                recv_data_B = recv_data_B * 10 + (recv_char - '0');
            }
            else if (recv_char == '-')
            {
                type_of_recv = NUMBER;
                is_negative = 1;
            }
            else
            {
                type_of_recv = STRING;
            }
        }
    }
    else
    {
        if (type_of_recv == NUMBER)
        {
            float final_value = recv_data_B + (float)recv_data_F / pow(10, decimal_places);
            if (is_negative)
            {
                final_value = -final_value;
            }
            g_received_float = final_value;
            g_type_of_recv = NUMBER;
            g_received_finished = 1;
        }
        else if (type_of_recv == STRING)
        {
            uart_recv_buffer[uart_index] = '\0';
            strncpy(g_received_string, uart_recv_buffer, sizeof(g_received_string) - 1);
            g_received_string[sizeof(g_received_string) - 1] = '\0';
            g_type_of_recv = STRING;
            g_received_finished = 1;
        }
        else
        {
            uart_putstring(&HUART, returns_null);
        }

        memset(uart_recv_buffer, 0, sizeof(uart_recv_buffer));
        uart_index = 0;
        recv_data_B = 0;
        recv_data_F = 0;
        point_position = 0;
        decimal_places = 0;
        is_negative = 0;
        type_of_recv = NULL_TYPE;
    }

    if(g_received_finished)
    {
        UART_CALLBACK();
        g_received_finished = 0;
        g_type_of_recv = NULL_TYPE;
    }
}
