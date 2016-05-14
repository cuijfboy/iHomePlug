#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "nrf24le1.h"
#include "agent.h"
#include "fifo.h"
#include "uart.h"
#include "hal_uart.h"
#include "hal_flash.h"

#define PMW 								0x10

#define NV_PAGE_ADDR				32
#define NV_ADDR_ADDR				0xFA00

#define CMD_ACK_FLAG        0x80
#define CMD_NOP             0x00
#define CMD_PORT_GET	    	0x01
#define CMD_PORT_SET	    	0x02
#define CMD_DIR_GET         0x03
#define CMD_DIR_SET         0x04
#define CMD_ADDR_GET				0x05
#define CMD_ADDR_SET			  0x06
/*
#define CMD_NV_GET					0x40
#define	CMD_NV_SET					0x41
#define	CMD_NV_CLR					0x42
*/

#define RET_NULL						0x00
#define FWD_SRC							0x01
#define FWD_DST							0x02
#define RET_SRC							0x04
#define RET_DST							0x08

#define UART_MSG						0x01
#define NRF_MSG							0x02

static struct uart_msg_fifo 	*uart_msg_rx_fifo, *uart_msg_tx_fifo; 
static struct nrf_msg_fifo 	*nrf_msg_rx_fifo, *nrf_msg_tx_fifo;

static uint8_t my_addr = MY_ADDR;
static uint8_t msg_seq;

void agent_task(uint8_t msg_type, uint8_t *msg, uint8_t len);
uint8_t agent_atcion( uint8_t src, uint8_t dst, uint8_t pip, uint8_t seq, 
											uint8_t *cmd, uint8_t cmd_len, 
											uint8_t *res_cmd, uint8_t* res_cmd_len);
uint8_t get_msg_seq(void);

void agent_init(struct uart_msg_fifo *uart_msg_rx_fifo_tmp,
								struct uart_msg_fifo *uart_msg_tx_fifo_tmp,
								struct nrf_msg_fifo *nrf_msg_rx_fifo_tmp,
								struct nrf_msg_fifo *nrf_msg_tx_fifo_tmp)
{
	uart_msg_rx_fifo = uart_msg_rx_fifo_tmp;
	uart_msg_tx_fifo = uart_msg_tx_fifo_tmp;
	nrf_msg_rx_fifo = nrf_msg_rx_fifo_tmp;
	nrf_msg_tx_fifo = nrf_msg_tx_fifo_tmp;
	
	PCON &= ~PMW;
	my_addr = hal_flash_byte_read(NV_ADDR_ADDR);
}

void agent_process(void)
{
	uint8_t msg[MSG_SIZE], len, /*new_msg[MSG_SIZE], new_len,*/ pip;
	
	while(!uart_msg_fifo_empty(uart_msg_rx_fifo) && !nrf_msg_fifo_full(nrf_msg_tx_fifo))
	{
		uart_msg_fifo_poll(uart_msg_rx_fifo, msg, &len);
		agent_task(UART_MSG, msg, len);
		nrf_msg_fifo_offer(nrf_msg_tx_fifo, msg, len, 0);
	}
	
	while(!nrf_msg_fifo_empty(nrf_msg_rx_fifo) && !uart_msg_fifo_full(uart_msg_tx_fifo))
	{
		nrf_msg_fifo_poll(nrf_msg_rx_fifo, msg, &len, &pip);
		//uart_new_msg(msg, len, new_msg, &new_len);
		//uart_msg_fifo_offer(uart_msg_tx_fifo, new_msg, new_len);
		agent_task(NRF_MSG, msg, len);
	}
}					

void agent_task(uint8_t msg_type, uint8_t *msg, uint8_t len)
{
	uint8_t src, dst, pip, seq, *cmd, cmd_len;
	uint8_t res[MSG_SIZE], res_len, *res_cmd, res_cmd_len, res_code;

	src = msg[0];
	dst = msg[1];
	pip = msg[2];
	seq = msg[3];
	cmd = msg + 4;
	cmd_len = len - 4;
	
	res_cmd = res + 4;
	
	res_code = agent_atcion(src, dst, pip, seq, 
													cmd, cmd_len, res_cmd, &res_cmd_len);
	
	if(res_code == RET_NULL)
		return;
	
	if(res_code & FWD_DST && msg_type == UART_MSG)
	{
		if(src == UART_ADDR)
			msg[0] = my_addr;
		if(seq == UART_SEQ)
			msg[3] = get_msg_seq();
	
		nrf_msg_fifo_offer(nrf_msg_tx_fifo, msg, len, pip);
	}
	
	if(res_code & RET_SRC && msg_type == NRF_MSG)
	{
		res[0] = my_addr;
		res[1] = src;
		res[2] = pip;
		res[3] = seq;
		res_len = res_cmd_len + 4;
		
		nrf_msg_fifo_offer(nrf_msg_tx_fifo, res, res_len, pip);
	}
	
	if(res_code & RET_SRC && msg_type == UART_MSG)
	{
		res[0] = my_addr;
		res[1] = src;
		res[2] = pip;
		res[3] = seq;
		res_len = res_cmd_len + 4;
		
		uart_msg_fifo_offer(uart_msg_tx_fifo, res, res_len);
	}
	
	if(res_code & FWD_SRC)
	{
		
	}
	
	if(res_code & RET_DST)
	{
		
	}
	
}

