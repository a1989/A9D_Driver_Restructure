#include "TMC2590.h"
#include "includes.h"

//定义所需配置的寄存器结构变量
unRegCHOPCONF   g_unRegChopCofig;
unRegSMARTEN    g_unRegSmartenConfig;
unRegDRVCONF    g_unRegDrvConfConfig;
unRegDRVCTRL    g_unRegDrvCtrlConfig;
unRegSGCSCONF   g_unRegSgcsConfConfig;

char arrErrorString[16];

uint8_t motor_step_value = 16;//细分设置，默认16细分
uint8_t TMC2590_error_status = 0;

//通过返回码获得字符串
static char *ErrorCodeToString(HAL_StatusTypeDef iErrorCode)
{
    int i;
    for(i = 0; i < sizeof(arrErrorString); i++)
    {
        arrErrorString[i] = 0;
    }
    
    switch(iErrorCode)
    {
        case HAL_OK:
          strcpy(arrErrorString, "OK");
          break;
        case HAL_ERROR:   
          strcpy(arrErrorString, "ERROR");
          break;
        case HAL_BUSY:   
          strcpy(arrErrorString, "BUSY");
          break;
        case HAL_TIMEOUT:  
          strcpy(arrErrorString, "TIMEOUT");
          break;
        default:
          strcpy(arrErrorString, "UNKNOWN");
          break;        
    }
    
    return arrErrorString;
}

//SPI发送数据, 分3个Byte发送
HAL_StatusTypeDef SPI_TMC2590_SendByte (uint32_t iWriteData, uint32_t *iRecvData)
{
    uint8_t Send_Buffer[3];
    uint8_t Recv_Buffer[3];
    
    Send_Buffer[0] = (iWriteData >> 16) & 0xFF;
    Send_Buffer[1] = (iWriteData >> 8) & 0xFF;
    Send_Buffer[2] = (iWriteData) & 0xFF;
			
//		printf("\r\n 0x%x", Send_Buffer[0]);
//		printf("\r\n 0x%x", Send_Buffer[1]);
//		printf("\r\n 0x%x", Send_Buffer[2]);
	
		SPI_TMC2590_CS_HIGH();
		Delay_ms (1);
    SPI_TMC2590_CS_LOW();
//		Delay_ms (5);
	
    HAL_StatusTypeDef iStatus = HAL_SPI_TransmitReceive (&TMC2590_SPI, Send_Buffer, Recv_Buffer, 3, 0xFFFFFF);    
//    HAL_StatusTypeDef iStatus = HAL_SPI_Transmit (&TMC2590_SPI, Send_Buffer, 3, 0xFFFFFF);
    
    *iRecvData = (Recv_Buffer[0] << 16) | (Recv_Buffer[1] << 8) | (Recv_Buffer[2]);
    SPI_TMC2590_CS_HIGH();
		Delay_ms (5);
		printf("\r\n Send Data 0x%x", iWriteData);
		printf("\r\n Recv Data 0x%x", *iRecvData);
    return iStatus;
}

//SPI写，将地址与寄存器值合并
static HAL_StatusTypeDef SPI_TMC2590_Write (uint32_t addr, uint32_t data)
{
    uint32_t Temp = 0;
    uint32_t iRead = 0;
    /* Select the FLASH: Chip Select low */
//    SPI_TMC2590_CS_HIGH();
    
    Temp = (addr) | (data);
    HAL_StatusTypeDef iStatus = SPI_TMC2590_SendByte (Temp, &iRead);

    /* Deselect the FLASH: Chip Select high */
//    SPI_TMC2590_CS_LOW();
    
    return iStatus;
}

//设置TMC2590寄存器
HAL_StatusTypeDef TMC2590_SetReg (uint32_t iRegAddr, uint32_t iValue)
{
		HAL_StatusTypeDef iStatus;
	
		iStatus = SPI_TMC2590_Write (iRegAddr, iValue);
    
    return iStatus;
}

//读取TMC2590的状态
HAL_StatusTypeDef TMC2590_ReadSingleStatus (ReadSelect iSelect, uint32_t *iValue)
{
    //根据DRVCONF的RDSEL设置选择读取的参数
    g_unRegDrvConfConfig.structReg.iRDSEL = iSelect;    
    uint32_t iTemp = REG_DRVCONF | g_unRegDrvConfConfig.iRegValue;
    
    HAL_StatusTypeDef iStatus = SPI_TMC2590_SendByte (iTemp, iValue);
    
    return iStatus;
}

