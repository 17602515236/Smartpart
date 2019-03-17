#ifndef __RA02__H__
#define __RA02__H__

#include "platform.h"
#include "radio.h"
#include "sx1276-Hal.h"
#include "sx1276.h"
#include "sx1276-LoRaMisc.h"
#include "sx1276-LoRa.h"
#include "sx1276-Fsk.h"

typedef struct terminal_type
{
	uint8_t status;
}Treminal_Type;

enum statue
{
	TX_OK,
	TX_FAILED,
	RX_OK,
	TX_Timeout,
	RX_Timeout,
	Net_Back_Timeout,
};
extern tRadioDriver *mradio ;
int Lora_Init(void);
int Lora_Wait_Package(uint8_t *output,uint32_t Timeout);
int Lora_Wait_Package_nb(uint8_t *output);
int Transmit_Package(uint8_t *Package, uint32_t len,uint32_t timeout,uint32_t Retry);
#endif