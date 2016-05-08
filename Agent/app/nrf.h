#ifndef NRF_H__
#define NRF_H__

#include <stdint.h>
#include <stdbool.h>

/**
Protocol states.
*/
typedef enum
{
  NRF_IDLE,
  NRF_PTX_ACTIVE,
  NRF_PRX_ACTIVE
}nrf_states_t;

void nrf_init(void);
void nrf_rx_start(void);
bool nrf_tx_data(const uint8_t *src, uint8_t length, uint8_t pipe);
bool nrf_rx_data_ready(void);
bool nrf_rx_fifo_read(uint8_t *dst, uint8_t *length, uint8_t *pipe);
nrf_states_t nrf_get_state(void);
void nrf_goto_idle(void);

#endif // NRF_H__