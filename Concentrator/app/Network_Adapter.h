#ifndef __NETWORK_ADAPTER_H__
#define __NETWORK_ADAPTER_H__

#include <stdint.h>
typedef enum
{
    Adapter_OK,
    Adapter_Failed,
} Adapter_Status;

typedef enum{
    NOINIT,
    GOT_IP,
    CONNECT_SERVER,
    NETWORK_DISCONNECT,
}Network_Status_Enum;

typedef struct{
    void *Device_Handle;

    Adapter_Status (*Init)();
    Adapter_Status (*TCP_Connect)(char *IP,
                                  uint32_t port,
                                  uint32_t timeout);
    Adapter_Status (*UDP_Connect)(char *IP,
                                  uint32_t port,
                                  uint32_t local_port,
                                  uint32_t timeout);
    int (*write)(unsigned char *Data, int count);
    int (*read_nb)(unsigned char *output,
                   uint32_t bytes);
    int (*read)(unsigned char *output,
                uint32_t bytes,
                uint32_t timeout);
    Network_Status_Enum (*Get_Network_Status)();
    // int (*recvnb)(char *);
} Network_Adapter;

/**
 * @brief NetWork Device Driver Init
 * 
 * @return Network_Adapter* 
 */
Network_Adapter *Network_Adapter_Init();

// void test(void *para);
#endif //Network_Adapter.h