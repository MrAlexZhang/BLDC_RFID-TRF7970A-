
#define _SPM_VER


#include "Globe.h"
#include <stdlib.h>




struct MESSAGE_FRM
{       u8 ID;
	u8 type;
	u8 len;	
	u8 data[20];
} msg,msgout;

union unionvalue{
  long value;
  u8 data[4];
}unionvalueinstance;

u8 writewithfeedback=0;





u16 _Merge_16Bit(u8 mbyte,u8 lbyte)
{
	u16 _data = 0;
	_data |= mbyte;
	_data <<= 8;
	_data |= lbyte;

	return _data;
}
u8 UART1_GET_CHAR(void)
{
  u8 temp = 0x00;
  if(uart1_recbuf_num>0){ 
    temp = uart1_recbuf[uart1_delrecbuf_index];
    uart1_delrecbuf_index++;
    disableInterrupts();
    uart1_recbuf_num--;
    enableInterrupts();
    if(uart1_delrecbuf_index > (UART1_RECBUF_LEN-1))
	 {
      uart1_delrecbuf_index = 0x00;
     }
    return temp;
  }else
  {
    return 0;
  }
}

void task_exectue(struct MESSAGE_FRM msg)
	{
    
		
  if((msg.ID==boardID)||(msg.ID==0))
  	{ 
      switch (msg.type)
      {
      case SET_MEDICINE_LENGTH:
        medicine_lenght=_Merge_16Bit(msg.data[0],msg.data[1]);
        break;
      case SEND_OUT_MEDICINENUM:       

        medicine_OUTNUM=msg.data[1];
	
	    BELTmotor_driver_flag=msg.data[0];
        
        break;
      case SEND_board_contral:      
		 if( msg.data[0]==1)

	          BoardDOWN_flag=1;
		 else if(msg.data[0]==0)
			 BoardUP_flag=1;

     
      case GET_CURRENT_VERSION:
        SPM_VERSION();
        break; 

		default:
       	return;
      	}
  	}
   
}

void SPM_SEND_MESSAGE(struct MESSAGE_FRM msg){
	u8 checksum=0;
  delay_ms(2);
  SPM_SET_WRITE();
    delay_ms(3);
  SPM_Send1Byte(FLAG_HEAD1);
  SPM_Send1Byte(FLAG_HEAD2);
  SPM_Send1Byte(msg.ID);
  checksum=msg.ID;
  SPM_Send1Byte(msg.type);
  checksum=checksum^msg.type;
  SPM_Send1Byte(msg.len);
  checksum=checksum^msg.len;
  for (u8 i=0;i<msg.len;i++){
    SPM_Send1Byte(msg.data[i]);
    checksum=checksum^msg.data[i];
  }
  SPM_Send1Byte(checksum);
  delay_ms(2);
  SPM_SET_READ();


}

/*******************************************************************************
*                         ==UART2��ʼ������==
* UART2_DeInit();                                           --> ��λ����2�ļĴ���ֵ
* UART2_Init((u32)9600, UART2_WORDLENGTH_8D, UART2_STOPBITS_1, UART2_PARITY_NO, UART2_SYNCMODE_CLOCK_DISABLE, UART2_MODE_TXRX_ENABLE);
                                                            --> ������ 9600 , 8λ����λ , 1λֹͣλ , ����żЧ�� , , ���ͽ���ʹ��
* UART2_ITConfig(UART2_IT_RXNE, ENABLE);                    --> ʹ�ܽ����ж�
* UART2_Cmd(ENABLE);                                        --> ʹ�ܴ���2
*******************************************************************************/
void UART1_INIT(void)
{
  
  uart1_recbuf_index = 0x00;
  uart1_recbuf_num = 0x00;
  uart1_delrecbuf_index = 0x00;
  for(u8 i=0;i<UART1_RECBUF_LEN;i++){
    uart1_recbuf[i]=0x00;
  }
  UART1_DeInit();
  UART1_Init((u32)115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
  UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);

  UART1_Cmd(ENABLE);
}


void SPM_SET_READ()  {
  for(int i=0;i<500;i++){//״̬�л���Ҫ����ʱ������֮ǰд����������״̬��û�н������������ݲ��ܸ���ȷ����
  }
  GPIO_WriteLow(GPIOB , GPIO_PIN_0);//����ͣ����ͽ�ֹ
  GPIO_WriteLow(GPIOB , GPIO_PIN_1);//�����, ����ʹ��

  writewithfeedback=0;
  UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
}