bool ReadStatuBit(uint32_t iStatus, uint32_t iPos)
{
		return ( (iStatus & (1 << iPos)) != 0 );
}

void TMC2590_GetStatus(StructStatus *structStatus)
{
		uint32_t iRead = 0;
		bool bIsOccur = false;
	
		HAL_StatusTypeDef iStatus = SPI_TMC2590_SendByte (0xEF040, &iRead);
		iStatus = SPI_TMC2590_SendByte (0xEF050, &iRead);
	
		iRead = (iRead >> 4) & 0xFFFFF;
	
		structStatus->StallGuard = (iRead >> 10) & 0x3FF;
		structStatus->iValue = iRead;
	
		//检测堵转
		if(ReadStatuBit(iRead, SG_POS))
		{
				
		}
		
		//A相短路
		if(ReadStatuBit(iRead, SG_SHORTA))
		{
				driver_over_current_flag = 1;
		}
		
		//B相短路
		if(ReadStatuBit(iRead, SG_SHORTB))
		{
				driver_over_current_flag = 1;
		}
		
		//A相开路
		if(ReadStatuBit(iRead, SG_OLA))
		{
				
		}		

		//B相开路
		if(ReadStatuBit(iRead, SG_OLB))
		{
				
		}				
		
		structStatus->iStallOccurs = iRead  & 0x1;
		
//		iStatus = SPI_TMC2590_SendByte (0xEF060, &iRead);		
}

// 设置细分
bool TMC2590_SetMicroStep (uint16_t iValue, HAL_StatusTypeDef *iErrorCode)
{
    *iErrorCode = HAL_OK;
    HAL_StatusTypeDef iStatus;
    uint32_t iSetRegister = REG_DRVCTRL;
    uint16_t iSetValue = 0;
    
    switch (iValue)
    {
        case 1:
            iSetValue = MICROSTEP_1;          //FullStep
            break;
        case 2:
            iSetValue = MICROSTEP_2;          //2细分
            break;
        case 4:
            iSetValue = MICROSTEP_4;          //4细分
            break;
        case 8:
            iSetValue = MICROSTEP_8;          //8细分
            break;
        case 16:
            iSetValue = MICROSTEP_16;         //16细分
            break;
        case 32:
            iSetValue = MICROSTEP_32;         //32细分
            break;
        case 64:
            iSetValue = MICROSTEP_64;         //64细分
            break;
        case 128:
            iSetValue = MICROSTEP_128;        //128细分
            break;
        case 256:
            iSetValue = MICROSTEP_256;        //256细分
            break;
        default:
            iSetValue= MICROSTEP_32;
    }

    g_unRegDrvCtrlConfig.structReg.iMRES = iSetValue;
    iStatus = TMC2590_SetReg (iSetRegister, g_unRegDrvCtrlConfig.iRegValue);
    if(iStatus != HAL_OK)
    {
        *iErrorCode = iStatus;
        return false;
    }
    
    return true;
}

//设置电流
bool TMC2590_SetCurrent (uint16_t iValue, HAL_StatusTypeDef *iErrorCode)
{
		HAL_StatusTypeDef iStatus;
		if(iValue > 31)
		{
				iValue = 31;
		}
		
		g_unRegSgcsConfConfig.structReg.iCS = iValue;
		
	  iStatus = TMC2590_SetReg (REG_SGCSCONF, g_unRegSgcsConfConfig.iRegValue);
    if(iStatus != HAL_OK)
    {
        *iErrorCode = iStatus;
        return false;
    }
		
    return true;
}

//设置力矩
bool TMC2590_SetTorque (uint8_t value, HAL_StatusTypeDef *iErrorCode)
{
//    uint16_t tmp0;
//    uint8_t  tmp1;
//
//    tmp0 = drv8711_torque_value;
//    tmp1 = value;
//    tmp0 = (tmp0 & 0xff00) | tmp1;
//    drv8711_torque_value = tmp0;
//    SPI_DRV8711_Write (REG_CURRENT, tmp0);
//    return tmp0;
		
	
    return true;
}

