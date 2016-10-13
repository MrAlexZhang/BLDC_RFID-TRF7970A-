#define _Driver_VER


#include "Globe.h"




void delay(u8 time)
{
  for(u8 i=0;i<time;i++){
    
  }
}



void Board1_CTRL(u8 move)

  {
    if(move==ON)
     {Board1movetime=0;

	  if(!Setsolt_info.board1motor_direct)
		SET_MOTOR1_CTRL;
	  else
	  	CLR_MOTOR1_CTRL;
	    SET_MOTOR1_Enable;

			 Board1status=1;
		
		TIM4_Cmd(ENABLE);		 
		  

	  
      }
    else if(move==OFF)
      	{
		 
     	Board1movetime=0;
		 if(!Setsolt_info.board1motor_direct)
	     CLR_MOTOR1_CTRL;
		 else
		 SET_MOTOR1_CTRL;	
	    
	     SET_MOTOR1_Enable;
         Board1status=1;
	      
        TIM4_Cmd(ENABLE);
        }
	if(move==STOP)
		{ 
		Board1movetime=0;
		 CLR_MOTOR1_Enable;
		// TIM4_Cmd(DISABLE);
	     Board1status=0;

	      }
	
	
   }
void Board2_CTRL(u8 move)
	{
	   if(move==ON)
     {
		Board2movetime=0; 
	   if(!Setsolt_info.board2motor_direct)
	   SET_MOTOR2_CTRL;
	   else
	   CLR_MOTOR2_CTRL;
	   
	   SET_MOTOR2_Enable; 
	   Board2status=1;
        
		TIM4_Cmd(ENABLE);
        
    	

	   
	  
      }
    else if(move==OFF)
      	{
		Board2movetime=0;  

		  if(!Setsolt_info.board2motor_direct)
		   CLR_MOTOR2_CTRL;	
		  else
		  	SET_MOTOR2_CTRL;
		   SET_MOTOR2_Enable;	
		  Board2status=1;
	     TIM4_Cmd(ENABLE);
        
	
        }
	if(move==STOP)
		{ 
		 	Board2movetime=0;  
		 CLR_MOTOR2_Enable;
		// TIM4_Cmd(DISABLE);
	     Board2status=0;

	      }
	
	 }


void IRF_CTRL(u8 IRNUM, u8 move)
  {


    switch(IRNUM)
    	{
         case 0:
		 	if(move==ON)
		 		{
				SET_IRCTRL1;
				SET_IRCTRL2;
				SET_IRCTRL3;
				SET_IRCTRL4;
			    }
             else if (move==STOP)
             	{
				 CLR_IRCTRL1;
				 CLR_IRCTRL2;
				 CLR_IRCTRL3;
                 CLR_IRCTRL4; 

			     }

			break;
	     case 1:
	        if(move==ON)
		        {
		         SET_IRCTRL1;
		  
		        }
            else if(move==STOP)
			   {
			   CLR_IRCTRL1;
		    	}

			break;
		
	     case 2:
	        if(move==ON)
		        {
		         SET_IRCTRL2;
		  
		        }
            else if(move==STOP)
			    {
			      CLR_IRCTRL2;
		    	}

			break;

	      case 3:
	        if(move==ON)
		        {
		         SET_IRCTRL3;
		  
		        }
            else if(move==STOP)
			   {
			     CLR_IRCTRL3;
		    	}

			break;
				
		  case 4:
			 if(move==ON)
				{
				 SET_IRCTRL4;
			
				}
			else if(move==STOP)
			   {
				 CLR_IRCTRL4;
				}
			break;
          default:

		  	break;
		  
		  


    }
 
 
}


