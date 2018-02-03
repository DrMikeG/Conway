#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <SPI.h>

U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

const int gameCols = 126; // Indexed from 0
const int gameRows = 62; // Indexed from 0
// 126 pixels per row = 16 bytes (16 * 8 = 128 bits)
// 62 rows *  16 bytes = 992 bytes per game grid...
const int bytesPerGameGrid = 16 * gameRows; 
byte gameBoard00[bytesPerGameGrid];



/** Drawing functions: ******************************/

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
  u8g2.setDrawColor(0); // Black
  u8g2.drawBox(70,1,57,7);  
  u8g2.setDrawColor(1); // White
  char timeStr[15] = {'1','5',':','5','3',' ','0','3','/','0','2','/','1','8',0};
  intToChars(hrI,&timeStr[0],&timeStr[1]); // HR
  intToChars(minI,&timeStr[3],&timeStr[4]); // HR
  intToChars(dayI,&timeStr[6],&timeStr[7]); // Day
  intToChars(mntI,&timeStr[9],&timeStr[10]); // Mnt
  intToChars(yrI, &timeStr[12],&timeStr[13]); // Mnt  
  u8g2.drawStr(71,1,timeStr);
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

void drawCell(int row, int col)
{
  u8g2.drawPixel(1+col,1+row);
}

void drawGameBoard(byte* gameBoard)
{
  // Game board is made up of bytesPerGameGrid bytes
  int r=0;
  int c=0;
  bool bitSet = false;
  for (int i=0; i < bytesPerGameGrid; i++)
  {
      byte b = gameBoard[i]; 
      for (int j=0; j < 8; j++)
      {
        bitSet = getBitInByte(b,j);
        if (bitSet)
        {
          drawCell(r,c);                    
        }
        c++;
          if (c >= gameCols)
          {
            c = 0;
            r++;
          }        
      }
  }
}

void draw(void) {
  u8g2_prepare();
  u8g2.setDrawColor(1); // White
  drawFrameFullScreen();  
  //drawCheckerBoard();
  testDraw();
  
  tmElements_t tm;
  if (RTC.read(tm)) {    
    drawDataTime(tm.Hour,tm.Minute,tm.Day,tm.Month,tmYearToY2k(tm.Year));
  }
}



void testDraw()
{
  // We want to iterate over rows and colums 
  // int r =0; r < gameRows
  // int c =0; c < gameCols
  randomiseGameBoard(gameBoard00);
  drawGameBoard(gameBoard00);
}

/*** Bit storage utils: *********************************************/

void PrintBytePadded(const byte& b)
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

void setBitInByte(byte& b, int index, bool v)
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

bool getBitInByte(const byte& b, int index)
{
    byte mask = 1 << (7-index);
    return (mask & b) > 0; 
}

void testPrintRCPair(int r, int c)
{
  Serial.print("[");
  if (c<100)  
    Serial.print("0");
  if (c<10)  
    Serial.print("0");
  Serial.print(c);
  Serial.print(",");
  if (r<100)  
    Serial.print("0");
  if (r<10)  
    Serial.print("0");
  Serial.print(r);
  Serial.print("]");
}

void testPrintRCPairs(int* rc, int* cs)
{
  testPrintRCPair(rc[0],cs[0]);
  testPrintRCPair(rc[1],cs[1]);
  testPrintRCPair(rc[2],cs[2]);
  Serial.println();
  testPrintRCPair(rc[3],cs[3]);
  Serial.print("[   ,   ]");
  testPrintRCPair(rc[4],cs[4]);
  Serial.println();
  /*    [CCC,RRR][CCC,RRR][CCC,RRR]
   *    [CCC,RRR][   ,   ][CCC,RRR]
   *    [CCC,RRR][CCC,RRR][CCC,RRR]
   */    
  testPrintRCPair(rc[5],cs[5]);
  testPrintRCPair(rc[6],cs[6]);
  testPrintRCPair(rc[7],cs[7]);
  Serial.println();
}

