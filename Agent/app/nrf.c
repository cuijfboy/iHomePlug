#include <stdint.h>
#include <stdbool.h>
#include "hal_nrf.h"
#include "hal_delay.h"
#include "nrf.h"

#define NRF_CHANNEL 				2
#define NRF_ADDR_WIDTH			HAL_NRF_AW_5BYTES
#define NRF_ADDR_PIPE0 			{3, 6, 9, 12, 0}          
#define NRF_ADDR_PIPE1 			1      
#define NRF_ADDR_PIPE2 			2                      
#define NRF_ADDR_PIPE3 			3                       
#define NRF_ADDR_PIPE4 			4                        
#define NRF_ADDR_PIPE5 			5

static uint16_t bdata nrf_bit_storage;
#define NRF_BIT(_var, _bitnum) sbit _var = nrf_bit_storage ^ _bitnum

NRF_BIT(nrf_tx_setup_modified, 0);  
NRF_BIT(nrf_rx_setup_modified, 1);
NRF_BIT(nrf_rx_dr, 2);
NRF_BIT(nrf_tx_empty, 3);
NRF_BIT(nrf_rfck_en, 4);
NRF_BIT(nrf_rfce, 5);

#define NRF_INTERRUPTS_DISABLE() do{ 	\
  RF = 0;                           	\
}while(0)

#define NRF_INTERRUPTS_ENABLE() do{  	\
  RF = 1;                           	\
}while(0)

#define NRF_RFCK_ENABLE() do{ 				\
  nrf_rfck_en = 1;      							\
  RFCKEN = 1;                  				\
} while(0)

#define NRF_RFCK_DISABLE() do{ 				\
  nrf_rfck_en = 0;       							\
  RFCKEN = nrf_rfce; 									\
} while(0)

#define NRF_RFCE_PULSE() do{ 					\
  RFCKEN = 1;                					\
  RFCE = 1;                  					\
  nrf_rfce = true; 										\
  delay_us(10); 											\
  RFCE = 0; 													\
} while(0)

#define NRF_RFCE_HIGH() do{ 					\
  RFCKEN = 1;                					\
  RFCE = 1;                  					\
  nrf_rfce = true; 										\
} while(0)

#define NRF_RFCE_LOW() do{ 						\
  RFCE = 0;                       		\
  RFCKEN = nrf_rfck_en;    						\
  nrf_rfce = false; 									\
} while(0)

static xdata nrf_states_t volatile nrf_state_var;  
static xdata uint8_t volatile nrf_current_tx_pipe;

static void nrf_interupts_disable_rfck_enable(void);
static void nrf_interupts_enable_rfck_disable(void);

static void nrf_interupts_disable_rfck_enable()
{
  uint8_t t_ea;
  t_ea = EA;
  EA = 0;
  NRF_INTERRUPTS_DISABLE();
  NRF_RFCK_ENABLE();
  EA = t_ea;
}

static void nrf_interupts_enable_rfck_disable()
{
  NRF_INTERRUPTS_ENABLE();
  NRF_RFCK_DISABLE();
}

nrf_states_t nrf_get_state(void)
{
	return nrf_state_var;
}

void nrf_goto_idle(void)
{
	if(nrf_state_var == NRF_PTX_ACTIVE)
	{
		while(nrf_state_var != NRF_IDLE)
		{}
	}
  else if(nrf_state_var == NRF_PRX_ACTIVE)
	{
		nrf_interupts_disable_rfck_enable();
		NRF_RFCE_LOW();
		nrf_state_var = NRF_IDLE;
		nrf_interupts_enable_rfck_disable();
	}
}

void nrf_init()
{
	uint8_t addr[NRF_ADDR_WIDTH] = NRF_ADDR_PIPE0;
	
	nrf_interupts_disable_rfck_enable();
	
	nrf_current_tx_pipe = 0;
	nrf_tx_setup_modified = true;
  nrf_rx_setup_modified = true;
	nrf_rx_dr = false;
	nrf_state_var = NRF_IDLE;

  NRF_RFCE_LOW();
	
  hal_nrf_enable_ack_payload(false);
  hal_nrf_enable_dynamic_payload(true);
  hal_nrf_setup_dynamic_payload(0xFF);
	hal_nrf_enable_dynamic_ack(false);

  hal_nrf_set_address(HAL_NRF_PIPE0, addr);
	addr[4] = NRF_ADDR_PIPE1;
  hal_nrf_set_address(HAL_NRF_PIPE1, addr);
  addr[0] = NRF_ADDR_PIPE2;
  hal_nrf_set_address(HAL_NRF_PIPE2, addr);
  addr[0] = NRF_ADDR_PIPE3;
  hal_nrf_set_address(HAL_NRF_PIPE3, addr);
  addr[0] = NRF_ADDR_PIPE4;
  hal_nrf_set_address(HAL_NRF_PIPE4, addr);
  addr[0] = NRF_ADDR_PIPE5;
  hal_nrf_set_address(HAL_NRF_PIPE5, addr);

  hal_nrf_set_rf_channel(NRF_CHANNEL);
  hal_nrf_set_output_power(HAL_NRF_0DBM);
  hal_nrf_set_datarate(HAL_NRF_2MBPS);
  hal_nrf_set_crc_mode(HAL_NRF_CRC_8BIT);
  hal_nrf_set_address_width(NRF_ADDR_WIDTH);

	hal_nrf_set_irq_mode(HAL_NRF_MAX_RT, false);
	hal_nrf_set_irq_mode(HAL_NRF_TX_DS, false);
	hal_nrf_set_irq_mode(HAL_NRF_RX_DR, true);
  hal_nrf_get_clear_irq_flags();

  hal_nrf_flush_rx();
  hal_nrf_flush_tx();
	
  hal_nrf_set_power_mode(HAL_NRF_PWR_UP);
  nrf_interupts_enable_rfck_disable();
}

