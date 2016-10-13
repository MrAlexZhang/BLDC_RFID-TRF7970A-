#include "stm8s.h"

#ifndef _SPM_H
#define _SPM_H

#define hardwaremainversion  1
#define hardwaresubversion 0
#define softwaremainversion 1
#define softwaresubversion 1

#define 	FLAG_HEAD1     0xaa
#define 	FLAG_HEAD2     0x55

#define PROGRAMME_TYPENUM           1
#define RFIDBOARD_TYPE              1 //RFIDVER :1 快发  ;2:智能存取 ;3 绑定台
//#define UARTPORT               //绑定台版本需要置位，用UARTPROT，其余版本用485PORT

#define UART1_RECBUF_LEN 128







#define 	    SET_MEDICINE_LENGTH     0x01
#define         SEND_OUT_MEDICINENUM     0x02
#define         SEND_board_contral      0x03 


#define         READ_RFID_DATA          0x04
#define         WRITE_RFID_DATA         0x05
#define 	    SET_ZERO_RAW_WEIGHT     0x06
#define         SET_STANDARD_WEIGHT     0x07       
#define         SET_CALIBRATION         0x09
#define         SET_CALIBRATION_ZERO    0x0a
#define         SET_EMPTY_THRESHOLD     0x0b
#define         GET_CURRENT_VERSION     0x0c
#define         SET_START_MONITOR       0x0d
#define         SET_STOP_MONITOR        0x0e
#define         Online_updata           0x0f
#define         OUTPort_Opterate        0x15
#define         ASK_WEIGHTVAULE         0x16
#define         MONITOR_WEIGHTVAULE      0x17
#define         SET_WEIGHTZERO           0x18
#define         Test_Command           0x20




#define         SEND_CURRENT_WEIGHT     0x80
#define         SEND_CURRENT_RFID_ID    0x81
#define         SEND_CURRENT_INFRARED   0x82
#define         SEND_CURRENT_MCU_ID     0x83


#define         SEND_READ_RFID_DATA     0x84
#define         SEND_CURRENT_VERSION    0x8c
#define         SEND_START_MONITOR      0x8d
#define         SEND_STOP_MONITOR       0x8e
#define         SEND_Online_updata      0x8f
#define         SEND_WEIGHTVAULE        0x96

#define         SEND_READY_STATUS       0x90

enum{OUT0,OUT1,OUT2,OUT3,OUT4,OUT5,OUT6,OUT7};
enum{OUTOFF,OUTON,OUTFLASH};

typedef enum _EM_RXSTATE
{
	RXSTATE_HEAD1 = 0,
	RXSTATE_HEAD2 = 1,	
	RXSTATE_ID = 3,	
	RXSTATE_TYPE = 4,
	RXSTATE_LEN = 5,
	RXSTATE_DATA = 6,
	RXSTATE_CHECK = 7
}EM_RXSTATE;


typedef struct tagOUTPortFLAG
 {
   u8 OUTPort0flash; 
   u8 OUTPort0status;
   u8 OUTPort1flash; 
   u8 OUTPort1status;
   u8 OUTPort2flash; 
   u8 OUTPort2status;
   u8 OUTPort3flash; 
   u8 OUTPort3status;
   u8 OUTPort4flash; 
   u8 OUTPort4status;
   u8 OUTPort5flash; 
   u8 OUTPort5status;
   u8 OUTPort6flash; 
   u8 OUTPort6status;
   u8 OUTPort7flash; 
   u8 OUTPort7status;
   u8 OUTPort8flash; 
   u8 OUTPort8status;
   }
SetOUTPortFLAG;


#ifdef _SPM_VER

long actualweight=0;
u8 Sendweightvalueflag=0;
u8 SETzeroweightflag=0;
u8 counter_delay_ms=0;
u8 uart1_recbuf[UART1_RECBUF_LEN]={0};/*uart2 receve buffer length*/
u8 uart1_recbuf_index=0;
u8 uart1_recbuf_num=0;
 u8 uart1_delrecbuf_index=0;/*串口取数下标*/
 u8 BELTmotor_driver_flag=1;


SetOUTPortFLAG OUTPortFLAG={0};
#else

extern long actualweight;
extern u8 Sendweightvalueflag;
extern u8 SETzeroweightflag;
extern u8 counter_delay_ms;

extern u8 uart1_recbuf[UART1_RECBUF_LEN];/*uart2 receve buffer length*/
extern u8 uart1_recbuf_index;
extern u8 uart1_recbuf_num;
extern u8 uart1_delrecbuf_index;/*串口取数下标*/
extern u8 BELTmotor_driver_flag;

extern SetOUTPortFLAG OUTPortFLAG;
#endif

extern u8 boardID;
extern u8 tag_saomiao;
extern u8 startflag;
extern u8 IRStatus;
extern u8 LastIRStatus;

extern u8 lasttag[8];
extern u8 temptag[8];
extern u8 flagdata[5];

void SPM_INIT(u8 boardID);
u8 SPM_Send1Byte(unsigned char data);
void SPM_ParseMessage(u8 data);
void SPM_PrintLong(long data);
u8 SPM_SendArray(u8 arr[],u8 length);
u8 SPM_Print(u8 *str);
void SPM_SET_READ();
void SPM_SET_WRITE();

void SPM_SENDWEIGHT();
void SPM_PrintLong2(long a,long b);


void SPM_VERSION();
void SPM_READY();
void ResponseProgram(u8 type);
void SPM_TESTDATA(u8 num,u16 data);

void RFIDLight_Opterate(u8 num,u8 action);

void RFIDOUTPort_Opterate(u8 num,u8 action);
void SPM_WEIGHTVAULE(u8 status,long vaule);
extern void SPM_PrintLong1(long a,long b,long c);
extern u8 getCard(u8 *data);

extern void delay_ms(unsigned int ms);

extern void delay_1ms(void);

extern void TASK_PROCESS(void);
extern void UART1_INIT(void);
extern u8 UART1_GET_CHAR(void);


#endif
