#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

// X,Y for screen position
// C,R for position on gameboard

const int gameCols = 63; // Indexed from 0
const int gameRows = 31; // Indexed from 0
const int gameBytesPerRow = 8; // 31bits < 32bits = 8 bytes
const int gameBytesRequired = gameBytesPerRow*gameRows; // 31 * 8 = 248 bytes
unsigned char game_bits_00[ gameBytesRequired ];

void u8g2_prepare(void) {
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

uint8_t draw_state = 0;

void drawFrameFullScreen()
{
  u8g2.drawRFrame(0,0,128,64, 1);  
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

void drawCorners()
{
  byte xmin = 1;
  byte xmax = 126;
  byte ymin = 1;
  byte ymax = 62;
  u8g2.drawPixel(xmin,ymin);
  u8g2.drawPixel(xmin,ymax);
  u8g2.drawPixel(xmax,ymin);
  u8g2.drawPixel(xmax,ymax);
}

void drawPixel2by2(byte r, byte c)
{
  u8g2.drawPixel(1+(c*2),1+(r*2));
  u8g2.drawPixel(2+(c*2),1+(r*2));
  u8g2.drawPixel(1+(c*2),2+(r*2));
  u8g2.drawPixel(2+(c*2),2+(r*2));
}

void drawGameBoard(uint8_t *bitmap)
{
  for (byte c=0; c <gameCols; c++)
  {
    for (byte r=0; r <gameRows; r++)
    {
      byte byteIndex = 0;
      byte bitIndex  = 0;
      getGameBoardByteIndex(r, c, byteIndex, bitIndex);
      bool b = getGameBoardBit(bitmap,byteIndex, bitIndex);
      if (b)
        drawPixel2by2(r,c);
      //if (r % 2 == c %2)
        //drawPixel2by2(r,c);
    }
  }
}

bool getGameBoardBit(uint8_t *bitmap, byte byteIndex, byte bitIndex)
{
  byte operand = 0;
  switch(bitIndex)
  {
    case 0:
    operand = B00000001;
    break;
    case 1:
    operand = B00000010;
    break;
    case 2:
    operand = B00000100;
    break;
    case 3:
    operand = B00001000;
    break;
    case 4:
    operand = B00010000;
    break;
    case 5:
    operand = B00100000;
    break;
    case 6:
    operand = B01000000;
    break;
    case 7:
    operand = B10000000;
    break;
  }
  return operand & bitmap[byteIndex] > 0;
}

void setGameBoardBit(uint8_t *bitmap, byte byteIndex, byte bitIndex, bool val)
{
  byte b = bitmap[byteIndex];
  byte operand = 0;
  switch(bitIndex)
  {
    case 0:
    operand = B00000001;
    break;
    case 1:
    operand = B00000010;
    break;
    case 2:
    operand = B00000100;
    break;
    case 3:
    operand = B00001000;
    break;
    case 4:
    operand = B00010000;
    break;
    case 5:
    operand = B00100000;
    break;
    case 6:
    operand = B01000000;
    break;
    case 7:
    operand = B10000000;
    break;
  }

  if (!val)
  {
    operand = ~operand;
  }
  bitmap[byteIndex] = b & operand;  
}

void getGameBoardByteIndex(byte r, byte c, byte& byteIndex, byte& bitIndex)
{  
  byteIndex       =   ( gameBytesPerRow*r ) + (c/8);
  bitIndex        =   ( c % 8);  
}

void populateGameBoard00With(bool val)
{
  // Set every bit value in all the bytes of game_bits_00 to val
  for (byte c=0; c <gameCols; c++)
  {
    for (byte r=0; r <gameRows; r++)
    {
      byte byteIndex = 0;
      byte bitIndex  = 0;
      getGameBoardByteIndex(r, c, byteIndex, bitIndex);
      setGameBoardBit(game_bits_00,byteIndex, bitIndex,val);
    }// end for r
  }// end for c    
}



void draw(void) {
  u8g2_prepare();

  u8g2.setDrawColor(1); // White
  drawFrameFullScreen();
  //drawCorners();
  //drawCheckerBoard();
  //populateGameBoard00With(true);
  //drawGameBoard(game_bits_00);
  // x y radius
  //u8g2.drawCircle(10,10,1); 
  //u8g2.drawDisc(20,20,1); 
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
  
  // increase the state
  draw_state++;
  if ( draw_state >= 12*8 )
    draw_state = 0;

  // deley between each page
  delay(100);

}
