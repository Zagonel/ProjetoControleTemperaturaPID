#include <main.h>
#include <mod_lcd.c>

#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8,STREAM=Wireless)

float TempRef, TempRes;
int16 pontDist;

char grau = 223;

float TempAnterior = 0;
float erro;
float kp = 10;
float ki = 0.2;
float kd = 0.005;
float proporcional;
float integrativo = 0;
float derivativo;
float PID;

float InserirTempRef(){

   float TempPot;
   set_adc_channel(0);
   delay_us(50);
   TempPot = read_adc() * 0.0489 + 20;
   return TempPot;
   
}

void InserirDisturbio(){

   set_adc_channel(1);
   delay_us(50);
   
   pontDist = read_adc();       
   set_pwm1_duty(pontDist);  
   delay_us(50);
}

float TempResistor(){
   float Temp;
   
   set_adc_channel(3);
   delay_us(50);   
   Temp = read_adc() * 0.489;
   
   return Temp;        
   
}

void ControlePID(){

   erro = TempRef - TempRes;
   
   proporcional = erro * kp;
   
   integrativo += erro * ki;
   
   derivativo = (TempAnterior - TempRes) * kd;
     
   TempAnterior = TempRes;
   
   PID = proporcional + derivativo + integrativo;
        
   if(PID < 1){
      PID = 0;
      integrativo = 0;
     set_pwm2_duty(PID);
   }else if(PID > 1023){
      PID = 1023;
      set_pwm2_duty(PID);
   }else{
     set_pwm2_duty((int16)PID);
   }   
}

void main(){
  
   setup_adc_ports(ALL_ANALOG);
   setup_adc(ADC_CLOCK_DIV_4);
   
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_1);
   setup_timer_1(T1_DISABLED);
   setup_timer_2(T2_DIV_BY_16,255,1);
   
   setup_ccp1(CCP_PWM);
   setup_ccp2(CCP_PWM);    
   
   lcd_ini();
   delay_ms(500);
   printf(lcd_escreve, "\fINICIANDO...");   
   delay_ms(1500); 
   
   while(TRUE){     
        
     InserirDisturbio();     
     TempRef = InserirTempRef();  
     TempRes = TempResistor();     
     ControlePID();   
     printf(lcd_escreve, "\fTEMP: %f %cC\n", TempRes,grau); 
     printf(lcd_escreve, "REF : %f %cC", TempRef,grau);
     
     fprintf(Wireless, "%f %f %f %Ld\n",TempRef,TempRes,erro,pontDist);
     
     delay_ms(1000);
        
   }

}