void testBitBang()
{
  Serial.println("Hello world");

  // Test void generateNeighbouringCellLocations(const int r, const int c, int* rs, int* cs)
  int rs[8];
  int cs[8];
  Serial.println("Test [1,1]:");
  generateNeighbouringCellLocations(1,1,rs,cs);
  testPrintRCPairs(rs,cs);

  // Row 1, col 0
  Serial.println("Test [1,0]:");
  generateNeighbouringCellLocations(1,0,rs,cs);
  testPrintRCPairs(rs,cs);

  // Row 0, col 1
  Serial.println("Test [0,1]:");
  generateNeighbouringCellLocations(0,1,rs,cs);
  testPrintRCPairs(rs,cs);

  // Row 1, col 126
  Serial.println("Test [1,125]:");
  generateNeighbouringCellLocations(1,125,rs,cs);
  testPrintRCPairs(rs,cs);

  // Row 61, col 126
  Serial.println("Test [61,125]:");
  generateNeighbouringCellLocations(61,125,rs,cs);
  testPrintRCPairs(rs,cs);

}

/** Game board managment Utils: ********************/

const byte& getByteForRowCol(byte* gameBoard, int row, int col)
{
    // There are 16 bytes on each row...
    int index = (row*16)+(col/8);
    return gameBoard[index];
}

bool getBitForRowCol(byte* gameBoard, int row, int col)
{
  const byte& b = getByteForRowCol(gameBoard,row,col);
  // Where about in this byte is the right bit?
  int bitIndex = (col%8);
  return getBitInByte(b,bitIndex);
}

void zeroGameBoard(byte* gameBoard)
{
  // Game board is made up of bytesPerGameGrid bytes
  for (int i=0; i < bytesPerGameGrid; i++)
  {
   gameBoard[i] = 0; 
  }
}

void randomiseGameBoard(byte* gameBoard)
{
  randomSeed(millis());
  int density = 4; // n where 1/n of the cells are likely to be populated
  for (int i=0; i < bytesPerGameGrid; i++)
  {
    byte randByte = 0;
    for (int j=0; j < 8; j++)
    {
      setBitInByte(randByte,j,random(density)==0);
    }
    gameBoard[i] = randByte;
  }  
}

void generateNeighbouringCellLocations(const int r, const int c, int* rs, int* cs)
{
  int i=0;
   for (int y=-1; y<2; y++)
    {
  for (int x=-1; x < 2; x++)
  {
   
        if (x==0 && y==0) // Skip Center
          continue;  

        // Standard values for non-center
        int nC = c+x;
        int nR = r+y;
        // Check for left/right wrap...
        if (nC == -1)
        {
          nC = gameCols-1; // Last valid value
        }
        else if (nC == gameCols) // Invalid value
        {
          nC = 0;
        }
        // Check for top/bottom wrap...
        if (nR == -1)
        {
          nR = gameRows-1; // Last valid value
        }
        else if (nR == gameRows) // Invalid value
        {
          nR = 0;
        }
        // Record pair;
        cs[i]=nC;
        rs[i]=nR;
        i++;
    }
  }
}

int countNeighbouringCells(byte* gameBoard, int r, int c)
{
  // Using toriodal method
  // Using diagonal neighbours
  int count = 0;

  int neighbourRs[8];
  int neighbourCs[8];
  generateNeighbouringCellLocations(r,c,neighbourRs,neighbourCs);  
  for (int n=0; n < 8; n++)
  {
    bool isSet = getBitForRowCol(gameBoard,neighbourRs[n],neighbourCs[n]);
    if (isSet)
      count++;
  }
  
  return count;
}

int calculateNextGeneration(byte* gameBoardIn, byte* gameBoardOut)
{
  return -1;
}

/** Overall program logic: *************************/

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
    testBitBang();
  }
}
