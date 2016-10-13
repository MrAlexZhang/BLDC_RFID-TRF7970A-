
#ifndef __SystemInit_H
#define __SystemInit_H

#define hardwaremainversion  1
#define hardwaresubversion 0
#define softwaremainversion 0
#define softwaresubversion 0x20



///////////////////////////board id parameter eeprom address ////////////////////////
#define boardid_address0            0x4010
#define boardid_address1            0x4011
#define boardid_address2            0x4012
#define boardid_address3            0x4013


/////////////////////rfid eeprom address ////////////////////////
#define RFID_DELAY_EN_EEPROM_ADDR   0x4020  //RFID 延时保护功能使能标志位地址
#define RFID_DELAY_EEPROM_ADDR1 0x4021  //RFID 延时保护功能 延时参数高字节
#define RFID_DELAY_EEPROM_ADDR2 0x4022  //RFID 延时保护功能 延时参数低字节


/////////////////////////STEPPING MOTOR CONFIG EEPROM ADDRESS ////////////////////
#define STEPPING_Motor_PPS_address0               0x4030   //MOTOR PPS HIGH BYTE
#define STEPPING_Motor_PPS_address1               0x4031  //MOTOR PPS LOW BYTE

#define STEPPING_motor_max_pps_address0 0x4032 //motor max pps high byte
#define STEPPING_motor_max_pps_address1 0x4033 //motor max pps low byte

#define STEPPING_motor_min_pps_address0 0x4034 //motor min pps high byte
#define STEPPING_motor_min_pps_address1 0x4035 //motor min pps low byte

#define STEPPING_motor_pps_acc_address0 0x4036 //motor pps acc high byte
#define STEPPING_motor_pps_acc_address1 0x4037 //motor pps acc low byte

#define STEPPING_motor_acc_time_interval0 0x4038 //motor acc time interval high byte
#define STEPPING_motor_acc_time_interval1 0x4039 //motor acc time interval low byte

#define STEPPING_motor_acc_function_address 0x403a//motor acc function enable or disable

#define STEPPING_motor_pwm_start_pps_address0 0x403b//motor start pps  high byte
#define STEPPING_motor_pwm_start_pps_address1 0x403c//motor start pps  low byte


/////////////////////////BLDC MOTOR CONFIG EEPROM ADDRESS ////////////////////
#define BLDC_Motor_WORK_ROTOR_TARGET_SPEED_address1               0x4060   //BLDC WORK ROTOR TARGET SPEED HIGH BYTES
#define BLDC_Motor_WORK_ROTOR_TARGET_SPEED_address2               0x4061   //BLDC WORK ROTOR TARGET SPEED HIGH BYTES

#define BLDC_MOTOR_WORK_PWM_DUTY_ADDRESS 0X4062 //BLDC WORK PWM DUTY 

#define BLDC_MOTOR_PID_KP_ADDRESS1 0X4063 //BLDC PID KP HIGH BYTES
#define BLDC_MOTOR_PID_KP_ADDRESS2 0X4064 //BLDC PID KP HIGH BYTES

#define BLDC_MOTOR_PID_KI_ADDRESS1 0X4065 //BLDC PID KI HIGH BYTES
#define BLDC_MOTOR_PID_KI_ADDRESS2 0X4066 //BLDC PID KI HIGH BYTES

#define BLDC_MOTOR_PID_KD_ADDRESS1 0X4067 //BLDC PID KD HIGH BYTES
#define BLDC_MOTOR_PID_KD_ADDRESS2 0X4068 //BLDC PID KD HIGH BYTES
 

    ///////////// UPDATE FLAG EEPROM ADDRESS ///////////// 
 
#define EEPROM_BASE_ADDRESS 0X4300  //STM8S 0X4300 ;STM8L -- 0X1300

