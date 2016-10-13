
#define _Can_VER


/* 包含自定义头文件 */
//#include "stm8s.h"
#include "Globe.h"

extern u08_t buf[300];

volatile u8 id_offset;
u8 mess[14];
u8	byte_counter;
u8 leds;
u16 tout;
/* Private constants ---------------------------------------------------------*/


void CAN_Command_exectue(void)
{
  u8 temp[20]={0};
  u16 dat = 0;
  
  switch (msg.Command_type)
  {
    
  case GET_CURRENT_VERSION:   // 01 获取版本号
    temp[0]=hardwaremainversion;   
    temp[1]=hardwaresubversion;    
    temp[2]=softwaremainversion;
    temp[3]=softwaresubversion;
    
    SPM_MESSAGE_OUT(1,4,GET_CURRENT_VERSION,0,temp);
    break;
    
  case SAVE_BOARDID://set board id
    EEPROM_INIT();
    
    FLASH_ProgramByte( boardid_address0, msg.data[0]);
    FLASH_ProgramByte( boardid_address1, msg.data[1]);
    FLASH_ProgramByte( boardid_address2, msg.data[2]);
    FLASH_ProgramByte( boardid_address3, msg.data[3]);
    FLASH_Lock(FLASH_MEMTYPE_DATA);
    
    temp[0] = 0x01;
    SPM_MESSAGE_OUT(1,4,SAVE_BOARDID,0,temp);
    break;
    
  case GET_BOARDID:          //0x03 获取板子ID
    EEPROM_INIT();
    
    temp[0]=FLASH_ReadByte(boardid_address0);   
    temp[1]=FLASH_ReadByte(boardid_address1);    
    temp[2]=FLASH_ReadByte(boardid_address2);
    temp[3]=FLASH_ReadByte(boardid_address3);
    FLASH_Lock(FLASH_MEMTYPE_DATA);
    
    SPM_MESSAGE_OUT(1,4,GET_BOARDID,0,temp);
    break;
    
  case Respond_set://set respond_flag
    Respond_flag = msg.data[0];
    
    temp[0] = 0x01;
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,1,Respond_set,0,temp);
    break; 
    
  case SET_RFID_TAG_REPLY_DELAY_FUNCTION:
    rfid_delay_protect.RFID_DELAY_PROTECT_FLAG = msg.data[0];
    dat = msg.data[1];
    dat <<= 8;
    dat |= msg.data[2];
    rfid_delay_protect.RFID_DELAY_threshold = dat;
    
    EEPROM_INIT();
    FLASH_ProgramByte(RFID_DELAY_EN_EEPROM_ADDR,msg.data[0]);
    FLASH_ProgramByte(RFID_DELAY_EEPROM_ADDR1,msg.data[1]);
    FLASH_ProgramByte(RFID_DELAY_EEPROM_ADDR2,msg.data[2]);
    FLASH_Lock(FLASH_MEMTYPE_DATA);
    
    temp[0] = 0x01;
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,1,SET_RFID_TAG_REPLY_DELAY_FUNCTION,0,temp);
    break;
    
  case DEMAND_RFID_TAG_REPLY_DELAY_FUNCTION:
    EEPROM_INIT();
    temp[0] = FLASH_ReadByte(RFID_DELAY_EN_EEPROM_ADDR);
    temp[1] = FLASH_ReadByte(RFID_DELAY_EEPROM_ADDR1);
    temp[2] = FLASH_ReadByte(RFID_DELAY_EEPROM_ADDR2);
    FLASH_Lock(FLASH_MEMTYPE_DATA);
    
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,3,DEMAND_RFID_TAG_REPLY_DELAY_FUNCTION,0,temp);
    break;
    
  case DEMAND_RFID_REGISITER_DATA:
    temp[0]= msg.data[0];
    
    Trf797xReadSingle(&temp[0], 1);
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,1,DEMAND_RFID_REGISITER_DATA,0,temp);
    break;
    
  case READ_DATA_FROM_RFID_MEMORY:
    buf[5]=0x02;//msg.data[0];
      buf[6] = 0x20;//msg.data[1];
      buf[7] = 0x01;
      
      HostRequestCommand(&buf[0],3,0,0);
    
    temp[0] = 1;
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,1,READ_DATA_FROM_RFID_MEMORY,0,temp);
      break;
    
  case SET_STEPPING_MOTOR_WORK_PPS://set step motor work pps
    EEPROM_INIT();
    
    FLASH_ProgramByte( STEPPING_Motor_PPS_address0, msg.data[0]);
    FLASH_ProgramByte( STEPPING_Motor_PPS_address1, msg.data[1]);
    FLASH_Lock(FLASH_MEMTYPE_DATA);
    
    dat = msg.data[0];
    dat <<= 8;
    dat |= msg.data[1];
    
    if(dat > step_motor_start_up_parameter.step_max_pps)
      step_motor_start_up_parameter.step_work_pps = step_motor_start_up_parameter.step_max_pps;
    else if(dat < step_motor_start_up_parameter.step_min_pps)
      step_motor_start_up_parameter.step_work_pps = step_motor_start_up_parameter.step_min_pps;
    else step_motor_start_up_parameter.step_work_pps =dat;
    
    temp[0] = 0x01;
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,1,SET_STEPPING_MOTOR_WORK_PPS,0,temp);
    break;
    
  case DEMAND_STEPPING_MOTOR_WORK_PPS://demand step motor work pps
    EEPROM_INIT();
    
    temp[0]=FLASH_ReadByte( STEPPING_Motor_PPS_address0);
    temp[1]=FLASH_ReadByte( STEPPING_Motor_PPS_address1);
    FLASH_Lock(FLASH_MEMTYPE_DATA);
    
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,2,DEMAND_STEPPING_MOTOR_WORK_PPS,0,temp);
    break;
    
  case SET_STEPPING_MOTOR_MAX_PPS://set stepping motor max pps
    EEPROM_INIT();
    
    FLASH_ProgramByte( STEPPING_motor_max_pps_address0, msg.data[0]);
    FLASH_ProgramByte( STEPPING_motor_max_pps_address1, msg.data[1]);
    FLASH_Lock(FLASH_MEMTYPE_DATA);
    
    dat = msg.data[0];
    dat <<= 8;
    dat |= msg.data[1];
    step_motor_start_up_parameter.step_max_pps =dat;
    
    temp[0] = 0x01;
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,1,SET_STEPPING_MOTOR_MAX_PPS,0,temp);
    break;
    
  case DEMAND_STEPPING_MOTOR_MAX_PPS:
    EEPROM_INIT();
    
    temp[0]=FLASH_ReadByte( STEPPING_motor_max_pps_address0);
    temp[1]=FLASH_ReadByte( STEPPING_motor_max_pps_address1);
    FLASH_Lock(FLASH_MEMTYPE_DATA);
    
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,2,DEMAND_STEPPING_MOTOR_MAX_PPS,0,temp);
    break;
    
  case SET_STEPPING_MOTOR_MIN_PPS:
    EEPROM_INIT();
    
    FLASH_ProgramByte( STEPPING_motor_min_pps_address0, msg.data[0]);
    FLASH_ProgramByte( STEPPING_motor_min_pps_address1, msg.data[1]);
    FLASH_Lock(FLASH_MEMTYPE_DATA);
    
    dat = msg.data[0];
    dat <<= 8;
    dat |= msg.data[1];
    step_motor_start_up_parameter.step_min_pps =dat;
    
    temp[0] = 0x01;
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,1,SET_STEPPING_MOTOR_MIN_PPS,0,temp);
    break;
    
  case DEMAND_STEPPING_MOTOR_MIN_PPS:
    EEPROM_INIT();
    
    temp[0]=FLASH_ReadByte( STEPPING_motor_min_pps_address0);
    temp[1]=FLASH_ReadByte( STEPPING_motor_min_pps_address1);
    FLASH_Lock(FLASH_MEMTYPE_DATA);
    
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,2,DEMAND_STEPPING_MOTOR_MIN_PPS,0,temp);
    break;
    
  case SET_STEPPING_MOTOR_ACC_PPS:
    EEPROM_INIT();
    
    FLASH_ProgramByte( STEPPING_motor_pps_acc_address0, msg.data[0]);
    FLASH_ProgramByte( STEPPING_motor_pps_acc_address1, msg.data[1]);
    FLASH_Lock(FLASH_MEMTYPE_DATA);
    
    dat = msg.data[0];
    dat <<= 8;
    dat |= msg.data[1];
    step_motor_start_up_parameter.step_acc_value =dat;
    
    temp[0] = 0x01;
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,1,SET_STEPPING_MOTOR_ACC_PPS,0,temp);
    break;
    
  case DEMAND_STEPPING_MOTOR_ACC_PPS:
    EEPROM_INIT();
    
    temp[0]=FLASH_ReadByte( STEPPING_motor_pps_acc_address0);
    temp[1]=FLASH_ReadByte( STEPPING_motor_pps_acc_address1);
    FLASH_Lock(FLASH_MEMTYPE_DATA);
    
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,2,DEMAND_STEPPING_MOTOR_ACC_PPS,0,temp);
    break;
    
  case SET_STEPPING_MOTOR_ACC_TIME_INTERVAL:
    EEPROM_INIT();
    
    FLASH_ProgramByte( STEPPING_motor_acc_time_interval0, msg.data[0]);
    FLASH_ProgramByte( STEPPING_motor_acc_time_interval1, msg.data[1]);
    FLASH_Lock(FLASH_MEMTYPE_DATA);
    
    dat = msg.data[0];
    dat <<= 8;
    dat |= msg.data[1];
    step_motor_start_up_parameter.step_time_interval =dat;
    
    temp[0] = 0x01;
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,1,SET_STEPPING_MOTOR_ACC_TIME_INTERVAL,0,temp);
    break;
    
    
  case DEMAND_STEPPING_MOTOR_ACC_TIME_INTERVAL:
    EEPROM_INIT();
    
    temp[0]=FLASH_ReadByte( STEPPING_motor_acc_time_interval0);
    temp[1]=FLASH_ReadByte( STEPPING_motor_acc_time_interval1);
    FLASH_Lock(FLASH_MEMTYPE_DATA);
    
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,2,DEMAND_STEPPING_MOTOR_ACC_TIME_INTERVAL,0,temp);
    break;   
    
  case SET_STEPPING_MOTOR_ENABLE_ACC_FUNCTION:
    EEPROM_INIT();
    
    FLASH_ProgramByte( STEPPING_motor_acc_function_address, msg.data[0]);

    FLASH_Lock(FLASH_MEMTYPE_DATA);
    
    step_motor_start_up_parameter.step_motor_enable_acc_flag = msg.data[0];
    
    temp[0] = 0x01;
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,1,SET_STEPPING_MOTOR_ENABLE_ACC_FUNCTION,0,temp);
    break;
    
  case DEMAND_STEPPING_MOTOR_ENABLE_ACC_FUNCTION:
    EEPROM_INIT();
    
    temp[0]=FLASH_ReadByte( STEPPING_motor_acc_function_address);
    FLASH_Lock(FLASH_MEMTYPE_DATA);
    
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,1,DEMAND_STEPPING_MOTOR_ENABLE_ACC_FUNCTION,0,temp);
    break;
    
  case SEND_STEPPING_Motor_CTRL:
    SetbeltInfo_val.VAL=msg.data[0];
    
    step_motor_start_up_parameter.step_motor_total_num=_Merge_32Bit(msg.data[1],msg.data[2],msg.data[3],msg.data[4]);
        
    temp[0] = 0x01;
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,1,SEND_STEPPING_Motor_CTRL,0,temp); 
    
    if(SetbeltInfo_val.stsetbeltcontral.run)
    {
      motor_move_ctrol(ON);
    }
    else
    {
      motor_move_ctrol(OFF);      
    }
    break; 
    
  case SET_STEPPING_MOTOR_START_PPS:
    EEPROM_INIT();
    
    FLASH_ProgramByte( STEPPING_motor_pwm_start_pps_address0, msg.data[0]);
    FLASH_ProgramByte( STEPPING_motor_pwm_start_pps_address1, msg.data[1]);
    FLASH_Lock(FLASH_MEMTYPE_DATA);
    
    dat = msg.data[0];
    dat <<= 8;
    dat |= msg.data[1];
    
    step_motor_start_up_parameter.step_motor_start_pps = dat;
    
    temp[0] = 0x01;
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,1,SET_STEPPING_MOTOR_START_PPS,0,temp);   
    break; 
    
  case DEMAND_STEPPING_MOTOR_START_PPS:
    EEPROM_INIT();
    
    temp[0]=FLASH_ReadByte( STEPPING_motor_pwm_start_pps_address0);
    temp[1]=FLASH_ReadByte( STEPPING_motor_pwm_start_pps_address1);

    FLASH_Lock(FLASH_MEMTYPE_DATA);

    if(Respond_flag)
      SPM_MESSAGE_OUT(1,2,DEMAND_STEPPING_MOTOR_START_PPS,0,temp);   
    break;
    
  case SET_BLDC_WORK_ROTOR_TARGET_SPEED:
    EEPROM_INIT();
    
    dat = msg.data[0];
    dat <<=8;
    dat |= msg.data[1];
    
    FLASH_ProgramByte( BLDC_Motor_WORK_ROTOR_TARGET_SPEED_address1, msg.data[0]);
    FLASH_ProgramByte( BLDC_Motor_WORK_ROTOR_TARGET_SPEED_address2, msg.data[1]);
    FLASH_Lock(FLASH_MEMTYPE_DATA);
    
    BLDC_Set_Target_rotor_speed(dat);
    
    temp[0] = 0x01;
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,1,SET_BLDC_WORK_ROTOR_TARGET_SPEED,0,temp);    
    break;
    
  case DEMAND_BLDC_WORK_ROTOR_TARGET_SPEED:

      EEPROM_INIT();
      
      temp[0]=FLASH_ReadByte( BLDC_Motor_WORK_ROTOR_TARGET_SPEED_address1);
      temp[1]=FLASH_ReadByte( BLDC_Motor_WORK_ROTOR_TARGET_SPEED_address2);
      
      FLASH_Lock(FLASH_MEMTYPE_DATA);
    
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,2,DEMAND_BLDC_WORK_ROTOR_TARGET_SPEED,0,temp);   
    break;
    
  case DEMAND_BLDC_MEASURE_ROTOR_SPEED:
    dat = BLDC_Get_Measured_rotor_speed();
    temp[0] = (dat >>8);
    temp[1] = dat;

    if(Respond_flag)
      SPM_MESSAGE_OUT(1,2,DEMAND_BLDC_MEASURE_ROTOR_SPEED,0,temp);    
    break;
    
  case SEND_BLDC_CTRL:

