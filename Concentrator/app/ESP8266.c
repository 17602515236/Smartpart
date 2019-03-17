#include "ESP8266.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
static ESP8266_Handle __ESP8266;
Serial_Handle ESP8266_Serial;

struct Tran_Back{
    char *AT;
    char *Correct_Back;
    uint8_t Retry_times;
};

char Temp[128];

#define ACK_CACHE_SIZE 64
char ACK_Cache[ACK_CACHE_SIZE];
char ACK_len;

SemaphoreHandle_t xSemaphore = NULL;

QueueHandle_t Qset;
struct Qset_Item __Qset_Item;
#define QSET_DEPTH 5
#define QSET_ITEM_SIZE 64   
/**
 * @brief The AT Command and Match ACK,Retry Times
 * 
 */
struct Tran_Back TBMatch[]={
    [AT]={.AT="AT\r\n",.Correct_Back="\r\nOK\r\n",.Retry_times=3},
    [ATE0]={.AT="ATE0\r\n",.Correct_Back="\r\nOK\r\n",.Retry_times=3},
    [ATE1]={.AT="ATE1\r\n",.Correct_Back="\r\nOK\r\n",.Retry_times=3},
    [CWMODE_CUR_1]={.AT="AT+CWMODE_CUR=1\r\n",.Correct_Back="\r\nOK\r\n",.Retry_times=3},
    [CWJAP_CUR]={.AT="AT+CWJAP_CUR=\"%s\",\"%s\"\r\n",.Correct_Back="\r\nOK\r\n",.Retry_times=3},
    [CIPSTART]={.AT="AT+CIPSTART=\"%s\",\"%s\",%d\r\n",.Correct_Back="\r\nOK\r\n",.Retry_times=3},
    [CIPSEND_TCP]={.AT="AT+CIPSEND=%d\r\n",.Correct_Back="\r\n>",.Retry_times=3},
    [CIPSEND_UDP]={.AT="AT+CIPSEND=%d\r\n",.Correct_Back="\r\n>",.Retry_times=3},
};

const TaskHandle_t Listened_TaskHandle;

//Transmit AT Command
#define AT_TRANSMIT(x,y) do{\
    HAL_UART_Transmit(__ESP8266.Serial->huart,(uint8_t *)x,y,0xFFFF); \
}while(0)



// static void Suspend_Listened()
// {
//     if(Listened_TaskHandle != NULL && __ESP8266.Listening_Flags == Listening)
//     {
//         vTaskSuspend(Listened_TaskHandle);
//         __ESP8266.Listening_Flags = Suspending;
//     }
// }

// static void Resume_Listened()
// {
//     if(Listened_TaskHandle != NULL && __ESP8266.Listening_Flags == Suspending)
//     {
//         vTaskSuspend(Listened_TaskHandle);
//         __ESP8266.Listening_Flags = Listening;
//     }
// }

static int Copy_to_ACK_Cache(uint8_t *pBuff,uint16_t len)
{
    xSemaphoreTake(xSemaphore,portMAX_DELAY);
    if (len > ACK_CACHE_SIZE)
        len = ACK_CACHE_SIZE;
    memcpy(ACK_Cache, pBuff, len);
    ACK_len = len;
    __ESP8266.Serial_ACK_Flags = Serial_ACK_Received;
    xSemaphoreGive(xSemaphore);
    return len;
}

static void Command_Process_Over(void)
{
    memset(ACK_Cache,0,ACK_CACHE_SIZE);
    ACK_len = 0;
    __ESP8266.Serial_ACK_Flags = Serial_ACK_None;
}




/**
 * @brief   ESP8266<===UART===>STM32,STM32 Transmit an command and wait ACK Back with ESP8266 
 *          until timeout
 * 
 * @param AT            :AT Command STM32 ===> ESP8266
 * @param Correct_ACK   :The Correct ACK from ESP8266(Only receive specify ACK this Function can return ok)
 * @param Data_Output   :output the receive ACK if this param is not NULL
 * @param ACK_timeout   :specify timeout
 * @param Retry_Times   :when timeout or receive ERROR ACK STM32 will retransmit AT command with Retrytimes
 * @return ESP8266_Status 
 */
