#include "SmartPart.h"
#include "gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Ra02.h"
#include <stdio.h>
#include <string.h>
Network_Adapter *Adapter = NULL;

uint8_t Lora_Buff[64];
void Lora_Process_Task(void *Para)
{
    uint16_t count=0;
    while(1)
    {
        if(Adapter->Get_Network_Status() == CONNECT_SERVER)
        {
            if ((count = Lora_Wait_Package(Lora_Buff,0xFFFF)) > 0)
                Adapter->write(&Lora_Buff[1], count);
        }
    }
}

uint8_t Buff[128];
void Server_Lora_Task(void *Para)
{
    int bytes = 0;
    if (Adapter->Get_Network_Status() == CONNECT_SERVER)
    {
        if ((bytes = Adapter->read_nb(Buff, 128)) > 0)
            Transmit_Package(Buff,bytes,500,3);
    }
}
void SmartPart_Task(void *param)
{
    // const unsigned char *str="OK\n";
    Adapter = Network_Adapter_Init();
    Lora_Init();
    xTaskCreate(Lora_Process_Task, "Lora_Process_Task", 64, NULL, 8, NULL);
    while(1)
    {
        if(Adapter->Get_Network_Status() == NOINIT)
            Adapter->Init();
        if (Adapter->Get_Network_Status() == GOT_IP)
            Adapter->UDP_Connect("47.106.129.185", 9008, 0, 5000);
        // if (Adapter->Get_Network_Status() == CONNECT_SERVER)
        // {
        //     memset(Buff, 0, 128);
        //     HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        //     Adapter->write(str, strlen(str));
        //     rc = Adapter->read(Buff, 128, 10000);
        //     if(strcmp(Buff,"sss"))
        //     {
        //     }
        //     vTaskDelay(3000);
        // }
    }
}