static bool TMC2590_SetTypicalCHOPCONF(HAL_StatusTypeDef *iErrorCode)
{    
    *iErrorCode = HAL_OK;
    uint32_t iRegValue = 0x0000;
    
    //CHOPCONF[16:15]位设置TBL(Blanking Time)
    //%00:16,%01:24,%10:36,%11:54
    //低电流时选择16或24, 高电流时选择36或54
    g_unRegChopCofig.structReg.iTBL = 2;
      
    //CHOPCONF[14]位设置CHM(Chopper mode)
    //0:标准模式(SpreadCycle),1:Constant tOFF with fast decay time
    //大多数情况下使用标准模式
    g_unRegChopCofig.structReg.iCHM = 0;
    
    //CHOPCONF[13]位设置RNDTF(Random TOFF time)
    //0:Chopper off time is fixed as set by bits tOFF,1:Random mode
    g_unRegChopCofig.structReg.iRNDTF = 0;
    
    //CHOPCONF[12:11]位设置HDEC(Hysteresis decrement interval or Fast decay mode)
    //%00:16,%01:32,%10:48,%11:64
    g_unRegChopCofig.structReg.iHDEC = 0;
    
    //CHOPCONF[10:7]位设置HEND(Hysteresis end (low) value or Sine wave offset)
    g_unRegChopCofig.structReg.iHEND = 3;       //%0011
    
    //CHOPCONF[6:4]位设置HSTRT(Hysteresis start value or Fast decay time setting)
    g_unRegChopCofig.structReg.iHSTRT = 0x3;    //%011
    
    //CHOPCONF[3:0]位设置TOFF(Off time/MOSFET disable)
    g_unRegChopCofig.structReg.iTOFF = 0x4;    //%0100
    
    iRegValue = g_unRegChopCofig.iRegValue;
    
		printf("\r\n Set CHOPCONF 0x%x", iRegValue);
    HAL_StatusTypeDef iStatus = TMC2590_SetReg (REG_CHOPCONF, iRegValue);
    if(iStatus != HAL_OK)
    {
        *iErrorCode = iStatus;
        return false;
    }
    
    return true;
}

static bool TMC2590_SetTypicalSMARTEN(HAL_StatusTypeDef *iErrorCode)
{
    *iErrorCode = HAL_OK;
    uint32_t iRegValue = 0x0000;
    
    //SMARTEN[15]位设置SEIMIN(Minimum CoolStep current)
    //0: 1/2 CS current setting, 1: 1/4 CS current setting
    g_unRegSmartenConfig.structReg.iSEIMIN = 0x1;

    //SMARTEN[14:13]位设置SEDN(Current decrement speed)
    //%00: 32,%01: 8,%10: 2,%11: 1
    g_unRegSmartenConfig.structReg.iSEDN = 0x0;

    //SMARTEN[11:8]位设置SEMAX(Upper CoolStep threshold as an offset from the lower threshold)
    g_unRegSmartenConfig.structReg.iSEMAX = 0x2;

    //SMARTEN[6:5]位设置SEUP(Current increment size)
    //%00: 1,%01: 2,%10: 4,%11: 8
    g_unRegSmartenConfig.structReg.iSEUP = 0x0;

    //SMARTEN[3:0]位设置SEMIN(Lower CoolStep threshold/CoolStep disable)
    g_unRegSmartenConfig.structReg.iSEMIN = 0x2;
    
    iRegValue = g_unRegSmartenConfig.iRegValue;
    
		printf("\r\n Set SMARTEN 0x%x", iRegValue);
    HAL_StatusTypeDef iStatus = TMC2590_SetReg (REG_SMARTEN, iRegValue);
    if(iStatus != HAL_OK)
    {
        *iErrorCode = iStatus;
        return false;
    }
    
    return true;
}

