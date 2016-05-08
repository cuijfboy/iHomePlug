#ifndef AGENT_H__
#define AGENT_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define UART_DIR_MASK       0xE7
#define PORT_0              P0
#define PORT_0_DIR          P0DIR
#define PORT_0_MASK         0xE7
#define PORT_1              P1
#define PORT_1_DIR          P1DIR
#define PORT_1_MASK         0x7F

void agent_process(	struct uart_msg_fifo *uart_msg_rx_fifo,
										struct uart_msg_fifo *uart_msg_tx_fifo,
										struct nrf_msg_fifo *nrf_msg_rx_fifo,
										struct nrf_msg_fifo *nrf_msg_tx_fifo);

#endif // AGENT_H__