void LED_CTRL(u8 LEDNUM,u8 action)
{


	switch(LEDNUM)
        
		{

		 case LED0:
			
			 	if(action==ON) 
					 {
					   SET_LED1;
					   SET_LED2;
					   SET_LED3;
					   SET_LED4;
					   LED_status.led1status=1;
					   LED_status.led2status=1;
					   LED_status.led3status=1;
					   LED_status.led4status=1;
						   
							}
						if(action==STOP)
						  {
						   CLR_LED1;
						   CLR_LED2;
						   CLR_LED3;
						   CLR_LED4;
						   LED_status.led1status=0;
					   LED_status.led2status=0;
					   LED_status.led3status=0;
					   LED_status.led4status=0;
							}
						if(action==OFF)
							{ SET_LED1;
					          SET_LED2;
					          SET_LED3;
					          SET_LED4;
					         LED_status.led1status=2;
					         LED_status.led2status=2;
					         LED_status.led3status=2;
					         LED_status.led4status=2;
							 TIM2_Cmd(ENABLE);
							}
            
			break;
         case LED1:
            if(action==ON) 
               {SET_LED1;
               LED_status.led1status=1;
			   
            	}
            if(action==STOP)
              {
               CLR_LED1;
			   LED_status.led1status=0;
            	}
			if(action==OFF)
				{
				  SET_LED1;
			     LED_status.led1status=2;
				 TIM2_Cmd(ENABLE);
				}
			
	       break;

	     case LED2:
            if(action==ON) 
				
             { SET_LED2;
			  LED_status.led2status=1;}
			  
            if(action==STOP)
              { CLR_LED2;
			   LED_status.led2status=0;}
			if(action==OFF)

			   { SET_LED2;
				 LED_status.led2status=2;
				 TIM2_Cmd(ENABLE);

			   }

			
	       break;
		   
		  case LED3:
            if(action==ON) 
             { SET_LED3;
			   LED_status.led3status=1;}
            if(action==STOP)
                { CLR_LED3;
			    LED_status.led3status=0;}
			if(action==OFF)
			   { SET_LED3;
				 LED_status.led3status=2;
				 TIM2_Cmd(ENABLE);}

				
	       break;

	      case LED4:
            if(action==ON) 
              {SET_LED4;
			   LED_status.led4status=1;}
            if(action==STOP)
                { CLR_LED4;
			     LED_status.led4status=0;}
			if(action==OFF)
			 { SET_LED4;
			   LED_status.led4status=2;
			   TIM2_Cmd(ENABLE);}

				 
	       break;
		   default:

		   	break;

		
		}
  }
void BELTmotor_move(u8 move)
  {
   
    if(move==ON)
	  {
	    SET_nSLEEP;
      
		SetbeltInfo_val.stsetbeltcontral.run=1;
        TIM3_Cmd(ENABLE);
    	}
	if(move==STOP)
      {
	    CLR_nSLEEP;
       coil_off;
			
		SetbeltInfo_val.VAL=0;
		
        TIM3_Cmd(DISABLE);
    	}

   }



void DAC_SendDate(u16 data)//////写数据到DAC
{ 
  unsigned int i;
  u16 voltage;
  voltage=data;
  SET_DAC_SNYC;

  CLR_DAC_SNYC;
  SET_DAC_CLK;
  for(i=0;i<16;i++)
  {  
    SET_DAC_CLK;
    if((voltage&0x8000)==0)
       CLR_DAC_DATA;
      else 
       SET_DAC_DATA;  
     CLR_DAC_CLK;
    
     voltage=voltage<<1;
  }
  SET_DAC_CLK;
  SET_DAC_DATA;
  SET_DAC_SNYC; 
 
}


void DRV8812INIT(void)
 { 
  
	
    SET_nSLEEP;
	CLR_nRESET;
    delay_ms(1);
    SET_nRESET;
    CLR_nSLEEP;
	


   
}
   
   

void pulse()
	{
  
  delay(5);
  GPIO_WriteLow(GPIOD , GPIO_PIN_0);
  delay(2);
}


