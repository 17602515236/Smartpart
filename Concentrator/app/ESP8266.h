#ifndef __ESP8266_H__
#define __ESP8266_H__

#include "Serial.h"

typedef enum{
    ESP8266_OK,
    ESP8266_Failed,

    WIFI_Connect,
    WIFI_NoConnect,

    TCP_UDP_Connect,
    TCP_UDP_NoConnect,

    TCP_UDP_Data_Ready,
    TCP_UDP_NoData,

    Listening,
    Suspending,
    NoListening,
}ESP8266_Status;

typedef enum{
    AT,
    ATE0,
    ATE1,
    CWMODE_CUR_1,
    CWJAP_CUR,
    CIPSTART,
    CIPSEND_TCP,
    CIPSEND_UDP,
}AT_Order;

typedef enum{
    TCP,
    UDP
}COnnect_Type;

enum{
    CHANGED_WIFI_DISCONNECT,
};

typedef enum{
    Err_Timeout = -10,
    Err_No_Server_Connect,
    Err_No_WIFI_Connect,
    Err_Listener_Stop,
}__Error;
typedef enum{
    Serial_ACK_Received,
    Serial_ACK_None,
}__ACK_Status;
typedef struct{
    Serial_Handle *Serial;

    uint8_t *SSID;
    uint8_t SSID_len;
    uint8_t *PASSWD;
    uint8_t PASSWD_len;
    uint8_t Wifi_Connect_Flags;     //WIFI_Connect & WIFI_NoConnect
    uint8_t TCP_UDP_Connect_Flags;  //TCP_UDP_Connect & TCP_UDP_NoConnect
    uint8_t Listening_Flags;        //Listening & Suspending & NoListening
    uint8_t Serial_ACK_Flags;


    ESP8266_Status (*Tran_WaitACK_NoParameter)(uint8_t Cmd_order,
                                               uint32_t ACK_timeout,
                                               uint8_t *Data_Output);
    ESP8266_Status (*Connect_wifi)(char *SSID,
                                   char *PASSWD,
                                   uint32_t timeout);
    ESP8266_Status (*Connect_Server)(char Protol,
                                     char *Server_IP,
                                     uint32_t Server_Port,
                                     uint32_t Local_Port,
                                     uint32_t timeout);
    int (*Transmit_Data)(unsigned char *Buff,
                             int count);
    int (*read_nb)(unsigned char *output,
                   uint32_t bytes);
    int (*read)(unsigned char *output,
                uint32_t bytes,
                uint32_t timeout);
    int (*Start_Listen_ESP8266)();
    int (*Stop_Listen_ESP8266)();
    void (*ReSet)();
    void (*Status_Changed_Callback)(uint8_t);
}ESP8266_Handle;


struct Qset_Item{
    uint8_t len;
    uint8_t *Data;
};
/**
 * @brief Init ESP8266 Handle
 * 
 * @return ESP8266_Handle* 
 */
ESP8266_Handle *ESP8266_Handle_Init(void);

#endif //ESP8266.h