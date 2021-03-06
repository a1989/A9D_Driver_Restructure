#include "interrupt.h"
#include "includes.h"
#include "usart.h"
#include "Communication.h"
#include "IncEncoderControl.h"
#include "MotorControl.h"
//#include "stdlib.h"

uint16_t time3_count = 0; //用于循环计数
uint8_t led_task_cnt = 50;

uint8_t can_send_data_task_cnt = 5;
uint8_t location_write_task_cnt = 60;

//uint8_t motor_limit_cnt = 70;
uint8_t motor_limit_cnt = 60;	//zyg 50
uint8_t gCan_Receive_Flag = 0;

//extern EncoderType GetEncoder;

extern CommunicationBlock 	g_Communication_t;
extern USART_Handle	USART_Handle_t;
extern DevInfo 	DriverBoardInfo;

extern IncEncoderTableInt *IncEncoderTableInt_t;
extern void *MotorVarToInt;

//编码器操作表

//void RegiserIncEncoderInt(void *pPrivate, void (*m_pInterruptHandler)(void *m_pThisPrivate, TIM_HandleTypeDef *hTIM))
//{
//		IncEncoderTable *pEncoderNode = (IncEncoderTable *)malloc(sizeof(IncEncoderTable));
//		IncEncoderTable *pNode;
//	
//		pEncoderNode->pPrivate = pPrivate;
//		pEncoderNode->m_pHandler = m_pInterruptHandler;
//		pEncoderNode->pNext = NULL;
//	
//		if(g_IncEncoderTable == NULL)
//		{
//				g_IncEncoderTable = pEncoderNode;
//		}
//		else
//		{
//				pNode = g_IncEncoderTable;
//				while(pNode->pNext != NULL)
//				{
//						pNode = pNode->pNext;
//				}
//				pNode->pNext = pEncoderNode;
//		}
//}

//void RegiserStepperInt(void *pPrivate, void (*m_pInterruptHandler)(void *m_pThisPrivate, TIM_HandleTypeDef *hTIM, int32_t iPos))
//{
//		StepperTable *pStepperNode = (StepperTable *)malloc(sizeof(StepperTable));
//		StepperTable *pNode;
//	
//		pStepperNode->pPrivate = pPrivate;
//		pStepperNode->m_pHandler = m_pInterruptHandler;
//		pStepperNode->pNext = NULL;
//	
//		if(pStepperNode == NULL)
//		{
//				g_StepperTable = pStepperNode;
//		}
//		else
//		{
//				pNode = g_StepperTable;
//				while(pNode->pNext != NULL)
//				{
//						pNode = pNode->pNext;
//				}
//				pNode->pNext = pStepperNode;
//		}
//}

uint8_t Uart_Receive_Interrupt_Switch (UART_HandleTypeDef* huart, uint8_t* uart_receive_data)
{
	uint8_t statu;

	__HAL_UNLOCK (huart);
	statu = HAL_UART_Receive_IT (huart, uart_receive_data, 1);
	return statu;
}

void HAL_UART_RxCpltCallback (UART_HandleTypeDef* huart) //串口中断回调函数
{
	uint8_t i;

	if (huart->Instance == USART_Handle_t.hUART.Instance) //串口1调试串口
	{
//		uart1_rxbuff[uart1_rxdata_cnt] = uart1_receive_byte;
//		if (uart1_rxbuff[uart1_rxdata_cnt] == '\n')
//		{
//			//printf ("\r\n uart1_rxbuff: %s", uart1_rxbuff);
//			HexStrToByte (uart1_rxbuff, uart1_rdata, UART1_RX_BUFF_LEN + 1); //数据转换
//			uart1_debug_data_len =uart1_rdata[3] ;//数据长度
//			if ((uart1_rdata[0] == UARTI_DEBUG_DATA_FRAME_START) && (uart1_rdata[1] == UARTI_DEBUG_DATA_FRAME_SECOND)) //接受到的是控制数据
//			{
//				memset (UARTI_DEBUG_DATA, 0, 256);
//				for (i=0; i < uart1_debug_data_len; i++) //将数据填充进入数组
//				{
//					UARTI_DEBUG_DATA[i] = uart1_rdata[i];
//					//printf ("\r\n uart1_rdata: %s", i);
//				}
//				// printf ("\r\n uart1_rdata: %x", PULLER_TO_CAR_DATA[4]);
//				uart1_Receive_Right_flag = 1;
//			}
//			else
//			{
//				uart1_Receive_Right_flag = 0;
//			}
//			uart1_rxdata_cnt = 0;
//			memset (uart1_rdata, 0, 256);
//		}
//		else
//		{
//			uart1_rxdata_cnt++;
//		}
//		if (uart1_rxdata_cnt > 255)
//		{
//			uart1_rxdata_cnt = 0;
//		}
//		Uart_Receive_Interrupt_Switch (&huart1, &uart1_receive_byte); //开中断,单字符接收
		Uart_Receive_Interrupt_Switch (&USART_Handle_t.hUART, &USART_Handle_t.chReceiveByte); //开中断,单字符接收
	}
}