void SPM_SET_WRITE(){


  GPIO_WriteHigh(GPIOB , GPIO_PIN_0);//����ͣ�����ʹ��
   
  GPIO_WriteHigh(GPIOB , GPIO_PIN_1);//�����, ���ս�ֹ
  writewithfeedback=0;
}

void SPM_SET_WRITE_WITH_FEEDBACK(){
  GPIO_WriteHigh(GPIOD , GPIO_PIN_4);//����ߣ� ����ʹ�� 
  GPIO_WriteLow(GPIOD , GPIO_PIN_7);//�����, ����ʹ��
  writewithfeedback=1;
  UART1_ITConfig(UART1_IT_RXNE, DISABLE);
}

void SPM_INIT(u8 board){
 boardID=board;

  SPM_SET_READ();
  UART1_INIT();
}


/*�ó����UART2��������ȡ��һ���ֽڵ����ݡ�*/
u8 UART2_GET_CHAR(void)
{
  u8 temp = 0x00;
  if(uart1_recbuf_num>0){ 
    temp = uart1_recbuf[uart1_delrecbuf_index];
    uart1_delrecbuf_index++;
    disableInterrupts();
    uart1_recbuf_num--;
    enableInterrupts();
    if(uart1_delrecbuf_index > (UART1_RECBUF_LEN-1))
	 {
      uart1_delrecbuf_index = 0x00;
     }
    return temp;
  }else
  {
    return 0;
  }
}




void SPM_ParseMessage(u8 data)
{   
  static EM_RXSTATE gRxState=RXSTATE_HEAD1;          //���ݽ������ڼ���
  static u8 CheckSum = 0;
  static int length;
  switch(gRxState)
    {
      case RXSTATE_HEAD1:
              if(data== FLAG_HEAD1)
                      gRxState = RXSTATE_HEAD2;
              break;
      case RXSTATE_HEAD2:
              if(data == FLAG_HEAD2)
                      gRxState = RXSTATE_ID;
              else if(data == FLAG_HEAD1)
                      gRxState = RXSTATE_HEAD2;
              else
                      gRxState = RXSTATE_HEAD1;
              break;
     
      case RXSTATE_ID:
	  	      if((boardID==data)||(data==0))
               {
                msg.ID=data;
                CheckSum=data;
                gRxState= RXSTATE_TYPE;
	  	      	}
			  else
			  	{
			  	gRxState=RXSTATE_HEAD1;
			  	  }
              break;
      case RXSTATE_TYPE:
            msg.type=data;
            CheckSum=CheckSum^data;
            gRxState= RXSTATE_LEN;
	    break;
      case RXSTATE_LEN:
              msg.len = data;
              if(data>20){//should be less than 20 bytes for payload
                gRxState = RXSTATE_HEAD1;
                 break;
              }
	      CheckSum = CheckSum^data;
              length=0;
	       if(msg.len!= 0)
              {       
                 gRxState = RXSTATE_DATA;   
              }
              else
              {
              	  gRxState = RXSTATE_CHECK;
              }
              break;
      case RXSTATE_DATA:
              msg.data[length] = data;
	      CheckSum = CheckSum^data;
	      length++;
	      if(length == msg.len)
		gRxState = RXSTATE_CHECK;
	      break;
      case RXSTATE_CHECK:
              if(CheckSum == data)
              {
                task_exectue(msg);
	
              }
              
	        gRxState = RXSTATE_HEAD1;
        break;
      default:
              gRxState = RXSTATE_HEAD1;
              break;
    }
  
}




/*******************************************************************************
*                         ==UART2_TX ����1���ֽ� ����==
* UART2_FLAG_TXE : Transmit Data Register Empty flag ����2���ͼĴ���Ϊ�ձ�־λ ����1 Ϊ�� ����0 ����
*******************************************************************************/
u8 SPM_Send1Byte(unsigned char TxData)
{
  while(UART1_GetFlagStatus(UART1_FLAG_TXE) == 0);
  UART1_SendData8(TxData); 
  if(writewithfeedback==1){
      for(u8 i=0;i<200;i++);//���ͽ�����ʱ
      if(UART1_ReceiveData8()!=TxData){
          return 0;
      }
       else
      {
        return 1;
      }
   }
   else 
   {
      return 1;
   }
}

/**********************************************************************************************************
** �������� ��UART0_Print
** �������� ���򴮿ڷ���һ�ַ���
** ��ڲ��� ��str	Ҫ���͵��ַ�����ָ��
** ���ڲ��� ����
**********************************************************************************************************/
u8 SPM_Print(u8 *str)
{
  SPM_SET_WRITE_WITH_FEEDBACK();
  while(*str !='\0')  {
    if(SPM_Send1Byte(*str++) !=1){
       SPM_SET_READ();
       return 0;
    }
  }
  SPM_SET_READ();
  return 1;
}

