#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "usart.h"

enum{
    Serial_Data_Ready,
    Serial_No_Data,
    DMA_Running,
    DMA_Stopping,
    Data_Received,
    NoData_Received,
};

#define Buff_Size 128
typedef struct{
    UART_HandleTypeDef *huart;
    uint8_t DMA_Receive_Flags;
    uint8_t DMA_Running_Flags;
    uint8_t Receive_Buff[Buff_Size];
    uint16_t Receive_Len;
}Serial_Handle;

void new_Serial(Serial_Handle *obj,UART_HandleTypeDef *huart);
void Serial_Start_DMA_Receive(Serial_Handle *obj);
void Serial_Interrupt_Process(Serial_Handle *obj);
void Serial_Stop_DMA_Receive(Serial_Handle *obj);
int Serial_Get_Running_Flags(Serial_Handle *obj);
int Serial_Get_Receive_Flags(Serial_Handle *obj);
uint8_t *Serial_Get_Receive_Buff(Serial_Handle *obj);
uint16_t Serial_Get_Receive_Len(Serial_Handle *obj);
#endif //Serial.h