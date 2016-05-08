#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "agent.h"
#include "fifo.h"
#include "uart.h"

#define CMD_ACK_FLAG        0x80
#define CMD_NOP             0x00
#define CMD_PORT_GET	    	0x01
#define CMD_PORT_SET	    	0x02
#define CMD_DIR_GET         0x03
#define CMD_DIR_SET         0x04

void agent_process(	struct uart_msg_fifo *uart_msg_rx_fifo,
										struct uart_msg_fifo *uart_msg_tx_fifo,
										struct nrf_msg_fifo *nrf_msg_rx_fifo,
										struct nrf_msg_fifo *nrf_msg_tx_fifo)
{
	uint8_t msg[MSG_SIZE], len, new_msg[MSG_SIZE], new_len, pip;
	
	while(!uart_msg_fifo_empty(uart_msg_rx_fifo) && !nrf_msg_fifo_full(nrf_msg_tx_fifo))
	{
		uart_msg_fifo_poll(uart_msg_rx_fifo, msg, &len);
		nrf_msg_fifo_offer(nrf_msg_tx_fifo, msg, len, 0);
	}
	
	while(!nrf_msg_fifo_empty(nrf_msg_rx_fifo) && !uart_msg_fifo_full(uart_msg_tx_fifo))
	{
		nrf_msg_fifo_poll(nrf_msg_rx_fifo, msg, &len, &pip);
		uart_new_msg(msg, len, new_msg, &new_len);
		uart_msg_fifo_offer(uart_msg_tx_fifo, new_msg, new_len);
	}
}											