// Code Example for jolliFactory's Bi-color 16X16 LED Matrix Conway's Game of Life example 1.0

// Uses 4 x jolliFactory's Bi-color LED Matrix modules driven using Arduino UNO/Nano
// Please visit http://www.instructables.com/id/Arduino-based-Bi-color-LED-Matrix-Game-of-Life/ for detail on project

#include "LedControl.h"

#define Width 16
#define Height 16

#define GREEN 0                          
#define RED 1                            


// You may change the pin assignments according to your requirements below
// First set of 2 Bi-color LED Matrices
byte devices1;
byte lc1_DataIn_Pin = 11;
byte lc1_CLK_Pin = 13;
byte lc1_Load_Pin = 10;
byte lc1_LEDMatrixQty = 2;  // No. of Bi-color LED Matrices daisy-chained

// Second set of 2 Bi-color LED Matrices
byte devices2;
byte lc2_DataIn_Pin = 11;
byte lc2_CLK_Pin = 13;
byte lc2_Load_Pin = 12;
byte lc2_LEDMatrixQty = 2;  // No. of Bi-color LED Matrices daisy-chained

//Create a new LedControl for first set of 2 Bi-color LED Matrices with 2 MAX7219 chips each
LedControl lc1=LedControl(lc1_DataIn_Pin, lc1_CLK_Pin, lc1_Load_Pin, lc1_LEDMatrixQty * 2); 

//Create another LedControl for second set of 2 Bi-color LED Matrices with 2 MAX7219 chips each
LedControl lc2=LedControl(lc2_DataIn_Pin, lc2_CLK_Pin, lc2_Load_Pin, lc2_LEDMatrixQty * 2);

byte maxInShutdown=RED; // indicates which LED Matrix color is currently off



int idx=0;
int noOfGeneration = 0;
int iCount = 0;
int setCount = 2;  //8;   //higher value will speed up game generation but display gets more jittery


/*
byte t1[16][16]  =  {{1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
                   {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}};
*/


byte t1[16][16];                      
byte t2[16][16];
byte last[16][16];



//**********************************************************************************************************************************************************
void setup() {
  Serial.begin (9600);
  Serial.println("jolliFactory - Conway's Game of Life example 1.0");              
  
  //we have already set the number of devices when we created the LedControl
  devices1=lc1.getDeviceCount();
  //we have to init all devices in a loop
  for(int address=0;address<devices1;address++) 
  {
    //The MAX72XX is in power-saving mode on startup, we have to do a WAKEUP call
    lc1.shutdown(address,false);
    // Set the brightness values (0 to 15)
    lc1.setIntensity(address,12);
    // and clear the display 
    lc1.clearDisplay(address);
  }
  
  //we have already set the number of devices when we created the LedControl
  devices2=lc2.getDeviceCount();
  //we have to init all devices in a loop
  for(int address=0;address<devices2;address++) 
  {
    //The MAX72XX is in power-saving mode on startup, we have to do a WAKEUP call
    lc2.shutdown(address,false);
    // Set the brightness values (0 to 15)
    lc2.setIntensity(address,12);
    // and clear the display
    lc2.clearDisplay(address);
  }

   randomize(t1); 
}



//**********************************************************************************************************************************************************
void loop()
{ 
   if (idx++ > 300)   //limit no. of generations for display
   {
     randomize(t1);   
     noOfGeneration = 0;     
     idx=0;
   }
     
    compute_previous_generation(t1,t2);
    compute_neighbouring_cells(t1,t2);
    compute_next_generation(t1,t2);
    display(t1);
}



//**********************************************************************************************************************************************************
void clear_window()
{  
  for(int address=0;address<devices1;address++) 
  {
    // and clear the display 
    lc1.clearDisplay(address);
  }

  for(int address=0;address<devices2;address++) 
  {
    // and clear the display
    lc2.clearDisplay(address);
  }
}