/**********************************************************************************************************
** �������� ��SPM_SendArray
** �������� ���򴮿ڷ���һ����
** ��ڲ��� ������
** ���ڲ��� ����
**********************************************************************************************************/
u8 SPM_SendArray(u8 array[],u8 length)
{
  SPM_SET_WRITE_WITH_FEEDBACK();
  for (u8 i=0;i<length;i++){
    if(SPM_Send1Byte(array[i]) !=1){
       SPM_SET_READ();
       return 0;
    }
  }
  SPM_SET_READ();
  return 1;
}

void SPM_PrintLongInt(long a){
  if(a<0){
     SPM_Send1Byte(45);
     a=-a;
  }
   if(a>=10) 
     SPM_PrintLongInt(a/10);
   SPM_Send1Byte((u8)(a%10+48));
}

 void SPM_PrintLong(long a)
{
  SPM_SET_WRITE();
  SPM_PrintLongInt(a);
  SPM_Send1Byte('\r');
  SPM_Send1Byte('\n');
  SPM_SET_READ();
}


 void SPM_PrintLong2(long a,long b)
{
  SPM_SET_WRITE();
  SPM_PrintLongInt(a);
  SPM_Send1Byte('\t');
  SPM_PrintLongInt(b);
  SPM_Send1Byte('\r');
  SPM_Send1Byte('\n');
  SPM_SET_READ();
}


 
 void SPM_PrintLong1(long a,long b,long c)
{
  SPM_SET_WRITE();
  SPM_PrintLongInt(a);
  SPM_Send1Byte('\t');
  SPM_PrintLongInt(b);
  SPM_Send1Byte('\t');
  SPM_PrintLongInt(c);
  SPM_Send1Byte('\r');
  SPM_Send1Byte('\n');
  SPM_SET_READ();
}


//*************���ͺ����ź�shuju[1]**********
void SPM_INFRARED(u8 temp)
{     
     msgout.ID=boardID;
     msgout.type=SEND_CURRENT_INFRARED;
     msgout.len=1;
     msgout.data[0]=temp;
     SPM_SEND_MESSAGE(msgout);

}



void SPM_VERSION(){
	
     msgout.ID=boardID;
     msgout.type=SEND_CURRENT_VERSION;
     msgout.len=4;
     #if  RFIDBOARD_TYPE==1 
     msgout.data[0]=hardwaremainversion;
	 #endif
     #if  RFIDBOARD_TYPE==2
      msgout.data[0]=hardwaremainversion+1;
     #endif
	 #if  RFIDBOARD_TYPE==3
      msgout.data[0]=hardwaremainversion+2;
     #endif
     msgout.data[1]=hardwaresubversion;
    
     msgout.data[2]=softwaremainversion;
     msgout.data[3]=softwaresubversion;
     SPM_SEND_MESSAGE(msgout);
}

void SPM_READY(){
     msgout.ID=boardID;
     msgout.type=SEND_READY_STATUS;
     msgout.len=0;
     SPM_SEND_MESSAGE(msgout);
}


void ResponseProgram(u8 type)
{
	 msgout.ID=boardID;
     msgout.type=type;
     msgout.len=0;
    
     SPM_SEND_MESSAGE(msgout);
}










/*******************************************************************************
*                           ==RX �����ж�==
*   UART1_IT_RXNE : Data Register Not Empty interrupt   ���ռĴ��� �� ��־λ 
*******************************************************************************/

#pragma vector=0x14

__interrupt void UART1_RX_IRQHandler(void)
{
  disableInterrupts();
  uart1_recbuf_num++;
  if(uart1_recbuf_num <= UART1_RECBUF_LEN)
  	{
     if(uart1_recbuf_index < UART1_RECBUF_LEN)
	   {
         uart1_recbuf[uart1_recbuf_index]=UART1_ReceiveData8();//save data
         uart1_recbuf_index++;
       }
  	 else
	   {
        uart1_recbuf_index = 0x00;
        uart1_recbuf[uart1_recbuf_index] = UART1_ReceiveData8();
         uart1_recbuf_index++;
        }
    }
   else
   	{
     UART1_ReceiveData8();
     uart1_recbuf_num = UART1_RECBUF_LEN;
    }
     enableInterrupts();
 }

