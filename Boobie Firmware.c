//ARCADE SERVO
//Andrew Armstrong 25/03/2018
//www.backofficeshow.com
//Requires the XT Oscilattor jumpers being set on the Boobie Basix

#include <16F688.h>
// C Optimised for the CCS PIC Compiler
#fuses NOWDT,PUT, NOPROTECT,NOMCLR,NOFCMEN,BROWNOUT_SW, XT//NOMCLR
#use delay(clock=20MHz, crystal=20MHz) 
#use rs232(baud=57600, parity=N, xmit=PIN_C4, rcv=PIN_C5, bits=8, STOP=1)

#define version 1
#define USER_PINS 12
#define ms20Start 100

unsigned char ms20Counter = ms20Start;
unsigned char incharacter[4]; 
unsigned char charcount = 0;

unsigned char set_duties[12] = {0, 0, 255, 255, 255, 0, 0, 1, 1, 2, 3, 4};
unsigned char duties[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned char dutytarget=0;

void setpin(BYTE pin, BYTE value)
{
   switch (pin)
   {
      case 0: if (value != 0) {output_high(45);}else{output_low(45);} break;
      case 1: if (value != 0) {output_high(44);}else{output_low(44);} break;
      //case 2: if (value != 0) {output_high(43);}else{output_low(43);} break;
      //case 3: if (value != 0) {output_high(61);}else{output_low(61);} break;
      //case 4: if (value != 0) {output_high(60);}else{output_low(60);} break;
      case 5: if (value != 0) {output_high(59);}else{output_low(59);} break;
      case 6: if (value != 0) {output_high(40);}else{output_low(40);} break;
      case 7: if (value != 0) {output_high(41);}else{output_low(41);} break;
      case 8: if (value != 0) {output_high(42);}else{output_low(42);} break;
      case 9: if (value != 0) {output_high(56);}else{output_low(56);} break;
      case 10: if (value != 0) {output_high(57);}else{output_low(57);} break;
      case 11: if (value != 0) {output_high(58);}else{output_low(58);} break;
   }
}
//clone of above for the interrupt handler
void setpin_rtc(BYTE pin, BYTE value)
{
   switch (pin)
   {
      case 0: if (value != 0) {output_high(45);}else{output_low(45);} break;
      case 1: if (value != 0) {output_high(44);}else{output_low(44);} break;
      //case 2: if (value != 0) {output_high(43);}else{output_low(43);} break;
      //case 3: if (value != 0) {output_high(61);}else{output_low(61);} break;
      //case 4: if (value != 0) {output_high(60);}else{output_low(60);} break;
      case 5: if (value != 0) {output_high(59);}else{output_low(59);} break;
      case 6: if (value != 0) {output_high(40);}else{output_low(40);} break;
      case 7: if (value != 0) {output_high(41);}else{output_low(41);} break;
      case 8: if (value != 0) {output_high(42);}else{output_low(42);} break;
      case 9: if (value != 0) {output_high(56);}else{output_low(56);} break;
      case 10: if (value != 0) {output_high(57);}else{output_low(57);} break;
      case 11: if (value != 0) {output_high(58);}else{output_low(58);} break;
   }
}

BYTE readpin(BYTE pin)
{
   switch (pin)
   {
      case 0: return(input(45)); break;
      case 1: return(input(44)); break;
      case 2: return(input(43)); break;
      //case 3: return(input(61)); break;
      //case 4: return(input(60)); break;
      case 5: return(input(59)); break;
      case 6: return(input(40)); break;
      case 7: return(input(41)); break;
      case 8: return(input(42)); break;
      case 9: return(input(56)); break;
      case 10: return(input(57)); break;
      case 11: return(input(58)); break;
   }
}

void alloffImmediate()
{
   setpin(1,0);
   setpin(5,0); 
   setpin(6,0); 
   setpin(7,0); 
   setpin(8,0);
   setpin(9,0); 
   setpin(10,0);
   setpin(11,0);
}

void allonImmediate()
{
   setpin(1,1);
   setpin(5,1); 
   setpin(6,1); 
   setpin(7,1); 
   setpin(8,1);
   setpin(9,1); 
   setpin(10,1);
   setpin(11,1);
}

signed int atoi(char *s)
{
   signed int result;
   int sign, base, index;
   char c;

   index = 0;
   sign = 0;
   base = 10;
   result = 0;

   if (!s)
      return 0;
   // Omit all preceeding alpha characters
   c = s[index++];

   // increase index if either positive or negative sign is detected
   if (c == '-')
   {
      sign = 1;         // Set the sign to negative
      c = s[index++];
   }
   else if (c == '+')
   {
      c = s[index++];
   }

   if (c >= '0' && c <= '9')
   {

      // Check for hexa number
      if (c == '0' && (s[index] == 'x' || s[index] == 'X'))
      {
         base = 16;
         index++;
         c = s[index++];
      }

      // The number is a decimal number
      if (base == 10)
      {
         while (c >= '0' && c <= '9')
         {
            result = 10*result + (c - '0');
            c = s[index++];
         }
      }
      else if (base == 16)    // The number is a hexa number
      {
         c = toupper(c);
         while ( (c >= '0' && c <= '9') || (c >= 'A' && c<='F'))
         {
            if (c >= '0' && c <= '9')
               result = (result << 4) + (c - '0');
            else
               result = (result << 4) + (c - 'A' + 10);

            c = s[index++];
            c = toupper(c);
         }
      }
   }

   if (sign == 1 && base == 10)
       result = -result;

   return(result);
}

#int_timer0
void timer0_isr(void)
{  
   unsigned char i;
   
   if (ms20Counter > 0)
   {
      ms20Counter --;
      for (i=0; i<12; i++)
      {
         if (duties[i]>0)
         {
            duties[i]--;
         }else{
            setpin_rtc(i,0);
         }
      }
   }else{
      //Reset the Duty cycle
      for (i=0; i<12; i++)
      {
         duties[i] = set_duties[i];
         setpin_rtc(i,1);
      }
      ms20Counter = ms20Start;
   }
}

//Serial Interrupt
#INT_RDA
void SerialInt()
{
   unsigned char buf[2];
   unsigned char pin=0;
   unsigned char val=0;
   unsigned char ignore=0;
   
   incharacter[charcount]=getchar();
   
   switch(incharacter[charcount])
   {
      case 'q': dutytarget=1; break;
      case 'w': dutytarget=3; break;
      case 'e': dutytarget=5; break;
      case 'r': dutytarget=7; break;
      case 't': dutytarget=10; break;
      case 'y': dutytarget=11; break;
      case 'u': dutytarget=13; break;
      case 'i': dutytarget=15; break;
      case 'o': dutytarget=17; break;
      case 'p': dutytarget=20; break;      
      case 'a': dutytarget=80; break;
      case '\n': ignore = 1; break;
      case '\r': ignore = 1; break;
   }
   
   if (dutytarget == 0 && ignore == 0)
   {
      charcount++;
      if (charcount == 4)
      {
         printf("!"); 
         charcount=0;
         
         buf[0]=incharacter[0];
         buf[1]=incharacter[1];
         pin = atoi(buf);
         buf[0]=incharacter[2];
         buf[1]=incharacter[3];
         val = atoi(buf);
         set_duties[pin-4]=val;
      }
   }
   else
   {
      charcount=0; //Reset buffer, something else is going to process the duty
   }
}

void main()
{
   unsigned char counter=0;
   unsigned char funloop=0;
   unsigned char funcounter=0;
   alloffImmediate();
   setup_timer_0 ( RTCC_INTERNAL | RTCC_8_BIT | RTCC_DIV_2 ); 
   enable_interrupts(INT_TIMER0);
   enable_interrupts(INT_RDA);
   enable_interrupts(GLOBAL);

   printf("\r\nBoobie Servo v%d\r\nAndrew Armstrong @backofficeshow\r\n", version);

   while (TRUE)
   {
      if (dutytarget != 0)
      {
         //Regular stuff happens under 80
         if (dutytarget < 80)
         {
            for (counter=0; counter<12; counter++)
            {
               set_duties[counter] = dutytarget;
            } 
         }
         else //However over 80 the fun stuff begins!!
         {
            funcounter = 0;
            for (funloop=0; funloop<5; funloop++)
            {
               for (counter=5; counter<12; counter++)
               {
                  
                  set_duties[counter] = funcounter;
                  funcounter = funcounter + 1;
                  if (funcounter > 18)
                  {
                     funcounter = 3;
                  }
                  delay_ms(50);
               } 
            }
         }
         dutytarget=0;
      }
   }   
} 