static ESP8266_Status __Trans_WaitACK_UntilTimeout(char *AT,
                                                   uint32_t len,
                                                   char *Correct_ACK,
                                                   uint8_t *Data_Output,
                                                   uint32_t ACK_timeout,
                                                   uint8_t Retry_Times)
{
    uint32_t Last_Time;
    uint8_t i = 0;
    uint8_t ret = ESP8266_Failed;
    for (i = 0; i < Retry_Times; i++)
    {
        //Transmit AT Command
        xSemaphoreTake(xSemaphore, portMAX_DELAY);
        Command_Process_Over();
        xSemaphoreGive(xSemaphore);
        AT_TRANSMIT(AT, len);
        Last_Time = HAL_GetTick() + ACK_timeout;
        while (HAL_GetTick() < Last_Time)
        {
            //Check the flag of SerilPort if Data is Ready or not
            if (__ESP8266.Serial_ACK_Flags == Serial_ACK_Received)
            {
                xSemaphoreTake(xSemaphore, portMAX_DELAY);
                if (strstr((const char *)ACK_Cache, (const char *)Correct_ACK) != NULL)
                {
                    if (Data_Output != NULL)
                        memcpy(Data_Output, ACK_Cache,ACK_len);
                    ret = ESP8266_OK;
                }
                // if (strstr((const char *)ACK_Cache, (const char *)"ERROR\r\n") != NULL)
                // {
                //     __ESP8266.Serial_ACK_Flags = Serial_ACK_None;
                //     break;
                // }
                Command_Process_Over();
                xSemaphoreGive(xSemaphore);
                if (ret == ESP8266_OK)
                    return ret;
            }
            vTaskDelay(10);
        }
    }
    xSemaphoreTake(xSemaphore, portMAX_DELAY);
    Command_Process_Over();
    xSemaphoreGive(xSemaphore);
    return ESP8266_Failed;
}

/**
 * @brief Transmit Standard AT Command without Parameter  and wwait correct ACK from ESP8266
 * 
 * @param Cmd_order     :No parameter AT Command Order(look enum AT_Order)
 * @param ACK_timeout   :Timeout
 * @param Data_Output   :Output Receive ACK if this parameter is not NULL
 * @return ESP8266_Status 
 */
static ESP8266_Status __Tran_WaitACK_NoParameter(uint8_t Cmd_order,
                                                 uint32_t ACK_timeout,
                                                 uint8_t *Data_Output)
{
    return __Trans_WaitACK_UntilTimeout(TBMatch[Cmd_order].AT,
                                        strlen(TBMatch[Cmd_order].AT),
                                        TBMatch[Cmd_order].Correct_Back,
                                        Data_Output,
                                        ACK_timeout,
                                        TBMatch[Cmd_order].Retry_times);
}

/**
 * @brief Connect to specify wifi
 * 
 * @param SSID              :wifi SSID
 * @param PASSWD            :wifi password
 * @param timeout           :COnnect timeout
 * @return ESP8266_Status 
 */
static ESP8266_Status __Connect_wifi(char *SSID,
                                     char *PASSWD,
                                     uint32_t timeout)
{
    sprintf(Temp, TBMatch[CWJAP_CUR].AT, SSID, PASSWD);
    //Transmit AT Command and Wait ACK
    if (__Trans_WaitACK_UntilTimeout(Temp,
                                     strlen(Temp),
                                     TBMatch[CWJAP_CUR].Correct_Back,
                                     NULL,
                                     timeout,
                                     TBMatch[CWJAP_CUR].Retry_times) == ESP8266_Failed)
    {
        __ESP8266.Wifi_Connect_Flags = WIFI_NoConnect;
        return ESP8266_Failed;
    }
    __ESP8266.Wifi_Connect_Flags = WIFI_Connect;
    return ESP8266_OK;
}

// static Network_Status __Device_Status(void)
// {
//     memset(Temp,0,128);
//     if (__Trans_WaitACK_UntilTimeout("AT+CIPSTATUS\r\n",
//                                      strlen("AT+CIPSTATUS\r\n"),
//                                      "\r\nSTATUS:",
//                                      (uint8_t *)Temp,
//                                      1000,
//                                      2) == ESP8266_Failed)
//     {
//         return ESP8266_Failed;
//     }
//     if (strstr(Temp, "\r\nSTATUS:2") != NULL)
//         return GOT_IP;
//     if (strstr(Temp, "\r\nSTATUS:3") != NULL)
//         return CONNECT_SERVER;
//     if (strstr(Temp, "\r\nSTATUS:4") != NULL)
//         return NETWORK_DISCONNECT;
//     if (strstr(Temp, "\r\nSTATUS:5") != NULL)
//         return WIFI_DISCONNECT;
//     return WIFI_DISCONNECT;
// }

