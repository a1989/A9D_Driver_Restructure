#include "bsp.h"
#include "includes.h"

void BSP_Initializes (void)
{
	if (bsp_init_flag == 0)
	{
		//At24c512_Init();//初始化EEP
		Driver_Board_Parameter_Init();//参数读取，必须放在最前面
		Driver_Can_Init();//CAN滤波器初始化
		Drv8711_Init(); //DRV8711参数写入，默认参数
		PID_Init();//电机PID参数初始化
		HAL_TIM_OC_Stop_IT (&htim2, TIM_CHANNEL_1);//停止PWM比较输出
		//HAL_TIM_OC_Start_IT ( &htim2,TIM_CHANNEL_1 );
		TIM3_Encoder_Switch (1);//开编码器
		TIM4_IT_Interrupt_Switch (1);//开启定时器4中断
		TIM1_IT_Interrupt_Switch (1);//开启定时器1中断
		HAL_CAN_Receive_IT (&hcan, CAN_FIFO0); //开CAN接收中断

		HAL_ADCEx_Calibration_Start (&hadc1);
		HAL_ADC_Start_DMA (&hadc1, ADC_Get_Info, 3);
		Uart_Receive_Interrupt_Switch (&huart1, &uart1_receive_byte);//开中断
		bsp_init_flag = 1;
		printf ("\r\n hwrobot steper motor driver board bsp init is ok !");
	}
}

void BSP_Init(void)
{
		MX_GPIO_Init();
		MX_DMA_Init();
		MX_USART1_UART_Init();
		MX_CAN_Init();
		MX_I2C2_Init();
		MX_SPI2_Init();
		MX_IWDG_Init();
		MX_IWDG_Init();
}

void ReadBoardParams(void)
{
		
}

void TIM_Init(void)
{
		//需要用到TIM1~TIM4
		//分别用于OC输出控制电机,编码器计数溢出时判断方向,事件处理
		//根据不同硬件在tim.c中修改配置
		MX_TIM1_Init();
		MX_TIM2_Init();
		MX_TIM3_Init();
		MX_TIM4_Init();
}