//    MECHARM_Cylinderctrl(msg.data[0],msg.data[1]);
    
    if(msg.data[0] == ON)
    {
      cylindermotorstatus.cylindermotorRUN = TRUE;
      cylindermotorstatus.cylindermotordirect = msg.data[1];

      BLDC_RUN_STATE_FLAG = TRUE;
      mcu_run_indicate_cnt = 30000;
      LED_OPTION_PROCESS(LED_7,ON);//bldc run indicate led
      
      tasknum=task_bldc;
    }
    else
    {
      cylindermotorstatus.cylindermotorRUN = FALSE;
      cylindermotorstatus.cylindermotordirect = 0;
      
      BLDC_RUN_STATE_FLAG = FALSE;
      mcu_run_indicate_cnt = 20;
      LED_OPTION_PROCESS(LED_7,OFF);//bldc run indicate led
    }
    
    temp[0] = 0x01;
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,1,SEND_BLDC_CTRL,0,temp);
    break;
    
  case SET_BLDC_WORK_PWM_DUTY:
    EEPROM_INIT();   
    FLASH_ProgramByte( BLDC_MOTOR_WORK_PWM_DUTY_ADDRESS, msg.data[0]);
    FLASH_Lock(FLASH_MEMTYPE_DATA);

    BLDC_Set_Duty_cycle(msg.data[0]);
    
    temp[0] = 0x01;
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,1,SET_BLDC_WORK_PWM_DUTY,0,temp);    
    break;
    
  case DEMAND_BLDC_WORK_PWM_DUTY:
    if(tasknum == task_bldc)
      temp[0] = BLDC_Get_Duty_cycle();
    else 
    {
      EEPROM_INIT();
      temp[0]=FLASH_ReadByte( BLDC_MOTOR_WORK_PWM_DUTY_ADDRESS);
      FLASH_Lock(FLASH_MEMTYPE_DATA);
    }
    
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,1,DEMAND_BLDC_WORK_PWM_DUTY,0,temp);   
    break;
    
  case SET_BLDC_PID_KP:
    dat = msg.data[0];
    dat <<=8;
    dat |= msg.data[1];
    
    EEPROM_INIT();   
    FLASH_ProgramByte( BLDC_MOTOR_PID_KP_ADDRESS1, msg.data[0]);
    FLASH_ProgramByte( BLDC_MOTOR_PID_KP_ADDRESS2, msg.data[1]);
    FLASH_Lock(FLASH_MEMTYPE_DATA);
    
    BLDC_Set_Speed_KP(dat);
    
    temp[0] = 0x01;
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,1,SET_BLDC_PID_KP,0,temp);
    break;
    
  case DEMAND_BLDC_PID_KP:
    if(tasknum == task_bldc)
    {
      dat = BLDC_Get_Speed_KP();
      
      temp[0] = (dat >>8);
      temp[1] = dat;
    }
    else 
    {
      EEPROM_INIT();
      temp[0]=FLASH_ReadByte( BLDC_MOTOR_PID_KP_ADDRESS1);
      temp[1]=FLASH_ReadByte( BLDC_MOTOR_PID_KP_ADDRESS2);
      FLASH_Lock(FLASH_MEMTYPE_DATA);
    }
    
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,2,DEMAND_BLDC_PID_KP,0,temp);
    break;
    
  case SET_BLDC_PID_KI:
    dat = msg.data[0];
    dat <<=8;
    dat |= msg.data[1];
    
    EEPROM_INIT();   
    FLASH_ProgramByte( BLDC_MOTOR_PID_KI_ADDRESS1, msg.data[0]);
    FLASH_ProgramByte( BLDC_MOTOR_PID_KI_ADDRESS2, msg.data[1]);
    FLASH_Lock(FLASH_MEMTYPE_DATA);
    
    BLDC_Set_Speed_KI(dat);
    
    temp[0] = 0x01;
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,1,SET_BLDC_PID_KI,0,temp);
    break;
    
  case DEMAND_BLDC_PID_KI:
    if(tasknum == task_bldc)
    {
      dat = BLDC_Get_Speed_KI();
      
      temp[0] = (dat >>8);
      temp[1] = dat;
    }
    else
    {
      EEPROM_INIT();
      temp[0]=FLASH_ReadByte( BLDC_MOTOR_PID_KI_ADDRESS1);
      temp[1]=FLASH_ReadByte( BLDC_MOTOR_PID_KI_ADDRESS2);
      FLASH_Lock(FLASH_MEMTYPE_DATA);
    }
    
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,2,DEMAND_BLDC_PID_KI,0,temp);
    break;
    
  case SET_BLDC_PID_KD:
    dat = msg.data[0];
    dat <<=8;
    dat |= msg.data[1];
    
    EEPROM_INIT();   
    FLASH_ProgramByte( BLDC_MOTOR_PID_KD_ADDRESS1, msg.data[0]);
    FLASH_ProgramByte( BLDC_MOTOR_PID_KD_ADDRESS2, msg.data[1]);
    FLASH_Lock(FLASH_MEMTYPE_DATA);
    
    BLDC_Set_Speed_KD(dat);
    
    temp[0] = 0x01;
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,1,SET_BLDC_PID_KD,0,temp);
    break;
    
  case DEMAND_BLDC_PID_KD:
    if(tasknum == task_bldc)
    {
      dat = BLDC_Get_Speed_KD();
      
      temp[0] = (dat >>8);
      temp[1] = dat;
    }
    else
    {
      EEPROM_INIT();
      temp[0]=FLASH_ReadByte( BLDC_MOTOR_PID_KD_ADDRESS1);
      temp[1]=FLASH_ReadByte( BLDC_MOTOR_PID_KD_ADDRESS2);
      FLASH_Lock(FLASH_MEMTYPE_DATA);
    }
    
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,2,DEMAND_BLDC_PID_KD,0,temp);
    break;
    
  case OUTPUT_CTROL:
    output_port_opteration(msg.data[0],msg.data[1]);
    
    temp[0] = 0x01;
    if(Respond_flag)
      SPM_MESSAGE_OUT(1,1,OUTPUT_CTROL,0,temp);
    break;
    
  case UPdata_perpare:
    if( msg.data[0] == PROGRAMME_TYPENUM)
    EEPROM_INIT();
    FLASH_ProgramByte(eeprom_updata_order_adress , 0X0A);
    FLASH_ProgramByte(aap_jump_iap_flag_adress , 0X0E);
    PC_bug_job();
    break; 
    
  case  Board_restart:									  //  上位机下载过程中中止
    
    PC_bug_job();
    break;
    
  default:break;
  }
}