void Putoutbeltmotor_stepcontral(void)
  {

    if(Setmedicine1Info_val.stSetmedicine1flag.medicine1_putoutstatus)
    	{

		
             if(!Setmedicine1Info_val.stSetmedicine1flag.putout_lastmedicine1)      //未到发最后一盒药的情况下
	           {
                if((Putmedicine1_Info.medicine1_current_outnum>=Putmedicine1_Info.medicine1_putoutnum)&&(Putmedicine1_Info.medicine1_motor_totaloutsteps>=0))  //正准备发最后一盒药
                   {
                      Setmedicine1Info_val.stSetmedicine1flag.putout_lastmedicine1=1;
					 
				      Putmedicine1_Info.medicine1_motor_totaloutsteps=Putmedicine1_Info.medicine1_motor_lastnumoutsteps;
					  Putmedicine1_Info.medicine1_motor_currentoutsteps= Putmedicine1_Info.medicine1_motor_totaloutsteps;
			        }
						
			  

		        if((Putmedicine1_Info.medicine1_current_outnum<Putmedicine1_Info.medicine1_putoutnum)&&(Putmedicine1_Info.medicine1_motor_totaloutsteps==0))  //防止药打滑或药品排列松
		      	   {
                
				      if(!Setmedicine1Info_val.stSetmedicine1flag.medicine1_protect)
				        {
				         Setmedicine1Info_val.stSetmedicine1flag.medicine1_protect=1;
						   
				          Putmedicine1_Info.medicine1_motor_totaloutsteps=medicine1_slot_protectmovesteps;
						  Putmedicine1_Info.medicine1_motor_currentoutsteps= Putmedicine1_Info.medicine1_motor_totaloutsteps;
						

					    }
				       else
				   	    {
				   	    
				   	     Setmedicine1Info_val.stSetmedicine1flag.medicine1_protect=0;
					    
				   	      Board1_CTRL(ON);
						 if(Setsolt_info.slotmode==1)
						   Board2_CTRL(ON);

						 
					
				         CLR_medicineflag(medicine1_putouttype);
				   	    }
			        }
	            
		     	 }		 
	       else if(Setmedicine1Info_val.stSetmedicine1flag.putout_lastmedicine1)       //发送到最后一盒药
	        	 {
				      if(Putmedicine1_Info.medicine1_motor_totaloutsteps==0)   
						 {

						  
		 				        Board1_CTRL(ON);
					     
						   	   if(Setsolt_info.slotmode==1)
						           Board2_CTRL(ON);


							  
						   	     CLR_medicineflag(medicine1_putouttype);
							
						   
				      	 }

			      }
    	}

		

     if(Setmedicine2Info_val.stSetmedicine2flag.medicine2_putoutstatus)
    	{

          if(!Setmedicine2Info_val.stSetmedicine2flag.putout_lastmedicine2)      //未到发最后一盒药的情况下
	          {
                if((Putmedicine2_Info.medicine2_current_outnum>=Putmedicine2_Info.medicine2_putoutnum)&&(Putmedicine2_Info.medicine2_motor_totaloutsteps>=0))  //正准备发最后一盒药
                   {
                      Setmedicine2Info_val.stSetmedicine2flag.putout_lastmedicine2=1;
				      Putmedicine2_Info.medicine2_motor_totaloutsteps=Putmedicine2_Info.medicine2_motor_lastnumoutsteps;
					   Putmedicine2_Info.medicine2_motor_currentoutsteps= Putmedicine2_Info.medicine2_motor_totaloutsteps;
			        }
						
			  

		        if((Putmedicine2_Info.medicine2_current_outnum<Putmedicine2_Info.medicine2_putoutnum)&&(Putmedicine2_Info.medicine2_motor_totaloutsteps==0))  //防止药打滑或药品排列松
		      	   {
                
				      if(!Setmedicine2Info_val.stSetmedicine2flag.medicine2_protect)
				        {
				         Setmedicine2Info_val.stSetmedicine2flag.medicine2_protect=1;
				          Putmedicine2_Info.medicine2_motor_totaloutsteps=medicine2_slot_protectmovesteps;
						  Putmedicine2_Info.medicine2_motor_currentoutsteps=Putmedicine2_Info.medicine2_motor_totaloutsteps;
                          
						
   
					     }
				       else
				   	    {
				   	     Setmedicine2Info_val.stSetmedicine2flag.medicine2_protect=0;
						
				   	     Board2_CTRL(ON);
					
				         CLR_medicineflag(medicine2_putouttype);
				   	    }
			        }
	            
		     	 }		 
	       else if(Setmedicine2Info_val.stSetmedicine2flag.putout_lastmedicine2)       //发送到最后一盒药
	        	 {
				      if(Putmedicine2_Info.medicine2_motor_totaloutsteps==0)   
						 {
						 
						  
						     Board2_CTRL(ON);
							
					         CLR_medicineflag(medicine2_putouttype);
							

						   
				      	 }

			      }
    	}	     
    	
   }

