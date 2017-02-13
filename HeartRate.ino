#include<SD.h>
//MOSI MISO SCLK SET BY DEFAULT
//spi sd dard pins
//mosi = pin 11;
//miso =pin 12;
//sclk =pin 13;
int CS_pin=10;//chip select pin we use it when we use spi
int pow_pin=8;//power pin we have sd card attached from pin 8 to 13
              //8 is going to send 5 volt to pin 8 selecting it as digital high
//i2c temperature pins
//sda=analog pins 4
//scl =analog pin 5
int IR1_pin=2;
float refresh_rate=0.0;
long id=1;//use thhis to store the id # of our reading

void setup()
{
  Serial.begin(115200);
    pinMode(CS_pin,OUTPUT);
    //card will draw power from pin 8, so set it high
  pinMode(pow_pin,OUTPUT);
  digitalWrite(pow_pin,HIGH);
  
   if(!SD.begin(CS_pin))//begin an sd card process and check if its working or connected
  {
  
    return;
  }
  
  
  //Read the Configuration information (COMMANDS.txt)
  File commandFile = SD.open("COMMANDS.txt");
  if (commandFile)
  {
   
    
    float decade = pow(10, (commandFile.available() - 1));
    while(commandFile.available())
    {
      float temp = (commandFile.read() - '0');
      refresh_rate = temp*decade+refresh_rate;
      decade = decade/10;
    }
 
  }
 
  //write log file header
  File logFile=SD.open("LOG.csv",FILE_WRITE);
  if(logFile)
  {
    logFile.println(", , , ,");//just a leading blank line to seperate data
    String header="ID,IR1";
    logFile.println(header);
    logFile.close();
   
  }

}

 void loop()
{ 
   static int  s   = -1;    /* state                          */
  static int  pin = 13;    /* generic pin number             */
 
  int  val =  0;           /* generic value read from serial */
  int  agv =  0;           /* generic analog value           */
  int  dgv =  0;           /* generic digital value          */


  /* The following instruction constantly checks if anything 
     is available on the serial port. Nothing gets executed in
     the loop if nothing is available to be read, but as soon 
     as anything becomes available, then the part coded after 
     the if statement (that is the real stuff) gets executed */

  if (Serial.available() >0) {

    /* whatever is available from the serial is read here    */
    val = Serial.read();
    
    /* This part basically implements a state machine that 
       reads the serial port and makes just one transition 
       to a new state, depending on both the previous state 
       and the command that is read from the serial port. 
       Some commands need additional inputs from the serial 
       port, so they need 2 or 3 state transitions (each one
       happening as soon as anything new is available from 
       the serial port) to be fully executed. After a command 
       is fully executed the state returns to its initial 
       value s=-1                                            */

    switch (s) {

      /* s=-1 means NOTHING RECEIVED YET ******************* */
      case -1:      

      /* calculate next state                                */
      if (val>47 && val<90) {
	 /* the first received value indicates the mode       
           49 is ascii for 1, ... 90 is ascii for Z          
           s=0 is change-pin mode;
           s=10 is DI;  s=20 is DO;  s=30 is AI;  s=40 is AO; 
           s=90 is query script type (1 basic, 2 motor);
           s=340 is change analog reference;
           s=400 example echo returning the input argument;
                                                             */
        s=10*(val-48);
      }
      
      /* the following statements are needed to handle 
         unexpected first values coming from the serial (if 
         the value is unrecognized then it defaults to s=-1) */
      if ((s>40 && s<90) || (s>90 && s!=340 && s!=400)) {
        s=-1;
      }

      /* the break statements gets out of the switch-case, so
      /* we go back and wait for new serial data             */
      break; /* s=-1 (initial state) taken care of           */


     
      /* s=0 or 1 means CHANGE PIN MODE                      */
      
      case 0:
      /* the second received value indicates the pin 
         from abs('c')=99, pin 2, to abs('¦')=166, pin 69    */
      if (val>98 && val<167) {
        pin=val-97;                /* calculate pin          */
        s=1; /* next we will need to get 0 or 1 from serial  */
      } 
      else {
        s=-1; /* if value is not a pin then return to -1     */
      }
      break; /* s=0 taken care of                            */


      case 1:
      /* the third received value indicates the value 0 or 1 */
      if (val>47 && val<50) {
        /* set pin mode                                      */
        if (val==48) {
          pinMode(pin,INPUT);
        }
        else {
          pinMode(pin,OUTPUT);
        }
      }
      s=-1;  /* we are done with CHANGE PIN so go to -1      */
      break; /* s=1 taken care of                            */
      


      /* s=10 means DIGITAL INPUT ************************** */
      
      case 10:
      /* the second received value indicates the pin 
         from abs('c')=99, pin 2, to abs('¦')=166, pin 69    */
      if (val>98 && val<167) {
        pin=val-97;                /* calculate pin          */
        dgv=digitalRead(pin);      /* perform Digital Input  */
        Serial.println(dgv);       /* send value via serial  */
      }
      s=-1;  /* we are done with DI so next state is -1      */
      break; /* s=10 taken care of                           */

      

      /* s=20 or 21 means DIGITAL OUTPUT ******************* */
      
      case 20:
      /* the second received value indicates the pin 
         from abs('c')=99, pin 2, to abs('¦')=166, pin 69    */
      if (val>98 && val<167) {
        pin=val-97;                /* calculate pin          */
        s=21; /* next we will need to get 0 or 1 from serial */
      } 
      else {
        s=-1; /* if value is not a pin then return to -1     */
      }
      break; /* s=20 taken care of                           */

      case 21:
      /* the third received value indicates the value 0 or 1 */
      if (val>47 && val<50) {
        dgv=val-48;                /* calculate value        */
	digitalWrite(pin,dgv);     /* perform Digital Output */
      }
      s=-1;  /* we are done with DO so next state is -1      */
      break; /* s=21 taken care of                           */


	
      /* s=30 means ANALOG INPUT *************************** */
      
      case 30:
      /* the second received value indicates the pin 
         from abs('a')=97, pin 0, to abs('p')=112, pin 15    */
      if (val>96 && val<113) {
        pin=val-97;                /* calculate pin          */
        agv=analogRead(pin);       /* perform Analog Input   */
	Serial.println(agv);       /* send value via serial  */
      }
      s=-1;  /* we are done with AI so next state is -1      */
      break; /* s=30 taken care of                           */



      /* s=40 or 41 means ANALOG OUTPUT ******************** */
      
      case 40:
      /* the second received value indicates the pin 
         from abs('c')=99, pin 2, to abs('¦')=166, pin 69    */
      if (val>98 && val<167) {
        pin=val-97;                /* calculate pin          */
        s=41; /* next we will need to get value from serial  */
      }
      else {
        s=-1; /* if value is not a pin then return to -1     */
      }
      break; /* s=40 taken care of                           */


      case 41:
      /* the third received value indicates the analog value */
      analogWrite(pin,val);        /* perform Analog Output  */
      s=-1;  /* we are done with AO so next state is -1      */
      break; /* s=41 taken care of                           */
      
      
      
      /* s=90 means Query Script Type: 
         (0 adio, 1 adioenc, 2 adiosrv, 3 motor)             */
      
      case 90:
      if (val==57) { 
        /* if string sent is 99  send script type via serial */
        Serial.println(0);
      }
      s=-1;  /* we are done with this so next state is -1    */
      break; /* s=90 taken care of                           */



      /* s=340 or 341 means ANALOG REFERENCE *************** */
      
      case 340:
      /* the second received value indicates the reference,
         which is encoded as is 0,1,2 for DEFAULT, INTERNAL  
         and EXTERNAL, respectively. Note that this function 
         is ignored for boards not featuring AVR or PIC32    */
         
#if defined(__AVR__) || defined(__PIC32MX__)

      switch (val) {
        
        case 48:
        analogReference(DEFAULT);
        break;        
        
        case 49:
        analogReference(INTERNAL);
        break;        
                
        case 50:
        analogReference(EXTERNAL);
        break;        
        
        default:                 /* unrecognized, no action  */
        break;
      } 

#endif

      s=-1;  /* we are done with this so next state is -1    */
      break; /* s=341 taken care of                          */



      /* s=400 roundtrip example function (returns the input)*/
      
      case 400:
      /* the second value (val) can really be anything here  */
      
      /* This is an auxiliary function that returns the ASCII 
         value of its first argument. It is provided as an 
         example for people that want to add their own code  */
         
      /* your own code goes here instead of the serial print */
      Serial.println(val);

      s=-1;  /* we are done with the aux function so -1      */
      break; /* s=400 taken care of                          */



      /* ******* UNRECOGNIZED STATE, go back to s=-1 ******* */
      
      default:
      /* we should never get here but if we do it means we 
         are in an unexpected state so whatever is the second 
         received value we get out of here and back to s=-1  */
      
      s=-1;  /* go back to the initial state, break unneeded */



    } /* end switch on state s                               */

  } /* end if serial available    */
  
  
   int IR1_val=analogRead(IR1_pin);
   //IR1_val=(IR1_val,0,1024,0,5);
  //Serial.println(IR1_val);
  String dataString=String(id) + "," + String(IR1_val);
  //open a file to write
  //only one file can be open at a time
  File logFile=SD.open("LOG.csv",FILE_WRITE);// we have data file of type file which open on sd card log.txt and we want to write to it
  if(logFile)
  {
    logFile.println(dataString);
    logFile.close();
   
  }
 
  id++;
  delay(3);
}