void PC_bug_job(void)
{
  
  IWDG->KR = IWDG_KEY_REFRESH;
  
  asm("LDW X,  SP ");
  asm("LD  A,  $FF");
  asm("LD  XL, A  ");
  asm("LDW SP, X  "); 
  disableInterrupts(); 
  asm("JPF $8000");//全部重启
}

void jump_bootload(void)
{
  
  EEPROM_INIT();
  FLASH_ProgramByte(eeprom_updata_order_adress , 0X0A);
  FLASH_ProgramByte(aap_jump_iap_flag_adress , 0X0E);
  
  disableInterrupts(); 
  asm("JPF $8000");
  
}





void SPM_MESSAGE_OUT(u8 dir,u8 len,u8 type,u8 sub_id,u8 *p)
{
  
  msgout.FRME_HEAD.Frme_direction=dir;
  msgout.FRME_HEAD.data_length=len;
  msgout.Command_type=type;
  msgout.FRME_HEAD.SUBgroove_ID=sub_id;
  
  for(u8 i=0;i<len;i++)
    msgout.data[i]=*p++;
  
  CAN_Transmit_Conf((u8*)(&msgout),msgout.FRME_HEAD.data_length+2);
}





/*******************************************************************************
* 名称: CAN_Init_Conf
* 功能: CAN初始化操作
* 形参: 无
* 返回: CAN_InitStatus_TypeDef 初始化状态值 
*		 0 - 失败 1 - 成功
* 说明: 无 
******************************************************************************/
CAN_InitStatus_TypeDef CAN_Init_Conf(void)
{
  CAN_InitStatus_TypeDef status = CAN_InitStatus_Failed;
  
  /* Filter Parameters */
  CAN_FilterNumber_TypeDef CAN_FilterNumber;
  FunctionalState CAN_FilterActivation;
  CAN_FilterMode_TypeDef CAN_FilterMode;
  CAN_FilterScale_TypeDef CAN_FilterScale;
  
  uint8_t CAN_FilterID1;
  uint8_t CAN_FilterID2;
  uint8_t CAN_FilterID3;
  uint8_t CAN_FilterID4;
  uint8_t CAN_FilterIDMask1;
  uint8_t CAN_FilterIDMask2;
  uint8_t CAN_FilterIDMask3;
  uint8_t CAN_FilterIDMask4; 
  
  /* Init Parameters*/
  CAN_MasterCtrl_TypeDef CAN_MasterCtrl;
  CAN_Mode_TypeDef CAN_Mode;
  CAN_SynJumpWidth_TypeDef CAN_SynJumpWidth;
  CAN_BitSeg1_TypeDef CAN_BitSeg1;
  CAN_BitSeg2_TypeDef CAN_BitSeg2;
  uint8_t CAN_Prescaler;  
  
  /* CAN register init */
  CAN_DeInit();
  
  /* CAN  init */
  CAN_MasterCtrl=CAN_MasterCtrl_AllDisabled;
  CAN_Mode = CAN_Mode_Normal;//CAN_Mode_LoopBack;
  CAN_SynJumpWidth = CAN_SynJumpWidth_1TimeQuantum;
  CAN_BitSeg1 = CAN_BitSeg1_4TimeQuantum;
  CAN_BitSeg2 = CAN_BitSeg2_5TimeQuantum;
  CAN_Prescaler = 16;
  status = CAN_Init(CAN_MasterCtrl, CAN_Mode, CAN_SynJumpWidth, CAN_BitSeg1, \
    CAN_BitSeg2, CAN_Prescaler);
  
  /* CAN filter init */
  CAN_FilterNumber = CAN_FilterNumber_0;
  CAN_FilterActivation = ENABLE;
  CAN_FilterMode = CAN_FilterMode_IdList;
  CAN_FilterScale = CAN_FilterScale_32Bit;
  CAN_FilterID1=boardID>>21;  
  CAN_FilterID2=(boardID>>15&0x07)|(0x01<<3)|(boardID>>13&0xe0);
  CAN_FilterID3=boardID>>7;
  CAN_FilterID4=boardID<<1;
  CAN_FilterIDMask1=0;
  CAN_FilterIDMask2=0x01<<3;
  CAN_FilterIDMask3=0;
  CAN_FilterIDMask4=0;	
  CAN_FilterInit(CAN_FilterNumber, CAN_FilterActivation, CAN_FilterMode, 
                 CAN_FilterScale,CAN_FilterID1, CAN_FilterID2, CAN_FilterID3,
                 CAN_FilterID4,CAN_FilterIDMask1, CAN_FilterIDMask2, 
                 CAN_FilterIDMask3, CAN_FilterIDMask4);
  
  /* Enable Fifo message pending interrupt*/
  /* Message reception is done by CAN_RX ISR*/
  CAN_ITConfig(CAN_IT_FMP, ENABLE);
  return status;
  
}