//**********************************************************************************************************************************************************
void display(byte t1[16][16])
{
  byte i,j;

  for(i=0;i<(Width/8);i++)
  {

    for(j=0;j<Height;j++)
    {  
      if (iCount > setCount){
        altShutDown();  // To perform alternate shutdown of MAX72xx chips
        iCount = 0;
      }
    
      iCount++;

      if (j<Height/2)  //Display on first set of LED Matrices
      {
        if (i<1){
          lc1.setRow(3,j,rowValue(i,j));
          lc1.setRow(2,j,rowValue_last(i,j));
        } else {
          lc1.setRow(1,j,rowValue(i*8,j));
          lc1.setRow(0,j,rowValue_last(i*8,j));
        }
      }
      else //Display on second set of LED Matrices
      {                 
        if (i<1){
          lc2.setRow(3,(j-8),rowValue(i,j));
          lc2.setRow(2,(j-8),rowValue_last(i,j));
        } else {          
          lc2.setRow(1,(j-8),rowValue(i*8,j));
          lc2.setRow(0,(j-8),rowValue_last(i*8,j));
        }
      }       

    }     
  }
  
}



//**********************************************************************************************************************************************************
int rowValue(byte i, byte j)  //rowValue(whichCol, whichRow)
{
  int result = (t1[i][j]*128) + (t1[i+1][j]*64) + (t1[i+2][j]*32) + (t1[i+3][j]*16) + (t1[i+4][j]*8) + (t1[i+5][j]*4) + (t1[i+6][j]*2) + (t1[i+7][j]*1);
  return result;
}



//**********************************************************************************************************************************************************
int rowValue_last(byte i, byte j)  //rowValue(whichCol, whichRow)
{
  int result = (last[i][j]*128) + (last[i+1][j]*64) + (last[i+2][j]*32) + (last[i+3][j]*16) + (last[i+4][j]*8) + (last[i+5][j]*4) + (last[i+6][j]*2) + (last[i+7][j]*1);
  return result;
}



//**********************************************************************************************************************************************************
void compute_previous_generation(byte t1[16][16],byte t2[16][16])
{
  byte i,j;

  for(i=0;i<Width;i++)
  {
    for(j=0;j<Height;j++)
    {
      t2[i][j]=t1[i][j];
      last[i][j]=t1[i][j];
    }
  }
}



//**********************************************************************************************************************************************************
void compute_next_generation(byte t1[16][16],byte t2[16][16])
{
  byte i,j;

  for(i=0;i<Width;i++)
  {
    for(j=0;j<Height;j++)
    {
      t1[i][j]=t2[i][j];
    }
  }
  
  noOfGeneration++;
  Serial.println(noOfGeneration);
}