typedef enum _adress_data
{
	eeprom_updata_order_adress              = EEPROM_BASE_ADDRESS,
        eeprom_byte_adress                      = EEPROM_BASE_ADDRESS+1,      //   4011--4014
     //  eeprom_erase_write_finish_adress        = EEPROM_BASE_ADDRESS+5,
      //   eeprom_erase_write_begin_adress         = EEPROM_BASE_ADDRESS+7,
       eeprom_flag_finish_adress               = EEPROM_BASE_ADDRESS+6,
       
       eeprom_crc_receive_all_data_addr_l = EEPROM_BASE_ADDRESS+20,
       eeprom_crc_receive_all_data_addr_h = EEPROM_BASE_ADDRESS+21,
     
       eeprom_erase_write_begin_adress         = 0x17FFE,   
       eeprom_erase_write_finish_adress        = 0x17FFF,  //STM8S    >=64K(0X17FFF) ;; <=32K(0XFFFF)
        aap_jump_iap_flag_adress                = EEPROM_BASE_ADDRESS+8,
        eeprom_erase_write_ID_FLAG_adress       = EEPROM_BASE_ADDRESS+9,
        eeprom_updata_order_adress_data         = 0x0A,
        eeprom_erase_write_finish_adress_data   = 0X0B,
        eeprom_flag_finish_adress_data          = 0x0C,
        eeprom_erase_write_begin_adress_data    = 0x0D,
        aap_jump_iap_flag_DATA                  = 0x0E,
        eeprom_erase_write_ID_FLAG_adress_data  = 0X0F,
}adress_data;




typedef enum{LED_1,LED_2,LED_3,LED_4,LED_5,LED_6,LED_7,LED_8}LED_NUM;
typedef enum{OFF,ON}LED_OPTION;
typedef enum{task_rfid,task_stepmotor,task_bldc}task_proc;
typedef enum{TIM4_RFID,TIM4_BLDC}TM4_FLAG;


#ifdef _SystemInit_VER
#define SYSTEM_EXT  
#else
#define SYSTEM_EXT extern 
#endif


SYSTEM_EXT volatile u8      tasknum;
SYSTEM_EXT u8 TIM4_FUNCTION_FLAG;

SYSTEM_EXT u32 	boardID;
SYSTEM_EXT u8 Driver1_step;

SYSTEM_EXT u8 CAN_Command_flag;

SYSTEM_EXT u8 LED_OPTION_DATA;

SYSTEM_EXT u32 _Merge_32Bit(u8 xbyte,u8 hbyte,u8 mbyte,u8 lbyte);

SYSTEM_EXT u8 Lsb_8Bit(u16 word);
SYSTEM_EXT u8 Msb_8Bit(u16 word);


SYSTEM_EXT void CLK_INIT(void) ;
SYSTEM_EXT void iDog_INIT(void);

SYSTEM_EXT void HSE_CLK_INIT(void);

SYSTEM_EXT void GPIO_INIT(void);

SYSTEM_EXT void EXTI_INIT(void);

SYSTEM_EXT void TIMER1_INIT(u16 period);
SYSTEM_EXT void TIMER2_INIT(void);

SYSTEM_EXT void TIMER3_INIT(TIM3_Prescaler_TypeDef TIM3_Prescaler,u16 period);  //1us *124=1ms

SYSTEM_EXT void TIMER4_INIT(void);
SYSTEM_EXT void delay_us(uint16_t delay);
SYSTEM_EXT void delay_ms(unsigned int ms);
SYSTEM_EXT void EEPROM_INIT(void);

SYSTEM_EXT void TASK_PROCESS(u8 tasknum);
SYSTEM_EXT void CLR_MEMORY(u8 *DATAaddress, u8 length);


SYSTEM_EXT void SYSTEMInit(void);

SYSTEM_EXT u16 _Merge_16Bit(u8 mbyte,u8 lbyte);

SYSTEM_EXT void LED_OPTION_PROCESS(u8 num,u8 type);

#endif