/*******************************************************************************
* 名称: CAN_Transmit_Conf
* 功能: CAN发送设置函数
* 形参: 无
* 返回: CAN_TxStatus_TypeDef 初始化状态值 
*		 0 - 失败 1 - 成功
* 说明: 无 
******************************************************************************/
CAN_TxStatus_TypeDef CAN_Transmit_Conf(u8 *buf, u8 cnt)
{
  CAN_TxStatus_TypeDef status = CAN_TxStatus_Failed;
  CAN_Id_TypeDef Tx_IDE = CAN_Id_Extended;
  CAN_RTR_TypeDef Tx_RTR = CAN_RTR_Data;
  uint8_t Tx_DLC = 0;
  uint32_t Tx_Id = 0;
  Tx_Id = boardID;   			//设置标准识符(标准11位)扩展29位
  
  Tx_RTR = CAN_RTR_Data;		//传输消息为数据帧
  Tx_DLC = cnt;				//发2帧数据，每帧为8位
  
  status = CAN_Transmit(Tx_Id, Tx_IDE, Tx_RTR, Tx_DLC, buf);
  return status;
}


#pragma vector=0x0a
__interrupt void CAN_Rx_Interrupt(void)
{
  
  u8 cnt;	   
  
  u8  length;
  
  CAN_Receive();
  length=CAN_GetReceivedDLC();
  msgheadInfo_val.VAL=CAN_GetReceivedData(0);
  if(msgheadInfo_val.FRME_HEAD.Frme_direction==0)
  {
    CAN_Command_flag=1; //CAN_Command_exectue(msg);
    msg.Command_type=CAN_GetReceivedData(1);
    
    for (cnt=2; cnt < length; cnt++) 
    {
      msg.data[cnt-2] = CAN_GetReceivedData(cnt);
    }
    CAN_FIFORelease(); 
    nop();
  }
  else
  {
    msgheadInfo_val.VAL=0;
    CAN_Command_flag=0;
  }
}



