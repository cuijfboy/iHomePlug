#include <stdbool.h>
#include <string.h>
#include "nrf24lu1p.h"

#include "hal_usb.h"
#include "hal_usb_hid.h"

#include "gzll_mcu.h"
#include "gzll.h"

#include "hal_delay.h"

#define USB_MSG_LEN 8

bool is_usb_data_ready = false;
bool is_usb_writing = false;
uint8_t usb_buf_tx[USB_MSG_LEN], usb_buf_rx[USB_MSG_LEN];
uint8_t usb_buf_rx_len = 0;

// USB callback function prototypes
void suspend_cb(void) reentrant
{
	USBSLP = 1; 	// Disable USB clock (auto clear)  
	WUCONF = 0x22;
}

void resume_cb(void) reentrant
{
	is_usb_writing = false;
}

void reset_cb(void) reentrant
{
	is_usb_writing = false;
}

uint8_t ep_1_in_cb(uint8_t *adr_ptr, uint8_t* size) reentrant
{
	is_usb_writing = false;
    //P03 = 1;
	return 0x60; // NAK
}

uint8_t ep_1_out_cb(uint8_t *adr_ptr, uint8_t* size) reentrant
{
    /*
    uint8_t len = *size;
	if(!is_usb_data_ready)
    {
        is_usb_data_ready = true;
        if(len < USB_MSG_LEN)
        {
            memset(usb_buf_rx+len, 0, USB_MSG_LEN-len);
        }
        memcpy(usb_buf_rx, adr_ptr, len < USB_MSG_LEN ? len : USB_MSG_LEN);
        P03 = 1;
    }
    */
	return 0x40; // NAK
}

hal_usb_dev_req_resp_t device_req_cb(hal_usb_device_req* req, 
                                     uint8_t** data_ptr, 
                                     uint8_t* size) reentrant
{
	hal_usb_dev_req_resp_t retval;
	P03 = 1;
    if(hal_usb_hid_device_req_proc(req, data_ptr, size, &retval) == true){
		// The request was processed with the result stored in the retval variable
		return retval;
	}else{
		return STALL;
	}
	return STALL;
}

void usb_send_data(uint8_t ep_in, uint8_t* dat, uint8_t len)
{
	uint8_t res = 0;
    is_usb_writing = true; 
    if(len < USB_MSG_LEN)
    {
        memset(usb_buf_tx+len, 0, USB_MSG_LEN-len);
    }
    memcpy(usb_buf_tx, dat, len < USB_MSG_LEN ? len : USB_MSG_LEN);
	hal_usb_send_data(ep_in, usb_buf_tx, USB_MSG_LEN);
}

void usb_send_data_sync(uint8_t ep_in, uint8_t* dat, uint8_t len)
{
	while(is_usb_writing)
	{}
    usb_send_data(ep_in, dat, len);
}

void usb_send_data_async(uint8_t ep_in, uint8_t* dat, uint8_t len)
{
	if(is_usb_writing)
		return;
    usb_send_data(ep_in, dat, len);
}

void usb_init(void)
{
	hal_usb_init(true, device_req_cb, reset_cb, resume_cb, suspend_cb);
	hal_usb_endpoint_config(0x81, USB_MSG_LEN, ep_1_in_cb); 		
    //hal_usb_endpoint_config(0x91, USB_MSG_LEN, ep_1_out_cb); 	
	is_usb_writing = false;
    is_usb_data_ready = false;
}

// ----------------------------------------------------------------------------

void main(void)
{
    uint8_t payload[GZLL_MAX_PAYLOAD_LENGTH];
    
    mcu_init();

    gzll_init();
    
    usb_init();

    P0DIR &= ~(1 << 3);                                   

    EA = 1;
    P03 = 1;
    
    delay_ms(500);
    usb_send_data_sync(0x81, "SYS_UP!", 7);
    P03 = 0;

    // Enter host mode (start monitoring for data)
    gzll_rx_start();                                          

    for(;;)
    {      
        /*
        if(is_usb_data_ready && !is_usb_writing)
        {
            //P03 = 1;
            is_usb_data_ready = false;
            // usb_send_data_sync(0x81, usb_buf, usb_buf_len);
        }
        
        // If data received
        if(gzll_rx_fifo_read(payload, NULL, NULL))
        {
            // Write received payload[0] to port 0
            // P0 = payload[0];
            // P03 = ~P03;
        }
        */
    } 
}