//tim call back
void HAL_TIM_PeriodElapsedCallback (TIM_HandleTypeDef *htim)		//10ms
{
		IncEncoderTableInt *pNode0 = IncEncoderTableInt_t;
		IncEncoderTableInt *pNode1 = IncEncoderTableInt_t;
//		IncEncoderTable *pEncoder = g_IncEncoderTable;
//	if (htim->Instance == htim4.Instance) //tim4 interrupt
//	{
//		time3_count++;
//		if ((time3_count % led_task_cnt ) == 0)
//		{
//			led_display_cnt_flag = 1;    // 1s 用于打印计时500ms
//		}
//		if ((time3_count % can_send_data_task_cnt) == 0)
//		{
//			can_send_data_cnt_flag = 1;    //100ms发送一次
//		}
//		/*
//		if ((time3_count % location_write_task_cnt) == 0)
//		{
//			location_write_cnt_flag = 1;    //1200ms发送一次
//		}
//		*/
//		if ((motor_limit_flag == 1) || (motor_limit_flag == 2))
//		{
//			if ((time3_count % motor_limit_cnt) == 0)
//			{
//				motor_limit_cnt_flag = 1;
//			}
//		}
//		if (time3_count > 65530)
//		{
//			time3_count = 0;
//		}
//		DevelopmentFramwork(); // 电机控制 周期10ms
//	}
//		if (htim->Instance == htim3.Instance) //tim3 interrupt
//		{
//			if (htim->Instance->CR1 & 0x0010) //小心注意
//			{
////				GetEncoder.rcnt3 -= 1;
//			}
//			else
//			{
////				GetEncoder.rcnt3 += 1;
//			}
//		}
	
//		if (htim->Instance == htim3.Instance) //tim3 interrupt
//		{
//			DEBUG_LOG("\r\npointer")
//			if (htim->Instance->CR1 & 0x0010) //小心注意
//			{

//			}
//			else
//			{

//			}
//		}	
		
//		while(pEncoder != NULL)
//		{
//				pEncoder->m_pHandler(pEncoder->pPrivate, htim);
//				pEncoder = pEncoder->pNext;
//		}

		while(pNode0 != NULL)
		{
				IncEncoderIntHandler(pNode0->m_pThisPrivate, htim);
				pNode0 = pNode0->pNext;
		}
		
		if (htim->Instance == htim4.Instance) //tim4 interrupt
		{
				while(pNode1 != NULL)
				{
						EncoderIntSpeedHandler(pNode1->m_pThisPrivate);
						pNode1 = pNode1->pNext;
				}			
		}
		
		if (htim->Instance == htim1.Instance) //tim1 interrupt
		{

		}
}

void HAL_CAN_ErrorCallback (CAN_HandleTypeDef* hcan)
{
		printf ("\r\nCAN Error\r\n");
		HAL_CAN_Receive_IT (hcan, CAN_FIFO0); //开CAN接收中断
}

