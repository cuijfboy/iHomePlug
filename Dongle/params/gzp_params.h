/* Copyright (c) 2008 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT. 
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRENTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 * $LastChangedRevision: 133 $
 */ 

#ifndef __GZP_PARAMS_H
#define __GZP_PARAMS_H

//-----------------------------------------------------------------------------
// IMPRTANT: Secret key should be modified and KEPT SECRET
//-----------------------------------------------------------------------------

/** 
  Definition of "secret key" used during "Host ID exchange".
*/  

#define GZP_SECRET_KEY {1, 23, 45, 57, 26, 68, 12, 64, 13, 73, 13, 62, 26, 45, 12, 77}

//-----------------------------------------------------------------------------

/** 
  Definition of the first static selected pairing channel. Should be located in
  the lower Nth of the channel range, where N is the size if the channel subset
  selected by the application.
*/  
#define GZP_CHANNEL_LOW 2

/** 
  Definition of the second static selected pairing channel. Should be located in
  the upper Nth of the channel range, where N is the size if the channel subset
  selected by the application.
*/  
#define GZP_CHANNEL_HIGH 79

/** 
  Definition of the static "global" pairing address.
*/ 
#define GZP_ADDRESS 4, 6, 8, 10

/** 
  Definition of output power to be used when sendning a pairing request. 
*/ 
#define GZP_POWER HAL_NRF_18DBM

/** 
  Definition of pairing request timeout. 
*/ 
#define GZP_REQ_TX_TIMEOUT 200

/** 
  Definition of the maximimum number of "backoff" packets (step 0) to be transmitted. 
*/  
#define GZP_MAX_BACKOFF_PACKETS 100

/**
  Definition of the period a device shall wait before attempting to send the packet for
  fetching the pairing response (step 1).
*/
#define GZP_TX_ACK_WAIT_TIMEOUT (GZP_CLOSE_PROXIMITY_BACKOFF_RX_TIMEOUT + 50)

/** 
  Definition of the period a device in close proximity shall back off on the pairing 
  address after a backoff packet has been received.  
*/  
#define GZP_CLOSE_PROXIMITY_BACKOFF_RX_TIMEOUT ((GZP_REQ_TX_TIMEOUT / 2) + 50)

/** 
  Definition of the period a device NOT in close proximity shall back off on the pairing 
  address after a backoff packet has been received.  
*/   
#define GZP_NOT_PROXIMITY_BACKOFF_RX_TIMEOUT (GZP_CLOSE_PROXIMITY_BACKOFF_RX_TIMEOUT + GZP_STEP1_RX_TIMEOUT)

/**
  Definition of the time the host waits for a device to complete
  transmission of the pairing request step 1 packet.  
*/
#define GZP_STEP1_RX_TIMEOUT (((GZP_REQ_TX_TIMEOUT / 2) + GZP_TX_ACK_WAIT_TIMEOUT) + 50)

/** 
  Definition of the lowest boundary for the channel range to be used.
*/
#define GZP_CHANNEL_MIN 2

/** 
  Definition of the upper boundary for the channel range to be used.
*/  
#define GZP_CHANNEL_MAX 80

/** 
  Definition of the minimum channel spacing for the channel subset generated
  during pairing.
*/  
#define GZP_CHANNEL_SPACING_MIN 5

/** 
  Non volatile memory (Flash or OTP) storage address. A device will
  require GZP_DEVICE_PARAMS_STORAGE_SIZE bytes of memory, and 
  Host 11 bytes.
*/ 
#ifdef EX_PROJ_LE1
#define GZP_PARAMS_STORAGE_ADR 0xFC00
#elif(defined(EX_PROJ_LU1))
#define GZP_PARAMS_STORAGE_ADR (31 * 512)
#else
#error Please define either EX_PROJ_LE1 or EX_PROJ_LU1.
#endif

/** 
  Number of bytes available for parameter storage in Device.
*/ 
#define GZP_DEVICE_PARAMS_STORAGE_SIZE 1024

#endif // GZP_PARAMS_H