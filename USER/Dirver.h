
#ifndef __Driver_H
#define __Driver_H






enum{medicine1_putouttype,medicine2_putouttype,medicine1_putintype,medicine2_putintype};
enum{slot0,slot1,slot2,slot3};

typedef union tagmedicine1Info_val
  {
	 u8 VAL; 	
	 struct tagSetmedicine1flag
  	    {
          u8 medicine1_putoutstatus       :1;
		  u8 medicine1_putinstatus        :1;
          u8 putout_lastmedicine1         :1;	
		  u8 medicine1_protect            :1;
		  u8 rev                          :4;
	      
          }
      stSetmedicine1flag; 
	               
  }
stSetmedicine1Info_val;

typedef union tagmedicine2Info_val
  {
	 u8 VAL; 	
	 struct tagSetmedicine2flag
  	    {
       
		  u8 medicine2_putoutstatus       :1;
		  u8 medicine2_putinstatus        :1;
          u8 putout_lastmedicine2         :1;
	      u8 medicine2_protect            :1;
	      u8 rev                          :4;
          }
      stSetmedicine2flag; 
	               
  }
stSetmedicine2Info_val;



typedef struct tagsoltInfo
   {
	u16  medicine1_lenght;
	u16  medicine1_weight;
	u8   medicine1_maxkeepnum;	
	u16  medicine2_lenght;
	u16  medicine2_weight;
	u8   medicine2_maxkeepnum;
	//u8   medicine2_manual_putin;
	u8   medicine_manual_putin;
	u16  motor_PPS;
	u8   last_medicine_protect;
	u8   board1motor_direct;
	u8   board2motor_direct;
	u8   outputdac1;
	u8   outputdac2;
	u8   inputdac;
	u8   motordac;
	u8   slotmode;
	u8   maxkeepweight;
	u8   lastmedicie1_boardmode;
	u8   lastmedicie2_boardmode;
    }
stSetsolt_info;


typedef struct tagputmedicine1_Info
  {
	u8 medicine1_putoutnum;
    u8 medicine1_putinnum;
	u8 medicine1_currentkeepnum;
	u8 medicine1_current_outnum;
	u8 medicine1_current_innum;
	u8 medicine1_current_leavenum;
    u16 medicine1Putout_totallenght;
	
	u16 medicine1_motor_totaloutsteps;
	u16	medicine1_motor_currentoutsteps;
	u16 medicine1_motor_singleoutsteps;
	u16  medicine1_motor_lastnumoutsteps;
	u16 medicine1_motor_totalinsteps;
	u16 medicine1_motor_currentinsteps;

  }

stPutmedicine1_Info;


typedef struct tagputmedicine2_Info
  {
	u8 medicine2_putoutnum;
    u8 medicine2_putinnum;
	u8 medicine2_currentkeepnum;
	u8 medicine2_current_outnum;
	u8 medicine2_current_innum;
	u8 medicine2_current_leavenum;
    u16 medicine2Putout_totallenght;
	
	u16 medicine2_motor_totaloutsteps;
	u16	medicine2_motor_currentoutsteps;
	u16 medicine2_motor_singleoutsteps;
	u16  medicine2_motor_lastnumoutsteps;
	u16 medicine2_motor_totalinsteps;
	u16 medicine2_motor_currentinsteps;

  }

stPutmedicine2_Info;

typedef struct tagLED_status

  {
    u8 led1status;
	u8 led2status;
	u8 led3status;
	u8 led4status;

  }

stLED_status;

#ifdef _Driver_VER
stSetmedicine1Info_val Setmedicine1Info_val={0}; 
stSetmedicine2Info_val Setmedicine2Info_val={0}; 

stPutmedicine1_Info  Putmedicine1_Info={0};
stPutmedicine2_Info  Putmedicine2_Info={0};
stSetsolt_info    Setsolt_info={0};
stLED_status    LED_status={0};


u32 BeltContralRUN_stepnum=0;

u8 IRF_TEST_FLAG=0;
u8 system_restart_rspflag=0;
u8 stop_beltflag=0;
long standardWeight=0;
u16 Belt_pwmnum=125;
float singlestep_length=7.85;
u32 tim_pps=1000000; 
u16 lastmedicine1_move_length=400;
float lastmedicine1_move_scale=3;
u16 medicine1_slot_protectmovesteps=4000;  //16000/7.85

u16 lastmedicine2_move_length=400;
float lastmedicine2_move_scale=3;
u16 medicine2_slot_protectmovesteps=4000;  //16000/3.925

//#if SOFT_MODIFY == 1
u8 medicine1_putin_single_finish_nofound = 0;
u8 medicine2_putin_single_finish_nofound = 0;
//#endif

#else
extern stSetmedicine1Info_val Setmedicine1Info_val; 
extern stSetmedicine2Info_val Setmedicine2Info_val; 

extern stPutmedicine1_Info  Putmedicine1_Info;
extern stPutmedicine2_Info  Putmedicine2_Info;
extern stSetsolt_info    Setsolt_info;
extern stLED_status    LED_status;


extern u32 BeltContralRUN_stepnum;
extern u8 IRF_TEST_FLAG;

extern u8 system_restart_rspflag;

extern u8 stop_beltflag;

extern long standardWeight;
extern float singlestep_length;
extern u8  Belt_pwmnum;

extern u32 tim_pps;
extern u16 lastmedicine1_move_length;
extern u16 medicine1_slot_protectmovesteps;
extern float lastmedicine1_move_scale;

extern u16 lastmedicine2_move_length;
extern float lastmedicine2_move_scale;
extern u16 medicine2_slot_protectmovesteps;  //16000/7.85

//#if SOFT_MODIFY == 1
extern u8 medicine1_putin_single_finish_nofound;
extern u8 medicine2_putin_single_finish_nofound ;
//#endif


#endif



extern void Board1_CTRL(u8 move);

extern void Board2_CTRL(u8 move);

extern void IRF_CTRL(u8 IRNUM,u8 move);

extern void belt_contral(void);


void LED_CTRL(u8 LEDNUM,u8 action);

void DRV8812INIT(void);

void delay(u8 time);
void pulse();

 void DAC_SendDate(u16 data);

void BELTmotor_move(u8 contraltype);

void Putoutbeltmotor_stepcontral(void);
void Putinbeltmotor_stepcontral(void);

void CLR_medicineflag(u8 type );



#endif