void Putinbeltmotor_stepcontral(void)
 {

/*
   if(!Setsolt_info.medicine_manual_putin)   //自动上药
   	{
	 if(Setmedicine1Info_val.stSetmedicine1flag.medicine1_putinstatus)
		{
		  
		 if((Putmedicine1_Info.medicine1_current_innum==Putmedicine1_Info.medicine1_putinnum)&&(Putmedicine1_Info.medicine1_motor_totalinsteps==4000))//||(Putmedicine1_Info.medicine1_currentkeepnum>=Setsolt_info.medicine1_maxkeepnum))
		 	{  
		 	  
		 	  	// if(!GPIO_ReadInputPin(GPIOC , GPIO_PIN_3))
		 	     //  CLR_medicineflag(medicine1_putintype);   //V19版本
				  if(GPIO_ReadInputPin(GPIOC , GPIO_PIN_3))
				 	{
				// 	 CLR_medicineflag(medicine1_putintype);  // V19版本
					  msgout.data[0]=medicineblind;
	                  SPM_Respond2(SEND_SOLT_ERROR,1,slot1); 
				 	}			        
		 	  	}
               if((Putmedicine1_Info.medicine1_current_innum<Putmedicine1_Info.medicine1_putinnum)&&(Putmedicine1_Info.medicine1_motor_totalinsteps==medicine1_slot_protectmovesteps))
               	{

				 if(!GPIO_ReadInputPin(GPIOC , GPIO_PIN_3))
				    {
				   //  CLR_medicineflag(medicine1_putintype);//V19版本
				     msgout.data[0]=medicine_inputerror;
	                 SPM_Respond2(SEND_SOLT_ERROR,1,slot1); 
				 	}
				 else  if(GPIO_ReadInputPin(GPIOC , GPIO_PIN_3))
					{
				//	 CLR_medicineflag(medicine1_putintype);//V19版本
				     msgout.data[0]=medicineblind;
				     SPM_Respond2(SEND_SOLT_ERROR,1,slot1); 
					}	
				 }

			   
		    }	 
		 
		

	 if(Setmedicine2Info_val.stSetmedicine2flag.medicine2_putinstatus)
		{

		 if((Putmedicine2_Info.medicine2_current_innum==Putmedicine2_Info.medicine2_putinnum)&&(Putmedicine2_Info.medicine2_motor_totalinsteps==4000))//||(Putmedicine2_Info.medicine2_currentkeepnum>=Setsolt_info.medicine2_maxkeepnum))
		 	{
		 	 //   if(!GPIO_ReadInputPin(GPIOC , GPIO_PIN_4))
		 	      //  CLR_medicineflag(medicine2_putintype);//V19 版本
				   if(GPIO_ReadInputPin(GPIOC , GPIO_PIN_4))
				  	{
					// CLR_medicineflag(medicine2_putintype);//V19版本
					 msgout.data[0]=medicineblind;
					 SPM_Respond2(SEND_SOLT_ERROR,1,slot2); 
				     }			    
		 	  	}
		 if((Putmedicine2_Info.medicine2_current_innum<Putmedicine2_Info.medicine2_putinnum)&&(Putmedicine2_Info.medicine2_motor_totalinsteps==medicine2_slot_protectmovesteps))//||(Putmedicine2_Info.medicine2_currentkeepnum>=Setsolt_info.medicine2_maxkeepnum))
			 {
			    if(!GPIO_ReadInputPin(GPIOC , GPIO_PIN_4))
				   {// CLR_medicineflag(medicine2_putintype); //V19版本
				       msgout.data[0]=medicine_inputerror;
	                 SPM_Respond2(SEND_SOLT_ERROR,1,slot2);
			    	}
			    else if(GPIO_ReadInputPin(GPIOC , GPIO_PIN_4))
				   {
					 //CLR_medicineflag(medicine2_putintype);//V19版本
					 msgout.data[0]=medicineblind;
					 SPM_Respond2(SEND_SOLT_ERROR,1,slot2); 
					 } 			 
			  }


		 
		    }	
	    }

      */
  if(Setsolt_info.medicine_manual_putin)    //手动上药
  	 {

       if(Setmedicine1Info_val.stSetmedicine1flag.medicine1_putinstatus==1)
       	{
	   if(Putmedicine1_Info.medicine1_motor_totalinsteps==medicine1_slot_protectmovesteps)
			 {
		       CLR_medicineflag(medicine1_putintype);
			 
		   }
       	}
	   if(Setmedicine2Info_val.stSetmedicine2flag.medicine2_putinstatus==1)
	   	{
	   	  if(Putmedicine2_Info.medicine2_motor_totalinsteps==medicine2_slot_protectmovesteps)
	   	  	{
			  CLR_medicineflag(medicine2_putintype);
			  
	   	  	}
	   	}

       }
  

}


void belt_contral(void)
{

   if(SetbeltInfo_val.stsetbeltcontral.contraltype==1)
   	{
	   BeltContralRUN_stepnum--;
	   if(BeltContralRUN_stepnum==0)
	   	{


		 SetbeltInfo_val.VAL=0;
		 
		 BELTmotor_move(STOP);

	     }


      }

 }



void CLR_medicineflag(u8 type )