static bool TMC2590_SetTypicalDRVCONF(HAL_StatusTypeDef *iErrorCode)
{
    *iErrorCode = HAL_OK;
    uint32_t iRegValue = 0x0000;
    
    g_unRegDrvConfConfig.iRegValue = 0;
    
    //Reserved TEST mode, 常规操作下清掉
    g_unRegDrvConfConfig.structReg.iTST = 0;
    
    //Slope control
    //SLP2, SLPH1, SLPH0,Gate driver strength 1 to 7.
    //SLP2, SLPL1, SLPL0,Gate driver strength 1 to 7.
    g_unRegDrvConfConfig.structReg.iSLPH = 2;
		g_unRegDrvConfConfig.structReg.iSLPL = 2;
    
    g_unRegDrvConfConfig.structReg.iSLP2 = 0;
    
    //0: Short to GND protection is enabled.
    //1: Short to GND protection is disabled.
    g_unRegDrvConfConfig.structReg.iDIS_S2G = 0;
    
    //Short detection delay for high-side and low-side FETs
    //%00: 3.2μs.%01: 1.6μs.%10: 1.2μs.%11: 0.8μs.
    g_unRegDrvConfConfig.structReg.iTS2G = 0;
    
    //STEP/DIR interface disable
    //0: Enable STEP/DIR operation.1: Disable STEP/DIR operation. SPI interface is used to move motor.
    g_unRegDrvConfConfig.structReg.iSDOFF = 0;
    
    //Sense resistor voltage-based current scaling
    //0: Full-scale sense resistor voltage is 325mV.1: Full-scale sense resistor voltage is 173mV.
    //(Full-scale refers to a current setting of 31.)
    g_unRegDrvConfConfig.structReg.iVSENSE = 0;
    
    //Select value for read out (RD bits)
    //%00:Microstep position read back
    //%01:StallGuard2 level read back
    //%10:StallGuard2 and CoolStep current level read back
    //%11:All status flags and detectors
    g_unRegDrvConfConfig.structReg.iRDSEL = 0x2;
    
    //Overtemperature sensitivity
    //0: Shutdown at 150°C
    //1: Sensitive shutdown at 136°C
    g_unRegDrvConfConfig.structReg.iOTSENS = 0;
    
    //Short to GND detection sensitivity
    //0: Low sensitivity,1: High sensitivity – better protection for high side FETs
    g_unRegDrvConfConfig.structReg.iSHRTSENS = 0;
    
    //Enable Passive fast decay / 5V undervoltage threshold
    //0: No additional motor dampening. 
    //1: Motor dampening to reduce motor resonance at medium velocity. In addition, this bit reduces the lower nominal operation voltage limit from 7V to 4.5V
    g_unRegDrvConfConfig.structReg.iEN_PFD = 0;
    
    //Short to VS protection / CLK failsafe enable
    //0: Short to VS and overload protection disabled
    //1: Protection enabled. In addition, this bit enables protection against CLK fail, when using an external clock source.
    g_unRegDrvConfConfig.structReg.iEN_S2VS = 0;    
    
    iRegValue = g_unRegDrvConfConfig.iRegValue;
    
		printf("\r\n Set DRVCONF 0x%x", iRegValue);
    HAL_StatusTypeDef iStatus = TMC2590_SetReg (REG_DRVCONF, iRegValue);
    if(iStatus != HAL_OK)
    {
        *iErrorCode = iStatus;
        return false;
    }
    
    return true;
}

static bool TMC2590_SetTypicalDRVCTRL(HAL_StatusTypeDef *iErrorCode)
{
    *iErrorCode = HAL_OK;
    uint32_t iRegValue = 0x0000;
    
    g_unRegDrvCtrlConfig.iRegValue = 0;
    
    //Enable STEP interpolation
    //0: Disable STEP pulse interpolation.
    //1: Enable MicroPlyer STEP pulse multiplication by 16.
    g_unRegDrvCtrlConfig.structReg.iINTPOL = 0;
    
    //Enable double edge STEP pulses
    //0: Rising STEP pulse edge is active, falling edge is inactive.
    //1: Both rising and falling STEP pulse edges are active.
    g_unRegDrvCtrlConfig.structReg.iDEDGE = 0;
    
    //Microstep resolution for STEP/DIR mode
    //Microsteps per fullstep:
    //%0000: 256
    //%0001: 128
    //%0010: 64
    //%0011: 32
    //%0100: 16
    //%0101: 8
    //%0110: 4
    //%0111: 2 (halfstep)
    //%1000: 1 (fullstep)
    g_unRegDrvCtrlConfig.structReg.iMRES = MICROSTEP_16;
    
    iRegValue = g_unRegDrvCtrlConfig.iRegValue;
		
    printf("\r\n Set DRVCTRL 0x%x", iRegValue);		
    HAL_StatusTypeDef iStatus = TMC2590_SetReg (REG_DRVCTRL, iRegValue);  
    if(iStatus != HAL_OK)
    {
        *iErrorCode = iStatus;
        return false;
    }
    
    return true;
}