//CAN接收数据
void HAL_CAN_RxCpltCallback (CAN_HandleTypeDef *hcan)
{
	uint8_t i = 0;
	
	/* 比较ID */
//	if (RxMessage.StdId == (*(uint32_t*)g_Communication_t.m_pGetInterfaceConfig(g_Communication_t.m_pThisPrivate, ))
	DEBUG_LOG("\r\nCAN1 data")
	if (hcan->pRxMsg->StdId == DriverBoardInfo.iDriverID)
	{
//		for (i = 0; i < 8; i++)
//		{
//			MAIN_TO_DRIVER_DATA[i] = RxMessage.Data[i];
//		}
			DEBUG_LOG("\r\nstart copy CAN data")
			g_Communication_t.m_pGetHostData(g_Communication_t.m_pThisPrivate, hcan->pRxMsg->Data, hcan->pRxMsg->DLC, eCAN1);
	}
	else
	{
//		can_Receive_Right_flag = 0;
	}
	
	
	
	if (HAL_BUSY == HAL_CAN_Receive_IT (hcan, CAN_FIFO0)) //开启中断接收
	{
		/* Enable FIFO 0 overrun and message pending Interrupt */
		__HAL_CAN_ENABLE_IT (hcan, CAN_IT_FOV0 | CAN_IT_FMP0);
	}
	/* 准备中断接收 */
	//HAL_CAN_Receive_IT (hcan, CAN_FIFO0);
}

//CAN中断函数
//void HAL_CAN_RxCpltCallback (CAN_HandleTypeDef *hcan)
//{
//	uint8_t i = 0;
//	/* 比较ID是否为0x1314 */
//	if (RxMessage.StdId == driver_can_stdid)
//	{
//		for (i = 0; i < 8; i++)
//		{
//			MAIN_TO_DRIVER_DATA[i] = RxMessage.Data[i];
//		}
//		can_Receive_Right_flag = 1; // 接收正确
//		gCan_Receive_Flag = 1;
//	}
//	else
//	{
//		can_Receive_Right_flag = 0;
//	}
//	if (HAL_BUSY == HAL_CAN_Receive_IT (hcan, CAN_FIFO0)) //开启中断接收
//	{
//		/* Enable FIFO 0 overrun and message pending Interrupt */
//		__HAL_CAN_ENABLE_IT (hcan, CAN_IT_FOV0 | CAN_IT_FMP0);
//	}
//	/* 准备中断接收 */
//	//HAL_CAN_Receive_IT (hcan, CAN_FIFO0);
//}
///**
//  * 函数功能: 定时器比较输出中断回调函数
//  * 输入参数: htim：定时器句柄指针
//  * 返 回 值: 无
//  * 说    明: 无
//  */
void HAL_TIM_OC_DelayElapsedCallback (TIM_HandleTypeDef *htim)
{
		//获取当前编码器的步数
//		MotionBlock_t.GetEncoderStep();
		//获取限位开关的值,接触时停,比较输出值设置为比较大的数,即降为慢速,防止怕断出是杂波干扰后以高速无法再启动
		
//		uint16_t count;
//		
//		count =__HAL_TIM_GET_COUNTER (&StepMotor_TIM);
//		__HAL_TIM_SET_COMPARE (&StepMotor_TIM, TIM_CHANNEL_1, (uint16_t)(count + Toggle_Pulse));
	
	
//		tim_Pulse_count++;
//		StepMotor_Pulse_cnt = tim_Pulse_count / (motor_step_value * 2);
	
//			uint16_t iCount;

//			iCount =__HAL_TIM_GET_COUNTER (htim);
//			__HAL_TIM_SET_COMPARE (&htim2, TIM_CHANNEL_1, (uint16_t)(iCount + 100));
	
		MotorIntHandler(MotorVarToInt, htim);
			
}

//void HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin)
//{
//	if (GPIO_Pin == INA20X_CMPOUT_Pin)
//	{
////      Aim_Location = CaptureNumber;
////      Vel_Exp_Val = 0;
////      HAL_TIM_OC_Stop_IT(&htim2,TIM_CHANNEL_1);
////      Drv8711_TORQUE_Set(0); // 设置力矩
////      memset(MAIN_TO_DRIVER_DATA,0,8);
////      over_current_error = 1;
////    __HAL_GPIO_EXTI_CLEAR_IT(INA20X_CMPOUT_Pin);
//	}
//}
