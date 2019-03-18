#include "Ra02.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
tRadioDriver *mradio = NULL;
static SemaphoreHandle_t wr_lock = NULL;
static QueueSetHandle_t Qset = NULL;
/**
 * @brief Transimit Lora Package
 * 
 * @param Package 
 * @param len 
 * @param timeout 
 * @param Retry 
 * @return int 
 */
int Transmit_Package(uint8_t *Package, uint32_t len,uint32_t timeout,uint32_t Retry)
{
	uint32_t last_time=0;
	uint8_t times = Retry;
	xSemaphoreTake(wr_lock,portMAX_DELAY);
	while (times--)
	{
		mradio->SetTxPacket(Package, (uint16_t)len);
		last_time = HAL_GetTick() + timeout;
		while (HAL_GetTick() < last_time)
		{
			switch (mradio->Process())
			{
			case RF_TX_DONE:		//Transmit OK
				mradio->StartRx();
				xSemaphoreGive(wr_lock);
				return TX_OK;
			case RF_TX_TIMEOUT:		//Timeout
				last_time = 0;
				break;
			default:
				break;
			}
		}
	}
	xSemaphoreGive(wr_lock);
	return TX_FAILED;
}

int Lora_Wait_Package_nb(uint8_t *output)
{
	if (xQueueReceive(Qset, output, 0) == pdTRUE)
		return output[0];
	return 0;
}

int Lora_Wait_Package(uint8_t *output,uint32_t Timeout)
{
	uint32_t recv_bytes = 0;
	uint32_t last_time = HAL_GetTick() + Timeout;
	while(HAL_GetTick() < last_time)
	{
		if ((recv_bytes = Lora_Wait_Package_nb(output)) > 0)
			return recv_bytes;
		vTaskDelay(10);
	}
	return 0;
}


void Lora_Receive_Task(void *Param)
{
	uint8_t Buff[64];
	uint16_t Bytes;
	while(1)
	{
		xSemaphoreTake(wr_lock, portMAX_DELAY);
		switch (mradio->Process())
		{
		case RF_RX_DONE:
			mradio->GetRxPacket(&Buff[1], &Bytes);
			Buff[0] = (uint8_t)Bytes;
			xQueueSend(Qset, Buff, 0);
			break;
		case RF_RX_TIMEOUT:
			mradio->StartRx();
			break;
		}
		xSemaphoreGive(wr_lock);
		vTaskDelay(50);
	}
}
int Lora_Init(void)
{
	mradio = RadioDriverInit();
	wr_lock = xSemaphoreCreateMutex();
	Qset = xQueueCreate(5,64);
	xTaskCreate(Lora_Receive_Task,"Lora_Receive_Task",64,NULL,8,NULL);
	if (mradio == NULL)
		return -1;
	mradio->Init();
	return 0;
}



////************************************
//// Method:    Wait_Package
//// FullName:  Wait_Package
//// Access:    public
//// Returns:   int
//// Qualifier:	�ȴ���ȡ���ݰ�
//// Parameter: uint8_t * output
//// Parameter: uint32_t * len
////************************************
//int Wait_Package(uint8_t *output,uint16_t *len)
//{
//	uint32_t time_count = 100000;
//	mradio->StartRx();		//��������
//	while (time_count--)
//	{
//		switch (mradio->Process())
//		{
//		case RF_RX_DONE:	//�������
//			mradio->GetRxPacket((void *)output, len);	//��ȡ����
//			return RX_OK;
//		case RF_RX_TIMEOUT:	//���ճ�ʱ
//			return RX_Timeout;
//		default:break;
//		}
//	}
//	return RX_Timeout;
//}
//void Ra02_Task(void *para)
//{
//	uint32_t ttt = 0;
//	Lora_Init();
//#ifdef SLAVE
//	char *str = "abcd";
//	mradio->SetTxPacket(str, 4);
//#elif MASTER
//	char RecvBuff[32] = { 0 };
//	uint16_t Recv_Bytes = 0;
//	mradio->StartRx();		//��������
//#endif
//	while (1)
//	{
//#ifdef SLAVE
//		switch (mradio->Process())
//		{
//		case RF_RX_DONE:	//�������
//			ttt++;
//			break;
//		case RF_TX_DONE:
//			printf("Slave send %s ok\n", str);
//			vTaskDelay(1000);
//			mradio->SetTxPacket(str, 4);
//			break;
//		case RF_TX_TIMEOUT:
//			ttt++;
//			break;
//		case RF_RX_TIMEOUT:
//			ttt++;
//			break;
//		default:break;
//		}
//#elif MASTER
//		switch (mradio->Process())
//		{
//		case RF_RX_DONE:	//�������
//			mradio->GetRxPacket((void *)RecvBuff, &Recv_Bytes);	//��ȡ����
//			printf("Master Receive packet:%s\n", RecvBuff);
//			mradio->StartRx();		//��������
//			break;
//		case RF_TX_DONE:
//			break;
//		case RF_TX_TIMEOUT:
//			break;
//		case RF_RX_TIMEOUT:
//			mradio->StartRx();		//��������
//			break;
//		default:break;
//		}
//#endif
//	}
//}