static ESP8266_Status __Connect_Server(char Type,
                                       char *Server_IP,
                                       uint32_t Server_Port,
                                       uint32_t Local_Port,
                                       uint32_t timeout)
{
    memset(Temp,0,128);
    if (Type == TCP)
        sprintf(Temp, "AT+CIPSTART=\"TCP\",\"%s\",%ld\r\n", Server_IP, Server_Port);
    else
    {
        if (Local_Port == 0)
            sprintf(Temp, "AT+CIPSTART=\"UDP\",\"%s\",%ld\r\n", Server_IP, Server_Port);
        else
            sprintf(Temp, "AT+CIPSTART=\"UDP\",\"%s\",%ld,%ld,0\r\n", Server_IP, Server_Port,Local_Port);
    }

    if (__Trans_WaitACK_UntilTimeout("AT+CIPCLOSE\r\n",
                                     strlen("AT+CIPCLOSE\r\n"),
                                     "\r\nOK\r\n",
                                     NULL,
                                     200,
                                     2) == ESP8266_OK)
    {
        __ESP8266.TCP_UDP_Connect_Flags = TCP_UDP_NoConnect;
    }
    if (__Trans_WaitACK_UntilTimeout(Temp,
                                     strlen(Temp),
                                     "\r\nOK\r\n",
                                     NULL,
                                     timeout,
                                     2) == ESP8266_Failed)
    {
        __ESP8266.TCP_UDP_Connect_Flags = TCP_UDP_NoConnect;
        return ESP8266_Failed;
    }
    __ESP8266.TCP_UDP_Connect_Flags = TCP_UDP_Connect;
    return ESP8266_OK;
}

/**
 * @brief Reset ESP8266
 * 
 */
static void __RST(void)
{
    __ESP8266.Wifi_Connect_Flags = WIFI_NoConnect;
    AT_TRANSMIT("AT+RST\r\n",strlen("AT+RST\r\n"));
}

static int __read_nb(unsigned char *output,uint32_t bytes)
{
    int count = 0;

    char Buff[64] = {0};
    char *p = NULL;
    if(__ESP8266.Wifi_Connect_Flags != WIFI_Connect)
        return Err_No_WIFI_Connect;
    if(__ESP8266.TCP_UDP_Connect_Flags != TCP_UDP_Connect)
        return Err_No_Server_Connect;
    if(__ESP8266.Listening_Flags == NoListening)
        return Err_Listener_Stop;
    if (xQueueReceive(Qset, Buff, 0) == pdTRUE)
    {
        p = strstr((const char *)Buff,":");
        p++;
        count = Buff[0] - (uint8_t)(p - Buff);
        if (bytes > count)
            bytes = count;
        memcpy(output,p,bytes);
        return count;
    }
    return 0;
}

static int __read(unsigned char *output,uint32_t bytes,uint32_t timeout)
{
    int rc = 0;
    uint32_t l_time = HAL_GetTick() + timeout;
    while(HAL_GetTick() < l_time)
    {
        rc = __read_nb(output,bytes);
        if (rc < 0)
            return rc;
        else if(rc > 0)
            return rc;
    }
    return Err_Timeout;
}

/**
 * @brief 
 * 
 * @param Buff 
 * @param count 
 * @return int 
 */
static int __Transmit_Data(unsigned char *Buff,
                               int count)
{
    memset(Temp,0,128);
    sprintf(Temp,"AT+CIPSEND=%d\r\n",count);
    if (__Trans_WaitACK_UntilTimeout(Temp,
                                     strlen(Temp),
                                     TBMatch[CIPSEND_TCP].Correct_Back,
                                     NULL,
                                     3000,
                                     TBMatch[CIPSEND_TCP].Retry_times) == ESP8266_Failed)
    {
        return -1;
    }

    memcpy(Temp,Buff,count);
    Temp[count]='\r';
    Temp[count + 1]='\n';
    if (__Trans_WaitACK_UntilTimeout(Temp,
                                     count,
                                     "SEND OK\r\n",
                                     NULL,
                                     3000,
                                     1) == ESP8266_Failed)
    {
        return -1;
    }
    return count;
}