static bool TMC2590_SetTypicalSGCSCONF(HAL_StatusTypeDef *iErrorCode)
{
    *iErrorCode = HAL_OK;
    uint32_t iRegValue = 0x0000;
    
    g_unRegSgcsConfConfig.iRegValue = 0;
    
    //StallGuard2 filter enable
    //0: Standard mode, fastest response time.
    //1: Filtered mode, updated once for each four fullsteps to compensate for variation in motor construction, highest accuracy.
    g_unRegSgcsConfConfig.structReg.iSFILT = 1;
    
    //StallGuard2 threshold value
    g_unRegSgcsConfConfig.structReg.iSGT = 0;
    
    //Current scale(scales digital currents A and B)
    //Current scaling for SPI and STEP/DIR operation.
    //%00000 … %11111: 1/32, 2/32, 3/32, … 32/32
    g_unRegSgcsConfConfig.structReg.iCS = 20;
    
    iRegValue = g_unRegSgcsConfConfig.iRegValue;
	
    printf("\r\n Set SGCSCONF 0x%x", iRegValue);	
    HAL_StatusTypeDef iStatus = TMC2590_SetReg (REG_SGCSCONF, iRegValue);  
    if(iStatus != HAL_OK)
    {
        *iErrorCode = iStatus;
        return false;
    }
    
    return true;
}

//uint32_t TMC2590_ReadStatus(uint32_t iReadSelect)
//{
//    uint32_t iReadValue = 0x0000;
//    
//    g_unRegDrvConfConfig.structReg.iRDSEL = iReadSelect;
//    iReadValue = SPI_TMC2590_SendByte (REG_DRVCONF | g_unRegDrvConfConfig.iRegValue);
//    
//    return iReadValue;
//}

//static void TMC2590_Msg(uint8_t *strMsg)
//{
//    uint16_t i;
//    
//    for(i = 0; i < sizeof(strMsg); i++)
//    {
//        HAL_UART_Transmit (&Uart_Handle, strMsg, 1, 1000);
//    }
//}

