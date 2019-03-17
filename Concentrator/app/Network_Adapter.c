#include "Network_Adapter.h"
#include "ESP8266.h"
#include "FreeRTOS.h"
#include "task.h"
static Network_Adapter __Adapter;

static Network_Status_Enum Network_Status;

static Network_Status_Enum __Adapter_Get_Network_Status()
{
    return Network_Status;
}

static void Status_Changed_Callback(uint8_t New_Satatus)
{
    if(New_Satatus == CHANGED_WIFI_DISCONNECT)
        Network_Status = NOINIT;
}
/**
 * @brief Init Network Driver
 * 
 * @return Adapter_Status 
 */
static Adapter_Status __Adapter_Init()
{
    ESP8266_Handle *Handle = (ESP8266_Handle *)__Adapter.Device_Handle;
    Handle->Status_Changed_Callback = Status_Changed_Callback;
    Handle->ReSet();
    vTaskDelay(1000);

    //AT
    if(Handle->Tran_WaitACK_NoParameter(AT, 100, NULL) == ESP8266_Failed)
        return Adapter_Failed;
    HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
    //ATE0
    if(Handle->Tran_WaitACK_NoParameter(ATE1,100,NULL) == ESP8266_Failed)
        return Adapter_Failed;
    HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
    //Set Mode Station
    if(Handle->Tran_WaitACK_NoParameter(CWMODE_CUR_1,100,NULL) == ESP8266_Failed)
        return Adapter_Failed;
    HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
    //Connect wifi
    if(Handle->Connect_wifi("vivo","123456789",15000) == ESP8266_Failed)
        return Adapter_Failed;
    Network_Status = GOT_IP;
    return Adapter_OK;
}

static Adapter_Status __Adapter_TCP_Connect(char *IP,
                                            uint32_t port,
                                            uint32_t timeout)
{
    ESP8266_Handle *Handle = (ESP8266_Handle *)__Adapter.Device_Handle;
    if (Handle->Connect_Server(TCP, IP, port,0, timeout) == ESP8266_Failed)
    {
        Network_Status = CONNECT_SERVER;
        return Adapter_OK;
    }
        return Adapter_Failed;
}

static Adapter_Status __Adapter_UDP_Connect(char *IP,
                                            uint32_t port,
                                            uint32_t local_port,
                                            uint32_t timeout)
{
    ESP8266_Handle *Handle = (ESP8266_Handle *)__Adapter.Device_Handle;
    if (Handle->Connect_Server(UDP, IP, port,local_port, timeout) == ESP8266_OK)
    {
        Network_Status = CONNECT_SERVER;
        return Adapter_OK;
    }
        return Adapter_Failed;
}

static int __Adapter_write(unsigned char *Data,int count)
{
    ESP8266_Handle *Handle = (ESP8266_Handle *)__Adapter.Device_Handle;
    return Handle->Transmit_Data(Data,count);
}


static int __read_nb(unsigned char *output,
                     uint32_t bytes)
{
    ESP8266_Handle *Handle = (ESP8266_Handle *)__Adapter.Device_Handle;
    return Handle->read_nb(output,bytes);
}
static int __read(unsigned char *output,
                  uint32_t bytes,
                  uint32_t timeout)
{
    ESP8266_Handle *Handle = (ESP8266_Handle *)__Adapter.Device_Handle;
    return Handle->read(output,bytes,timeout);
}

Network_Adapter* Network_Adapter_Init()
{
    __Adapter.Device_Handle = ESP8266_Handle_Init();
    __Adapter.Init = __Adapter_Init;
    __Adapter.TCP_Connect = __Adapter_TCP_Connect;
    __Adapter.UDP_Connect = __Adapter_UDP_Connect;
    __Adapter.read = __read;
    __Adapter.read_nb = __read_nb;
    __Adapter.write = __Adapter_write;
    __Adapter.Get_Network_Status = __Adapter_Get_Network_Status;

    Network_Status = NOINIT;
    return &__Adapter;
}
// void test(void *para)
// {
//     Network_Adapter_Init();
//     ESP8266_Handle *Handle = (ESP8266_Handle *)__Adapter.Device_Handle;
//     while(__Adapter.Init() != Adapter_OK);
//     Handle->Connect_TCP_Server("10.36.236.184",6666,5000);
//     while(1)
//     {
//         HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
//         vTaskDelay(300);
//     }
// }