uint8_t agent_atcion( uint8_t src, uint8_t dst, uint8_t pip, uint8_t seq, 
											uint8_t *cmd, uint8_t cmd_len, 
											uint8_t *res_cmd, uint8_t* res_cmd_len)
{
	bool succ;
	res_cmd[0] = cmd[0] | CMD_ACK_FLAG;

	*res_cmd_len = cmd_len;
	succ = true;

	switch(cmd[0])
	{
		case CMD_ADDR_SET:
			my_addr = cmd[1];
			PCON &= ~PMW;
			hal_flash_page_erase(NV_PAGE_ADDR);
			hal_flash_byte_write(NV_ADDR_ADDR, my_addr);
			my_addr = hal_flash_byte_read(NV_ADDR_ADDR);
		case CMD_ADDR_GET:
			res_cmd[1] = my_addr;
			res_cmd[2] = dst;
			res_cmd[3] = (uint8_t)(NV_ADDR_ADDR >> 8);
			res_cmd[4] = (uint8_t)(NV_ADDR_ADDR & 0x00FF);
			break;
		default:
			succ = false;
			break;
	}
	
	if(succ)
		return RET_SRC;
	
	// --------------------------------------------------------------------------
	
	if(my_addr != dst)
		return FWD_DST;

	// --------------------------------------------------------------------------

	*res_cmd_len = cmd_len;
	succ = true;	
	
	switch(cmd[0])
	{
		case CMD_NOP:
			break;
		case CMD_PORT_SET:
			PORT_0 |= cmd[1] &  cmd[2] &  PORT_0_MASK;
			PORT_0 &= cmd[1] | ~cmd[2] | ~PORT_0_MASK;
			PORT_1 |= cmd[3] &  cmd[4] &  PORT_1_MASK;
			PORT_1 &= cmd[3] | ~cmd[4] | ~PORT_1_MASK;
		case CMD_PORT_GET:
			res_cmd[1] = PORT_0      & cmd[2];
			res_cmd[2] = PORT_0_MASK & cmd[2];
			res_cmd[3] = PORT_1      & cmd[4];
			res_cmd[4] = PORT_1_MASK & cmd[4];
			break;
		case CMD_DIR_SET:
			PORT_0_DIR |= cmd[1] &  cmd[2] &  PORT_0_MASK;
			PORT_0_DIR &= cmd[1] | ~cmd[2] | ~PORT_0_MASK; 
			PORT_1_DIR |= cmd[3] &  cmd[4] &  PORT_1_MASK;
			PORT_1_DIR &= cmd[3] | ~cmd[4] | ~PORT_1_MASK; 
		case CMD_DIR_GET:
			res_cmd[1] = PORT_0_DIR  & cmd[2];
			res_cmd[2] = PORT_0_MASK & cmd[2];
			res_cmd[3] = PORT_1_DIR  & cmd[4];
			res_cmd[4] = PORT_1_MASK & cmd[4];
			break;
		case CMD_ADDR_SET:
			my_addr = cmd[1];
			PCON &= ~PMW;
			hal_flash_page_erase(NV_PAGE_ADDR);
			hal_flash_byte_write(NV_ADDR_ADDR, my_addr);
			my_addr = hal_flash_byte_read(NV_ADDR_ADDR);
		case CMD_ADDR_GET:
			res_cmd[1] = my_addr;
			res_cmd[2] = (uint8_t)(NV_ADDR_ADDR >> 8);
			res_cmd[3] = (uint8_t)(NV_ADDR_ADDR & 0x00FF);
			break;
		default:
			succ = false;
			break;
	}
	
	if(succ)
		return RET_SRC;
	return RET_NULL;
}

uint8_t get_msg_seq(void)
{
	msg_seq++;
	if(msg_seq == UART_SEQ)
		msg_seq = 0;
	return msg_seq;
}
