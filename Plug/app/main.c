/* Copyright (c) 2009 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is confidential property of Nordic 
 * Semiconductor ASA.Terms and conditions of usage are described in detail 
 * in NORDIC SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT. 
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRENTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *              
 * $LastChangedRevision: 133 $
 */

/** @file
 * @brief Gazell Link Layer Host example
 * @defgroup gzll_host_example Gazell Link Layer Host example
 * @{
 * @ingroup nrf_examples
 *
 * @brief This example listens for data and writes the first byte (byte 0) of the
 * received payloads to P0. 
 *
 * Protocol parameters such as addresses and channels are specified 
 * in @ref gazell_examples/params/gzll_params.h. 
 *
 * The project @ref gzll_device_example can be used as a counterpart for transmitting the data.
 * 
*/

#include <stdio.h>
#include <string.h>
#include "nrf24le1.h"
#include "hal_uart.h"
#include "hal_clk.h"
#include "gzll_mcu.h"
#include "gzll.h"

#define PLUG_PORT		P1
#define PLUG_UP         0x40
#define PLUG_DW         0x20
#define PLUG_UP_ON	    PLUG_PORT |=  PLUG_UP
#define PLUG_UP_OFF	    PLUG_PORT &= ~PLUG_UP
#define PLUG_DW_ON	    PLUG_PORT |=  PLUG_DW
#define PLUG_DW_OFF	    PLUG_PORT &= ~PLUG_DW
#define IS_PLUG_UP_ON   !(PLUG_PORT & PLUG_UP)
#define IS_PLUG_DW_ON   !(PLUG_PORT & PLUG_DW)

#define CMD_NOP 0x00
#define CMD_GET	0x01
#define CMD_SET	0x02

uint8_t packet_cnt = 0;
uint8_t payload[GZLL_MAX_PAYLOAD_LENGTH];

void process()
{
    switch(payload[0])
    {
        case CMD_NOP:
            gzll_ack_payload_write(payload, 1, 0);
            break;
        case CMD_SET:
            if(payload[1])
                PLUG_UP_ON;
            else
                PLUG_UP_OFF;
            if(payload[2])
                PLUG_DW_ON;
            else
                PLUG_DW_OFF;
        case CMD_GET:
            payload[1] = IS_PLUG_UP_ON;
            payload[2] = IS_PLUG_DW_ON;
            memset(payload+3, 0, GZLL_MAX_PAYLOAD_LENGTH-3);
            gzll_ack_payload_write(payload, 3, 0);
            break;
        default:
            break;
    }
}

// Cusomization of low level stdio function. Used by for example printf().
#ifdef __ICC8051__
int putchar(int c)
#else /*presume C51 or other accepting compilator*/
char putchar(char c)
#endif
{
    hal_uart_putchar(c);
    return c;
}

// Cusomization of low level stdio function. Used by for example gets().
#ifdef __ICC8051__
int getchar(void)
#else /*presume C51 or other accepting compilator*/
char getchar(void)
#endif
{
    return hal_uart_getchar();
}

// Repeated putchar to print a string
void putstring(char *s)
{
    while(*s != 0)
        putchar(*s++);
}

void putdata(uint8_t *d, uint8_t s)
{
    while(s--)
        putchar(*d++);
}

void main(void)
{
    mcu_init();
    gzll_init();

    P0DIR = 0xE7;
    P1DIR = 0x9F;

    // Initializes the UART
    hal_uart_init(UART_BAUD_9K6);

    // Wait for XOSC to start to ensure proper UART baudrate
    while(hal_clk_get_16m_source() != HAL_CLK_XOSC16M)
    {}

    // Enable global interrupts
    EA = 1;

    // Print "Hello World" at start-up
    putstring("\r\nHello World! I'm iHomePlug ^_^\r\n");

    // Enter host mode (start monitoring for data)
    gzll_rx_start();                                          

    for(;;)
    {      
        // If data received
        if(gzll_rx_fifo_read(payload, NULL, NULL))
        {
            putstring("\r\nRx: ");
            putdata(payload, GZLL_MAX_PAYLOAD_LENGTH);
            process();
            putstring("\r\nTx: ");
            putdata(payload, GZLL_MAX_PAYLOAD_LENGTH);
        }
    } 
}

/** @} */
