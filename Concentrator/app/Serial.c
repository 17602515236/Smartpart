#include "Serial.h"
#include "string.h" 

void new_Serial(Serial_Handle *obj,UART_HandleTypeDef *huart)
{
    memset(obj,0,sizeof(Serial_Handle));
    obj->huart = huart;
    obj->DMA_Receive_Flags = Serial_No_Data;
    obj->DMA_Running_Flags = DMA_Stopping;
}

void Serial_Start_DMA_Receive(Serial_Handle *obj)
{
    memset(obj->Receive_Buff,0,Buff_Size);
    obj->DMA_Receive_Flags = Serial_No_Data;
    if(obj->DMA_Running_Flags == DMA_Stopping)
    {
        obj->DMA_Running_Flags = DMA_Running;
        __HAL_UART_CLEAR_FLAG(obj->huart, UART_FLAG_IDLE);
        __HAL_UART_ENABLE_IT(obj->huart, UART_IT_IDLE);
        HAL_UART_Receive_DMA(obj->huart, obj->Receive_Buff, Buff_Size);
    }
}
void Serial_Stop_DMA_Receive(Serial_Handle *obj)
{
    obj->DMA_Receive_Flags = Serial_No_Data;
    if(obj->DMA_Running_Flags == DMA_Running)
    {
        obj->DMA_Running_Flags = DMA_Stopping;
        HAL_UART_DMAStop(obj->huart);
    }
}

int Serial_Get_Running_Flags(Serial_Handle *obj)
{
    return obj->DMA_Running_Flags;
}

int Serial_Get_Receive_Flags(Serial_Handle *obj)
{
    return obj->DMA_Receive_Flags;
}

uint8_t *Serial_Get_Receive_Buff(Serial_Handle *obj)
{
    return obj->Receive_Buff;
}

uint16_t Serial_Get_Receive_Len(Serial_Handle *obj)
{
    return obj->Receive_Len;
}

void Serial_Interrupt_Process(Serial_Handle *obj)
{
    __HAL_UART_CLEAR_FLAG(obj->huart,UART_FLAG_RXNE);
    if(__HAL_UART_GET_FLAG(obj->huart,UART_FLAG_IDLE))
    {
        __HAL_UART_CLEAR_FLAG(obj->huart, UART_FLAG_IDLE);
        obj->Receive_Len = obj->huart->Instance->SR;
        obj->Receive_Len = obj->huart->Instance->DR;
        obj->Receive_Len = Buff_Size - obj->huart->hdmarx->Instance->CNDTR;
        obj->DMA_Receive_Flags = Serial_Data_Ready;
    }
}