#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <SPI.h>

U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

const int gameCols = 126; // Indexed from 0
const int gameRows = 62; // Indexed from 0

void u8g2_prepare(void) {
  //u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFont(u8g2_font_micro_tr);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

void drawFrameFullScreen()
{
  u8g2.drawRFrame(0,0,128,64, 1);  
}

void intToChars(int i, char* charA, char* charB)
{
  char tens = (char)((i / 10)+48);
  char units = (char)((i % 10)+48);
  *charA = tens;
  *charB = units;
}

void drawDataTime(int hrI, int minI, int dayI, int mntI, int yrI)
{
  //                   0   1   2   3   4   5   6   7   8   9  10  11  12  13
  char timeStr[15] = {'1','5',':','5','3',' ','0','3','/','0','2','/','1','8',0};
  
  intToChars(hrI,&timeStr[0],&timeStr[1]); // HR
  intToChars(minI,&timeStr[3],&timeStr[4]); // HR
  intToChars(dayI,&timeStr[6],&timeStr[7]); // Day
  intToChars(mntI,&timeStr[9],&timeStr[10]); // Mnt
  intToChars(yrI, &timeStr[12],&timeStr[13]); // Mnt
  
  u8g2.drawStr(71,1,timeStr);
  //u8g2.drawStr(71,1,"15:53 03/02/18");
}

void drawCheckerBoard()
{
  byte xmax = 126;
  byte ymax = 62;
  for (byte x=0; x <xmax; x++)
  {
    for (byte y=0; y <ymax; y++)
    {
      if (x % 2 == y % 2)
      {      
        u8g2.drawPixel(x+1,y+1);
      }// end of i
    }// end for y
  }// end for x
}

void DrawCell(int row, int col)
{
  u8g2.drawPixel(1+col,1+row);
}

void draw(void) {
  u8g2_prepare();
  u8g2.setDrawColor(1); // White
  drawFrameFullScreen();
  TestDraw();
  //drawCheckerBoard();

  tmElements_t tm;
  if (RTC.read(tm)) {
    drawDataTime(tm.Hour,tm.Minute,tm.Day,tm.Month,tmYearToY2k(tm.Year));
  }
}

void PrintBytePadded(byte b)
{
  for (int i = 0; i < 8; i++)
  {
     if (b < pow(2, i))
     {
       Serial.print(B0);               
     }
  }
  Serial.println(b,BIN);
}

void setup(void) {
  Serial.begin(9600); 
  u8g2.begin();  
}

void SetBitInByte(byte& b, int index, bool v)
{
  // Index is counted from RIGHT (LSB)
  byte mask = 1 << (7-index);
  if (v)
  {
    // Any bits already set, plus the single bit set in mask...
    b = mask | b;    
  }
  else
  {
    // Any bits already set in b, and with all ones, except the one zero in mask
    b = b & ~mask;
  }
}

bool GetBitInByte(const byte& b, int index)
{
    byte mask = 1 << (7-index);
    return (mask & b) > 0; 
}


void TestBitBang()
{
  Serial.println("Hello world");
}

void TestDraw()
{
  // We want to iterate over rows and colums 
  // int r =0; r < gameRows
  // int c =0; c < gameCols
  DrawCell(0,0); // Top left col/row
  DrawCell(gameRows-1,0); // Bottom left col/row
  DrawCell(gameRows-1,gameCols-1); // Bottom right col/row
}


int count = 0;

void loop(void) {
  // picture loop  
  u8g2.clearBuffer();
  draw();
  u8g2.sendBuffer();
  
  // deley between each page
  delay(100);  

  // Delay 10 iterations to catch serial stream
  if (count <= 10)
  {
    count++;    
  }
  if (count == 10)
  {     
    TestBitBang();
  }
}
