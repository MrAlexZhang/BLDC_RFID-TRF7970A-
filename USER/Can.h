
#ifndef __CAN_H
#define __CAN_H


#define PROGRAMME_TYPENUM 2 //3-桌面传输板

#define 	   GET_CURRENT_VERSION      0x01
#define 	   SAVE_BOARDID             0x02
#define 	   GET_BOARDID              0x03
#define 	    Test_IRF                  0x04
#define 	    Read_E2PROM              0x05
#define 	    Erace_E2PROM              0x06
#define 	    Write_E2PROM              0x07

#define 	    Respond_set              0x08

#define 	    SEND_RFID_TAG_MESSAGES_H             0x0a  //回传标签低4字节信息
#define 	    SEND_RFID_TAG_MESSAGES_L    0x0b  ////回传标签高4字节信息
#define SET_RFID_TAG_REPLY_DELAY_FUNCTION 0x0c //设置rfid 延时保护功能
#define DEMAND_RFID_TAG_REPLY_DELAY_FUNCTION 0x0d//查询RFID 延时保护功能
#define DEMAND_RFID_REGISITER_DATA 0x0e
#define READ_DATA_FROM_RFID_MEMORY 0x0f




/////////////STEPPING MOTOR COMMAND  ///////////
#define 	    SET_STEPPING_MOTOR_WORK_PPS      0x10
#define         DEMAND_STEPPING_MOTOR_WORK_PPS      0x11
#define         SET_STEPPING_MOTOR_MAX_PPS   0x12
#define         DEMAND_STEPPING_MOTOR_MAX_PPS  0x13
#define         SET_STEPPING_MOTOR_MIN_PPS         0x14
#define         DEMAND_STEPPING_MOTOR_MIN_PPS         0x15
#define         SET_STEPPING_MOTOR_ACC_PPS    0x16
#define         DEMAND_STEPPING_MOTOR_ACC_PPS       0x17
#define SET_STEPPING_MOTOR_ACC_TIME_INTERVAL 0x18
#define DEMAND_STEPPING_MOTOR_ACC_TIME_INTERVAL 0x19
#define SET_STEPPING_MOTOR_ENABLE_ACC_FUNCTION 0x1a
#define DEMAND_STEPPING_MOTOR_ENABLE_ACC_FUNCTION 0x1b
#define         SEND_STEPPING_Motor_CTRL           0x1c
#define SEND_STEPPING_MOTOR_FAULT_CODE 0x1d
#define SET_STEPPING_MOTOR_START_PPS 0x1e
#define DEMAND_STEPPING_MOTOR_START_PPS 0x1f

//////////////////////BLDC COMMAND///////////////////
#define 	SET_BLDC_WORK_ROTOR_TARGET_SPEED      0x20
#define         DEMAND_BLDC_WORK_ROTOR_TARGET_SPEED      0x21
#define         DEMAND_BLDC_MEASURE_ROTOR_SPEED   0x22
//#define         DEMAND_BLDC_START_PWM  0x23
#define         SEND_BLDC_CTRL         0x24
#define         SEND_BLDC_FAULT_CODE         0x25
#define SET_BLDC_WORK_PWM_DUTY 0x26
#define DEMAND_BLDC_WORK_PWM_DUTY 0x27
#define SET_BLDC_PID_KP 0x28
#define DEMAND_BLDC_PID_KP 0x29
#define SET_BLDC_PID_KI 0x2A
#define DEMAND_BLDC_PID_KI 0x2B
#define SET_BLDC_PID_KD 0X2C
#define DEMAND_BLDC_PID_KD 0X2D




#define OUTPUT_CTROL 0x30
#define INPUT_SIGNAL_MESSAGE 0x31





#define        UPdata_perpare	          0xE0
#define        Send_PACKET_MSG            0xE1
#define        Send_programme_frame       0xE2
#define        Send_programme_PACKETnum   0xE3

#define        Board_restart              0xE5


enum{non_error,medicine_lenghterror,medicine_outputblock,motor_error,medicine_inputerror,medicineblind,overkeepnum,modeerror};


typedef struct FRME_HEAD
{ 
  
  u8 data_length        :3;
  u8 SUBgroove_ID 	  :2;
  u8 reserve      	  :2;
  u8 Frme_direction  	  :1;		
  
  
}
stSetFRME_HEAD;


typedef union tagSetmsgheadInfo_val
{
  u8 VAL;
  stSetFRME_HEAD  FRME_HEAD;
}
stSetmsgheadInfo_val;


typedef struct MESSAGE_FRM
{   stSetFRME_HEAD  FRME_HEAD;
u8              Command_type;
u8              data[6];
} set_msg,set_msgout;



typedef union tagSetbeltInfo_val
{
  u8 VAL; 	
  struct tagSetbeltcontral
  {
    u8 run          :1;
    u8 direction    :1;
    u8 contraltype  :2;
    u8 rev          :4;
  }
  stsetbeltcontral; 
  
}
stSetbeltInfo_val;







#ifdef _Can_VER


set_msg msg={0};
set_msgout msgout={0};

stSetmsgheadInfo_val  msgheadInfo_val={0};
stSetbeltInfo_val SetbeltInfo_val={0};  



u8 Respond_flag=1;

u16 mcu_run_indicate_cnt = 20;

bool BLDC_RUN_STATE_FLAG = FALSE;

void SPM_MESSAGE_OUT(u8 dir,u8 len,u8 type,u8 sub_id,u8 *p);

void PC_bug_job(void);

void CAN_Command_exectue(void);

CAN_TxStatus_TypeDef CAN_Transmit_Conf(u8 *buf, u8 cnt);

CAN_InitStatus_TypeDef CAN_Init_Conf(void);

#else
extern set_msg msg;
extern set_msgout msgout;

extern stSetmsgheadInfo_val  msgheadInfo_val;

extern stSetbeltInfo_val SetbeltInfo_val; 


extern u8 Respond_flag;

extern u16 mcu_run_indicate_cnt;

extern bool BLDC_RUN_STATE_FLAG;


extern void SPM_MESSAGE_OUT(u8 dir,u8 len,u8 type,u8 sub_id,u8 *p);

extern void PC_bug_job(void);

extern void CAN_Command_exectue(void);

extern CAN_InitStatus_TypeDef CAN_Init_Conf(void);

#endif




















#endif
