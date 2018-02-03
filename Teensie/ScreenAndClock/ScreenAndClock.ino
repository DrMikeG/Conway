#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <SPI.h>


U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);


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


void draw(void) {
  u8g2_prepare();
  u8g2.setDrawColor(1); // White
  drawFrameFullScreen();
  //drawCheckerBoard();

  tmElements_t tm;
  if (RTC.read(tm)) {
    drawDataTime(tm.Hour,tm.Minute,tm.Day,tm.Month,tmYearToCalendar(tm.Year));
  }
}


void setup(void) {
  Serial.begin(9600); 
  u8g2.begin();  

}

void loop(void) {
  // picture loop  
  u8g2.clearBuffer();
  draw();
  u8g2.sendBuffer();
  
  // deley between each page
  delay(100);

}