bool nrf_tx_data(const uint8_t *src, uint8_t length, uint8_t pipe)
{
	uint8_t addr[NRF_ADDR_WIDTH] = NRF_ADDR_PIPE0;
	
	if(nrf_state_var == NRF_PRX_ACTIVE)
		nrf_goto_idle();
	nrf_state_var = NRF_PTX_ACTIVE;
	
	nrf_interupts_disable_rfck_enable();
	NRF_RFCE_LOW();
	
	if(pipe != nrf_current_tx_pipe)
	{
		nrf_current_tx_pipe = pipe;
		nrf_tx_setup_modified = true;
	}
	
	if(nrf_tx_setup_modified)
	{
		nrf_tx_setup_modified = false;
		nrf_rx_setup_modified = true;
		
		switch(pipe) 
		{
			case HAL_NRF_PIPE0:
				break;
			case HAL_NRF_PIPE1:
				addr[4] = pipe;
				break;
			case HAL_NRF_PIPE2:
			case HAL_NRF_PIPE3:
			case HAL_NRF_PIPE4:
			case HAL_NRF_PIPE5:
				addr[0] = pipe;
				break;
			default:
				nrf_state_var = NRF_IDLE;
				nrf_interupts_enable_rfck_disable();
				return false;
		}
		
		hal_nrf_set_address(HAL_NRF_TX, addr);
		hal_nrf_close_pipe(HAL_NRF_ALL);
		hal_nrf_open_pipe(HAL_NRF_TX, false);
	}

  hal_nrf_set_operation_mode(HAL_NRF_PTX);
	hal_nrf_write_tx_payload_noack(src, length);

	do 
	{
		NRF_RFCE_PULSE();
		delay_us(333);
		nrf_tx_empty = hal_nrf_tx_fifo_empty();
	}while(!nrf_tx_empty);

	nrf_state_var = NRF_IDLE;
	nrf_interupts_enable_rfck_disable();
	return true;
}

void nrf_rx_start(void)
{
	nrf_goto_idle();
	
	if(nrf_rx_setup_modified)
	{
		nrf_interupts_disable_rfck_enable();
		
		nrf_rx_setup_modified = false;
		nrf_tx_setup_modified = true;
		
		hal_nrf_close_pipe(HAL_NRF_ALL);
		hal_nrf_open_pipe(HAL_NRF_PIPE0, false);
		hal_nrf_set_operation_mode(HAL_NRF_PRX);
	}
	
	nrf_state_var = NRF_PRX_ACTIVE;
	NRF_RFCE_HIGH();
	nrf_interupts_enable_rfck_disable();
}

bool nrf_rx_data_ready(void)
{
	if(hal_nrf_rx_fifo_empty())
    nrf_rx_dr = false;
	else
		nrf_rx_dr = true;
	
	return nrf_rx_dr;
}

bool nrf_rx_fifo_read(uint8_t *dst, uint8_t *length, uint8_t *pipe)
{
	uint16_t pipe_and_length, temp_length;
	
	nrf_interupts_disable_rfck_enable();
	if(nrf_rx_dr)
	{
		temp_length = hal_nrf_read_rx_payload_width();
		if(temp_length <= 32) //TODO: Remove or comment hardcoded value
		{
			pipe_and_length = hal_nrf_read_rx_payload(dst);
			*pipe = (pipe_and_length >> 8);
			*length = temp_length;
			
			if(hal_nrf_rx_fifo_empty())
        nrf_rx_dr = false;
			
			nrf_interupts_enable_rfck_disable();
			return true;
		}
	}
	else
	{
		hal_nrf_flush_rx();
		nrf_rx_dr = false;
	}
	
	nrf_interupts_enable_rfck_disable();
	return false;
}

NRF_ISR()
{
  uint8_t irq_flags;
	
	NRF_INTERRUPTS_DISABLE();
  NRF_RFCK_ENABLE();
	
  irq_flags = hal_nrf_get_clear_irq_flags();

  if(irq_flags & (1<<(uint8_t)HAL_NRF_RX_DR))
		nrf_rx_dr = true;
	
	if((irq_flags & (1<<(uint8_t)HAL_NRF_TX_DS)))
    nrf_state_var = NRF_IDLE;
	
	if((irq_flags & (1<<(uint8_t)HAL_NRF_MAX_RT)))
  {
		hal_nrf_flush_tx();
    nrf_state_var = NRF_IDLE;
  }
	
	NRF_RFCK_DISABLE();
  NRF_INTERRUPTS_ENABLE();
}