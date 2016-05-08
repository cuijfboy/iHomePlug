#include <stdio.h>
#include <string.h>
#include "nrf2.h"
#include "nrf.h"
#include "fifo.h"
#include "hal_uart.h"
#include "uart.h"

void nrf_rx_msg(struct nrf_msg_fifo *fifo)
{
	uint8_t msg[MSG_SIZE], len, pip;
	
	if(nrf_get_state() == NRF_PTX_ACTIVE)
		return;
	
	while(!nrf_msg_fifo_full(fifo) && nrf_rx_data_ready())
		if(nrf_rx_fifo_read(msg, &len, &pip))
			nrf_msg_fifo_offer(fifo, msg, len, pip);
}

void nrf_tx_msg(struct nrf_msg_fifo *fifo)
{
	uint8_t msg[MSG_SIZE], len, pip;
	
	if(nrf_msg_fifo_empty(fifo))
		return;
	
	if(nrf_get_state() == NRF_PRX_ACTIVE)
		nrf_goto_idle();
	
	while(nrf_msg_fifo_poll(fifo, msg, &len, &pip))
		nrf_tx_data(msg, len, pip);
	
	nrf_rx_start();		
}