{   




   if(type==medicine1_putouttype)
   	{
	   if(!system_restart_rspflag)

	   SPM_Respond_putmedicine(PUTOUT_MEDICINE,1,Putmedicine1_Info.medicine1_current_outnum);
	   if(Setsolt_info.lastmedicie1_boardmode==0)
	      BELTmotor_move(STOP);
	   
	   IRF_CTRL(IRF1,STOP);
	   CLR_LED1;
	   Setmedicine1Info_val.stSetmedicine1flag.medicine1_protect=0;
	   Setmedicine1Info_val.stSetmedicine1flag.medicine1_putoutstatus=0;
	   Setmedicine1Info_val.stSetmedicine1flag.putout_lastmedicine1=0;
	   Putmedicine1_Info.medicine1_current_leavenum=0;	   
	   Putmedicine1_Info.medicine1_currentkeepnum-=Putmedicine1_Info.medicine1_current_outnum;	   
	   Putmedicine1_Info.medicine1_current_outnum=0;
	   Putmedicine1_Info.medicine1_current_leavenum=0;
	   Putmedicine1_Info.medicine1_putoutnum=0;
	   Putmedicine1_Info.medicine1_motor_currentoutsteps=0;
	   Putmedicine1_Info.medicine1_motor_totaloutsteps=0;
	  
      
     }

  if(type==medicine2_putouttype)
  	 {     if(!system_restart_rspflag)
	        SPM_Respond_putmedicine(PUTOUT_MEDICINE,2,Putmedicine2_Info.medicine2_current_outnum);
			if(Setsolt_info.lastmedicie2_boardmode==0)
			   BELTmotor_move(STOP);	
			
			  IRF_CTRL(IRF2,STOP);
			  CLR_LED2;
			 Setmedicine2Info_val.stSetmedicine2flag.medicine2_protect=0;
			 Setmedicine2Info_val.stSetmedicine2flag.medicine2_putoutstatus=0;
			 Setmedicine2Info_val.stSetmedicine2flag.putout_lastmedicine2=0;
			 Putmedicine2_Info.medicine2_current_leavenum=0;		 
			 Putmedicine2_Info.medicine2_currentkeepnum-=Putmedicine2_Info.medicine2_current_outnum;	 
			 Putmedicine2_Info.medicine2_current_outnum=0;
			 Putmedicine2_Info.medicine2_current_leavenum=0;
			 Putmedicine2_Info.medicine2_putoutnum=0;
			 Putmedicine2_Info.medicine2_motor_currentoutsteps=0;
			 Putmedicine2_Info.medicine2_motor_totaloutsteps=0;
			

      }
   if(type==medicine1_putintype)
   	 {
        //  if(!system_restart_rspflag)   
	  // SPM_Respond_putmedicine(PUTIN_MEDICINE,1,Putmedicine1_Info.medicine1_current_innum);
           if(Setsolt_info.medicine_manual_putin) 
            SPM_Respond_putmedicine(PUTIN_MEDICINE,1,Putmedicine1_Info.medicine1_current_innum);
	  else
	  	 SPM_Respond(PUTIN_MEDICINE,OK);
#if SOFT_MODIFY == 1  
          //手动上药 连续执行直到手动关闭
          medicine1_putin_single_finish_nofound = 1;
#else   
	     IRF_CTRL(IRF3,STOP);
	     CLR_LED3;
	   Setmedicine1Info_val.stSetmedicine1flag.medicine1_putinstatus=0;
#endif

        Putmedicine1_Info.medicine1_currentkeepnum+=Putmedicine1_Info.medicine1_current_innum;
	   Putmedicine1_Info.medicine1_current_innum=0;
	   Putmedicine1_Info.medicine1_putinnum=0;
	   Putmedicine1_Info.medicine1_motor_currentinsteps=0;
	   Putmedicine1_Info.medicine1_motor_totalinsteps=0;


       }
   if(type==medicine2_putintype)
   	 {
	 //  if(!system_restart_rspflag)	 
           if(Setsolt_info.medicine_manual_putin)
             SPM_Respond_putmedicine(PUTIN_MEDICINE,2,Putmedicine2_Info.medicine2_current_innum);
	   else
	   	SPM_Respond(PUTIN_MEDICINE,OK);           
#if SOFT_MODIFY == 1
           //手动上药 连续执行直到手动关闭
          medicine2_putin_single_finish_nofound = 1;
#else
	   IRF_CTRL(IRF4,STOP);
	   CLR_LED4;
	   Setmedicine2Info_val.stSetmedicine2flag.medicine2_putinstatus=0;
#endif
           
	   Putmedicine2_Info.medicine2_currentkeepnum+=Putmedicine2_Info.medicine2_current_innum;
	   Putmedicine2_Info.medicine2_current_innum=0;
	   Putmedicine2_Info.medicine2_putinnum=0;
	   Putmedicine2_Info.medicine2_motor_currentinsteps=0;
	   Putmedicine2_Info.medicine2_motor_totalinsteps=0;


       }

  
  }



