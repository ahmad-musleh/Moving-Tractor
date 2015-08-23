#include <hidef.h>      /* common defines and macros */
#include <mc9s12c128.h>     /* derivative information */
#pragma LINK_INFO DERIVATIVE "mc9s12c128"


//1000 microseconds
#define MICRO1000 1920
//5 milliseconds
#define MILLI5 9950
#define FORWARDFULLSPEED 242
#define FORWARDFULLSPEED2 254
#define BACKWARDFULLSPEED 230
#define STOP 236
#define ACTUALSTOP 255

//TCNT increments that represent seconds
#define HEADER_LOW  18000
#define HEADER_HIGH 8600
#define SPACE_LOW   1320
#define BIT_ZERO    920
#define BIT_ONE     3140

// Set LED to button pressed
volatile int LED;

//find whether it is high or low
int high = 1;

//command that is recieved
int command;

//start , end and diff
int start;
int end;
int diff;

byte right;
byte left;

//function definitions
int difference (int start, int end);
void setUpOC(void);
void setUpLEDs(void);


void interrupt 9 TC1handler(void){
  //ack the interrupt
  TFLG1 |= 0x02;
    
  start = end;
  end = TCNT;
  
  diff = difference (start, end);
  
  if (diff > 8000) {
    command = 0;
  } else if (diff > 800  && diff < 1120){
    command = command << 1;
  } else if (diff > 2230 && diff < 5870){
    command = command << 1;
    command |= 1;
  } 
}

//calculate the difference in TCNT
int difference (int start, int end){
  if((end-start)<0){
    return (start+(0xFFFF-end));
  }else{
    return (end-start); 
  }
}

void refreshLEDs(void){

  //Power Button
  if (command == 0xD827){
    LED = 0x1;
  } 
  //Channel UP
  else if (command == 0xA05F){
    LED = 0x2;
    //right = 120;
    //left = 242;
    left = FORWARDFULLSPEED;
    right = 150;
  }
  //Channel Down
  else if (command == 0x00FF){
    LED = 0x4;
    right = 242;
    left = 150;
  }
  //A pressed equivalent to MUTE  
  else if (command == 0xF807){
    LED = 0x8;
  }
  //Volume Up
  else if (command == 0x807F){
    LED = 0x10;
    right = FORWARDFULLSPEED;
    left = FORWARDFULLSPEED;
  }
  //Volume Down
  else if (command == 0x10EF){
    LED = 0x20;
    right = 120;
    left = 120;
  }
  //B pressed equivalent to AV/TV
  else if (command == 0x7887){
    LED = 0x40;
  }
  //C pressed equivalent to all LEDs on
  else if (command == 0x58A7){
    LED = 0xFF;
  }
  //select is LED 8
  else if (command == 0x20DF ){
    LED = 0x80;
    right = 255;
    left = 255;
  }
  PORTA = LED;
  PWMDTY0 = right;
  PWMDTY1 = left;
}

void setUpOC(void){
  //Enable TCNT Timer
  TSCR1 = TSCR1 |= 0x80;
  //Set IOC1 to Input Compare
  TIOS  = TIOS  &  0xFD;
  //Enable IOC1 Interrupt
  TIE   = TIE   |= 0x02;
  //Timer Control Reegister Interrupt on Rising and Falling Edges
  TCTL4 = 0x0C;
  
}

void setUpLEDs(void){
  DDRA  = 0xFF;
  
}

void setupPWM (void);

void setupPWM (void){
  //Enable PWM0 
  PWME = 0x3;
  //Set polarity of PWM0;
  PWMPOL = 0x0;
  //Setting clock of PWM0  # change to 1 if needing more to scale
  PWMCLK = 0x3; //= 0x0;
  //scale the clock to account for ms
  PWMPRCLK = 0x4;
  //forward full speed
  //PWMDTY0 = 242;
  //backward full speed
  //PWMDTY1 = 120; //Really stops
  //stop moving
  //PWMDTY0 = 236;
  //PWMSCLA scaling clock SA
  PWMSCLA = 0x5;   
}


void main(void) {
  /* put your own code here */
  
  //DDRA = 0xFF;
 
  setupPWM();
  setUpLEDs();
  setUpOC();

	EnableInterrupts;


  for(;;) {
    _FEED_COP(); /* feeds the dog */
            
    refreshLEDs();
  } /* loop forever */
  /* please make sure that you never leave main */
}