/**
 * @brief Listening to ESP8266 ,Sync the Receive data to set 
 *          the flags,this function will not comeout; 
 * 
 */
void __Listen_ESP8266(void *param)
{
    while(1)
    {
        // if (__ESP8266.Wifi_Connect_Flags == WIFI_Connect &&
        //     __ESP8266.TCP_UDP_Connect_Flags == TCP_UDP_Connect)
        // {
        if (Serial_Get_Running_Flags(__ESP8266.Serial) == DMA_Stopping)
            Serial_Start_DMA_Receive(__ESP8266.Serial);
        if (Serial_Get_Receive_Flags(__ESP8266.Serial) == Serial_Data_Ready)
        {
            Serial_Stop_DMA_Receive(__ESP8266.Serial);
            uint8_t *pBuff = Serial_Get_Receive_Buff(__ESP8266.Serial);
            uint16_t len = Serial_Get_Receive_Len(__ESP8266.Serial);
            //Sync Data
            /*Receive UDP or TCP Data*/
            if (strstr((const char *)Serial_Get_Receive_Buff(__ESP8266.Serial), "+IPD,") != NULL)
            {
                pBuff[0] = len;
                xQueueSend(Qset, pBuff, 0);
            }
            if (strstr((const char *)Serial_Get_Receive_Buff(__ESP8266.Serial), "WIFI DISCONNECT\r\n") != NULL)
            {
                if(__ESP8266.Status_Changed_Callback != NULL)
                    __ESP8266.Status_Changed_Callback(CHANGED_WIFI_DISCONNECT);
            }
            else
            {
                Copy_to_ACK_Cache(pBuff,len);
            }
        }
        // }
    }
}

static int __Start_Listen_ESP8266(void)
{
    if (__ESP8266.Listening_Flags == NoListening)
    {
        __ESP8266.Listening_Flags = Listening;
        // xTaskCreate(__Listen_ESP8266, "ESP8266_Listened_Task", 256, NULL, 8, Listened_TaskHandle);
        xTaskCreate(__Listen_ESP8266, "Listened_Task", 64, NULL, 8,&Listened_TaskHandle);
        return ESP8266_OK;
    }
    return ESP8266_Failed;
}

static int __Stop_Listen_ESP8266(void)
{
    if(__ESP8266.Listening_Flags == Listening)
    {
        __ESP8266.Listening_Flags = NoListening;
        vTaskDelete(Listened_TaskHandle);
        return ESP8266_OK;
    }
    return ESP8266_Failed;
}

ESP8266_Handle *ESP8266_Handle_Init(void)
{
    new_Serial(&ESP8266_Serial,&huart1);
    __ESP8266.Serial = &ESP8266_Serial;

    __ESP8266.Wifi_Connect_Flags = WIFI_NoConnect;
    __ESP8266.TCP_UDP_Connect_Flags = TCP_UDP_NoConnect;
    __ESP8266.Listening_Flags = NoListening;
    __ESP8266.Serial_ACK_Flags = Serial_ACK_None;

    __ESP8266.Tran_WaitACK_NoParameter = __Tran_WaitACK_NoParameter;
    __ESP8266.Connect_wifi = __Connect_wifi;
    __ESP8266.Connect_Server = __Connect_Server;
    __ESP8266.Start_Listen_ESP8266 = __Start_Listen_ESP8266;
    __ESP8266.Transmit_Data = __Transmit_Data;
    __ESP8266.read = __read;
    __ESP8266.read_nb = __read_nb;
    __ESP8266.Stop_Listen_ESP8266 = __Stop_Listen_ESP8266;
    __ESP8266.ReSet = __RST;
    Qset = xQueueCreate(QSET_DEPTH,QSET_ITEM_SIZE);
    xSemaphore = xSemaphoreCreateMutex();
    __ESP8266.Start_Listen_ESP8266();
    return &__ESP8266;
}