bool TMC2590_Init (void)
{
    printf ("\r\n TMC2590 Start Init!");
    Delay_ms(200);
  
    uint32_t iRead = 0;
    /* Select the FLASH: Chip Select low */
//    SPI_TMC2590_CS_HIGH();
//		Delay_ms (5);
//    SPI_TMC2590_CS_LOW();
//		Delay_ms (5);
	
//    HAL_StatusTypeDef iStatus = SPI_TMC2590_SendByte (0x901b4, &iRead);  
//		iStatus = SPI_TMC2590_SendByte (0xd0010, &iRead);  
//		iStatus = SPI_TMC2590_SendByte (0xef020, &iRead);
//		iStatus = SPI_TMC2590_SendByte (0x00003, &iRead);  
//		iStatus = SPI_TMC2590_SendByte (0xa8202, &iRead);  

//	  HAL_StatusTypeDef iStatus = SPI_TMC2590_SendByte (0x00003, &iRead);  
//		printf ("\r\n s%d!", iStatus);
//		iStatus = SPI_TMC2590_SendByte (0x91935, &iRead);  
//		printf ("\r\n s%d!", iStatus);
//		iStatus = SPI_TMC2590_SendByte (0xA0000, &iRead);
//		printf ("\r\n s%d!", iStatus);
//		iStatus = SPI_TMC2590_SendByte (0xD0810, &iRead);  //StallGuard无需传感器情况下实现步进电机力矩控制, CoolStep无需传感器情况驱动器输出的电流随负载动态变化, 节省电能, 避免发热, 允许过载
//		printf ("\r\n s%d!", iStatus);
//		iStatus = SPI_TMC2590_SendByte (0xEF040, &iRead);  
//		printf ("\r\n s%d!", iStatus);
		HAL_GPIO_WritePin(TMC2590_ENN_GPIO_Port, TMC2590_ENN_Pin,GPIO_PIN_SET);
		
		#if TMC2590_STANDALONE
				HAL_GPIO_WritePin(GPIOB, TMC2590_CFG3_Pin, GPIO_PIN_SET);
				motor_step_value = 16;
				HAL_GPIO_WritePin(GPIOB, TMC2590_CFG2_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, TMC2590_CFG1_Pin, GPIO_PIN_RESET);				
		#else
			HAL_StatusTypeDef iStatus = SPI_TMC2590_SendByte (0x901b4, &iRead);  
			iStatus = SPI_TMC2590_SendByte (0xD000F, &iRead);  
			iStatus = SPI_TMC2590_SendByte (0xEF000, &iRead);
			iStatus = SPI_TMC2590_SendByte (0x00003, &iRead);
//			iStatus = SPI_TMC2590_SendByte (0x00004, &iRead);
			iStatus = SPI_TMC2590_SendByte (0xA8202, &iRead);  
		#endif
		Delay_ms(5);
//		HAL_GPIO_WritePin(TMC2590_ENN_GPIO_Port, TMC2590_ENN_Pin,GPIO_PIN_RESET);
		Delay_ms(5);
//		HAL_GPIO_WritePin(SIGNAL_LED_G_GPIO_Port, SIGNAL_LED_G_Pin,GPIO_PIN_RESET);
//		HAL_GPIO_WritePin(SIGNAL_LED_R_GPIO_Port, SIGNAL_LED_R_Pin,GPIO_PIN_SET);
//		HAL_GPIO_WritePin(SIGNAL_LED_B_GPIO_Port, SIGNAL_LED_B_Pin,GPIO_PIN_SET);
		Delay_ms(100);
		
		return true;
	
		
	
    HAL_StatusTypeDef iErrorCode;
  
    //设置Chopper Control Register
    if(!TMC2590_SetTypicalCHOPCONF(&iErrorCode))
    {
        printf("\r\n Set CHOPCONF Failed! Msg:%s", ErrorCodeToString(iErrorCode));
        return false;
    }   
    Delay_ms(1);  
	
		if(!TMC2590_SetTypicalSGCSCONF(&iErrorCode))
    {
        printf("\r\n Set SGCSCONF Failed! Msg:%s", ErrorCodeToString(iErrorCode));
        return false;
    }   
    Delay_ms(1);
		
    if(!TMC2590_SetTypicalDRVCONF(&iErrorCode))
    {
        printf("\r\n Set DRVCONF Failed! Msg:%s", ErrorCodeToString(iErrorCode));
        return false;
    }    
    Delay_ms(1);
    
    if(!TMC2590_SetTypicalDRVCTRL(&iErrorCode))
    {
        printf("\r\n Set DRVCTRL Failed! Msg:%s", ErrorCodeToString(iErrorCode));
        return false;
    }   
    Delay_ms(1);
    

    
    if(!TMC2590_SetTypicalSMARTEN(&iErrorCode))
    {
        printf("\r\n Set SMARTEN Failed! Msg:%s", ErrorCodeToString(iErrorCode));
        return false;
    }   
    Delay_ms(1);
    

    //设置CoolStep Control Register
    printf("\r\n TMC2590 Init Sucess");
    
//    SPI_TMC2590_Write (TORQUE_Register_ADDR, drv8711_torque_value);
//    Delay_ms(1);
//    SPI_TMC2590_Write (OFF_Register_ADDR, drv8711_off_value);
//    Delay_ms(1);
//    SPI_DRV8711_Write (BLANK_Register_ADDR, drv8711_blank_value);
//    Delay_ms(1);
//    SPI_DRV8711_Write (DECAY_Register_ADDR, drv8711_decay_value);
//    Delay_ms(1);
//    SPI_DRV8711_Write (STALL_Register_ADDR, drv8711_stall_value);
//    Delay_ms(1);
//    SPI_DRV8711_Write (DRIVE_Register_ADDR, drv8711_drive_value);
//    Delay_ms(1);
//    SPI_DRV8711_Write (STATUS_Register_ADDR, drv8711_status_value);
//
//    TMC2590_Segment_Set (motor_step_value); // 设置细分
//    Delay_ms(1);
//    Drv8711_ISGAIN_Set (drv_isgain);  // 设置增益
//    Delay_ms(1);
//    Drv8711_TORQUE_Set (0); // 设置力矩
//    Delay_ms(1);
//    //Drv8711_TORQUE_Set(drv_torque_value); // 设置力矩
//    //Delay_ms(1);
//    Drv8711_Reg_Read(); // 写入的数据在都出来
    //STEPMOTOR_OUTPUT_DISABLE();
 //   STEPMOTOR_OUTPUT_ENABLE();//这里设置输出使能才能使能力矩
}