//**********************************************************************************************************************************************************
void compute_neighbouring_cells(byte t1[16][16],byte t2[16][16])   //To Re-visit - does not seems correct
{
  byte i,j,a;

  for(i=0;i<Width;i++)
  {
    for(j=0;j<Height;j++)
    {
      if((i==0)&&(j==0))
      {
        a=t1[i][j+1]+t1[i+1][j]+t1[i+1][j+1]+t1[i][Height-1]+t1[i+1][Height-1]+t1[Width-1][j]+t1[Width-1][j+1]+t1[Width-1][Height-1];
      }

      if((i!=0)&&(j!=0)&&(i!=(Width-1))&&(j!=(Height-1)))
      {
        a=t1[i-1][j-1]+t1[i-1][j]+t1[i-1][j+1]+t1[i][j+1]+t1[i+1][j+1]+t1[i+1][j]+t1[i+1][j-1]+t1[i][j-1];
      }
      
      if((i==0)&&(j!=0)&&(j!=(Height-1)))
      {
        a=t1[i][j-1]+t1[i+1][j-1]+t1[i+1][j]+t1[i+1][j+1]+t1[i][j+1]+t1[Width-1][j-1]+t1[Width-1][j]+t1[Width-1][j+1];
      }

      if((i==0)&&(j==(Height-1)))
      {
        a=t1[i][j-1]+t1[i+1][j-1]+t1[i+1][j]+t1[i][0]+t1[i+1][0]+t1[Width-1][0]+t1[Width-1][j]+t1[Width-1][j-1];
      }
      
      if((i==(Width-1))&&(j==0))
      {
        a=t1[i-1][j]+t1[i-1][j+1]+t1[i][j+1]+t1[i][Height-1]+t1[i-1][Height-1]+t1[0][j]+t1[0][j+1]+t1[0][Height-1];
      }
      
      if((i==(Width-1))&&(j!=0)&&(j!=(Height-1)))
      {
        a=t1[i][j-1]+t1[i][j+1]+t1[i-1][j-1]+t1[i-1][j]+t1[i-1][j+1]+t1[0][j]+t1[0][j-1]+t1[0][j+1];
      }
      
      if((i==(Width-1))&&(j==(Height-1)))
      {
        a=t1[i][j-1]+t1[i-1][j-1]+t1[i-1][j]+t1[0][j]+t1[0][j-1]+t1[i][0]+t1[i-1][0]+t1[0][0];
      }

      if((i!=0)&&(i!=(Width-1))&&(j==0))
      {
        a=t1[i-1][j]+t1[i-1][j+1]+t1[i][j+1]+t1[i+1][j+1]+t1[i+1][j]+t1[i][Height-1]+t1[i-1][Height-1]+t1[i+1][Height-1];
      }

      if((i!=0)&&(i!=(Width-1))&&(j==(Height-1)))
      {
        a=t1[i-1][j]+t1[i-1][j-1]+t1[i][j-1]+t1[i+1][j-1]+t1[i+1][j]+t1[i][0]+t1[i-1][0]+t1[i+1][0];
      }

      if((t1[i][j]==0)&&(a==3)){t2[i][j]=1;}                   // populate if 3 neighours around it
      if((t1[i][j]==1)&&((a==2)||(a==3))){t2[i][j]=1;}         // stay alive if 2 or 3 neigbours around it
      if((t1[i][j]==1)&&((a==1)||(a==0)||(a>3))){t2[i][j]=0;}  // die if only one neighbour or over-crowding with 4 or more neighours
    }
  }
}



//**********************************************************************************************************************************************************
void randomize(byte t1[16][16])
{
  byte i,j;
  randomSeed(millis());
  for(i=0;i<Width;i++)
  {
    for(j=0;j<Height;j++)
    {
      t1[i][j]=random(2);
    }
  }
}


//**********************************************************************************************************************************************************
void altShutDown() {  //This toggles shutdown between the 2MAX72xx's in the Bi-color LED Matrix module
  if(maxInShutdown==RED){
    lc1.shutdown(0,true);    // The order here is critical - Shutdown first!
    lc1.shutdown(1,false);   // . . . Then restart the other.

    lc1.shutdown(2,true);    // The order here is critical - Shutdown first!
    lc1.shutdown(3,false);   // . . . Then restart the other.


    lc2.shutdown(0,true);    // The order here is critical - Shutdown first!
    lc2.shutdown(1,false);   // . . . Then restart the other.

    lc2.shutdown(2,true);    // The order here is critical - Shutdown first!
    lc2.shutdown(3,false);   // . . . Then restart the other.

    maxInShutdown=GREEN;
  }
  else {
    lc1.shutdown(1,true);    // The order here is critical - Shutdown first!
    lc1.shutdown(0,false);   // . . . Then restart the other.

    lc1.shutdown(3,true);    // The order here is critical - Shutdown first!
    lc1.shutdown(2,false);   // . . . Then restart the other.


    lc2.shutdown(1,true);    // The order here is critical - Shutdown first!
    lc2.shutdown(0,false);   // . . . Then restart the other.

    lc2.shutdown(3,true);    // The order here is critical - Shutdown first!
    lc2.shutdown(2,false);   // . . . Then restart the other.

    maxInShutdown=RED;
  }
}  

