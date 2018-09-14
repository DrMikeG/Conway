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

int boardToDraw = 0;
byte gameBoard00[bytesPerGameGrid];
byte gameBoard01[bytesPerGameGrid];

int nextFiFoSpace = 0;
int ageFifo[10];
byte encodingsFIFO[1000 * 10];
int gameStepsSinceNewRLE = 0;

const int density = 8; // n where 1/n of the cells are likely to be populated

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
  bool bitSet = false;

  for (int c=0; c < gameCols; c++){
    for (int r=0; r < gameRows; r++)
    {
      bitSet = getBitForRowCol(gameBoard, r, c);
      if (bitSet)
        {
          drawCell(r,c);                    
        }
    }
  } 
}


tmElements_t tm;

void draw(void) {
  u8g2_prepare();
  u8g2.setDrawColor(1); // White
  drawFrameFullScreen();  

  drawCurrentGameBoard();

  if (RTC.read(tm)) {    

    boolean after25March = (tm.Month > 3 || (tm.Month == 3 && tm.Day > 25));
    boolean before25Oct = (tm.Month < 10 || (tm.Month == 10 && tm.Day < 25));
    if ( after25March && before25Oct)
      tm.Hour+=1;
    drawDataTime(tm.Hour,tm.Minute,tm.Day,tm.Month,tmYearToY2k(tm.Year));
  }
}

void drawCurrentGameBoard()
{
  if (boardToDraw == 0)
  {
   drawGameBoard(gameBoard00);
  }
  else if (boardToDraw == 1)
  {
   drawGameBoard(gameBoard01);
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

//Brian Kernighan
byte countBitsInByte(byte b)
{
  byte count;
  for (count = 0; b != 0; count++)
  {
    b &= b - 1; // this clears the LSB-most set bit
  }
  return count;
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

void testBitBang()
{
  Serial.println("Hello world");

}

/** Game board managment Utils: ********************/

long countBitsInGameGrid(byte* gameBoard)
{
  long count = 0;

   // Game board is made up of bytesPerGameGrid bytes
  for (int i=0; i < bytesPerGameGrid; i++)
  {
    count+= countBitsInByte(gameBoard[i]);
  }
  return count;
}

const byte& getByteForRowCol(byte* gameBoard, int row, int col)
{
    // There are 16 bytes on each row...
    int index = (row*16)+(col/8);
    return gameBoard[index];
}

void setByteForRowCol(byte* gameBoard, int row, int col, byte b)
{
    // There are 16 bytes on each row...
    int index = (row*16)+(col/8);
    gameBoard[index] = b;
}

bool getBitForRowCol(byte* gameBoard, int row, int col)
{
  const byte& b = getByteForRowCol(gameBoard,row,col);
  // Where about in this byte is the right bit?
  int bitIndex = (col%8);
  return getBitInByte(b,bitIndex);
}

void setBitForRowCol(byte* gameBoard, int row, int col, bool val)
{
  byte b = getByteForRowCol(gameBoard,row,col);
  int index = (col % 8);
  setBitInByte(b, index, val);
  setByteForRowCol(gameBoard,row,col,b);
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

void preSetGameBoard(byte* gameBoard, int setting)
{
  zeroGameBoard(gameBoard);
  if (setting == 0)
  {
    // Seed with spinner 
     setBitForRowCol(gameBoard, 10, 10, true);
     setBitForRowCol(gameBoard, 10, 11, true);
     setBitForRowCol(gameBoard, 10, 12, true);
  }
  else if (setting == 1)
  {
    // Seed with spinner 
     setBitForRowCol(gameBoard, 10, 10, true);
     setBitForRowCol(gameBoard, 11, 10, true);
     setBitForRowCol(gameBoard, 12, 10, true);
  }
  else if (setting == 2)
  {
    // Seed with flyer
     setBitForRowCol(gameBoard, 10, 13, true);
     setBitForRowCol(gameBoard, 11, 11, true);
     setBitForRowCol(gameBoard, 11, 13, true);
     setBitForRowCol(gameBoard, 12, 12, true);
     setBitForRowCol(gameBoard, 12, 13, true);
  }
  else if (setting == 3)
  {
    // Seed with flyer
     setBitForRowCol(gameBoard, 10, 10, true);
     setBitForRowCol(gameBoard, 11, 11, true);
     setBitForRowCol(gameBoard, 12, 12, true);     
  }
  else if (setting == 4)
  {
    // Seed with flyer
     setBitForRowCol(gameBoard, 10, 10, true);
     setBitForRowCol(gameBoard, 11, 11, true);
     setBitForRowCol(gameBoard, 12, 12, true);     
  }
  else if (setting == 5)
  {
    // Seed with Glider Gun
    setBitForRowCol(gameBoard, 11,15,true);
    setBitForRowCol(gameBoard, 11,16,true);
    setBitForRowCol(gameBoard, 12,15,true);
    setBitForRowCol(gameBoard, 12,16,true);
    setBitForRowCol(gameBoard, 21,15,true);
    setBitForRowCol(gameBoard, 21,16,true);
    setBitForRowCol(gameBoard, 21,17,true);
    setBitForRowCol(gameBoard, 22,14,true);
    setBitForRowCol(gameBoard, 22,18,true);
    setBitForRowCol(gameBoard, 23,13,true);
    setBitForRowCol(gameBoard, 23,19,true);
    setBitForRowCol(gameBoard, 24,13,true);
    setBitForRowCol(gameBoard, 24,19,true);
    setBitForRowCol(gameBoard, 25,16,true);
    setBitForRowCol(gameBoard, 26,14,true);
    setBitForRowCol(gameBoard, 26,18,true);
    setBitForRowCol(gameBoard, 27,15,true);
    setBitForRowCol(gameBoard, 27,16,true);
    setBitForRowCol(gameBoard, 27,17,true);
    setBitForRowCol(gameBoard, 28,16,true);
    setBitForRowCol(gameBoard, 31,13,true);
    setBitForRowCol(gameBoard, 31,14,true);
    setBitForRowCol(gameBoard, 31,15,true);
    setBitForRowCol(gameBoard, 32,13,true);
    setBitForRowCol(gameBoard, 32,14,true);
    setBitForRowCol(gameBoard, 32,15,true);
    setBitForRowCol(gameBoard, 33,12,true);
    setBitForRowCol(gameBoard, 33,16,true);
    setBitForRowCol(gameBoard, 35,11,true);
    setBitForRowCol(gameBoard, 35,12,true);
    setBitForRowCol(gameBoard, 35,16,true);
    setBitForRowCol(gameBoard, 35,17,true);
    setBitForRowCol(gameBoard, 45,13,true);
    setBitForRowCol(gameBoard, 45,14,true);
    setBitForRowCol(gameBoard, 46,13,true);
    setBitForRowCol(gameBoard, 46,14,true);
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

void calculateNextGeneration(byte* gameBoardIn, byte* gameBoardOut)
{
  for (byte c=0; c < gameCols; c++)
  {
    for (int r=0; r < gameRows; r++)
    {
      bool outIsAlive = false;
      
      bool inIsAlive = getBitForRowCol(gameBoardIn,r,c);
      int inNeighbourCount = countNeighbouringCells(gameBoardIn,r,c);
      
      if ( inIsAlive )
      {
        switch (inNeighbourCount)
        {
          case 0:   
          case 1:
            outIsAlive = false; // Lonely :-(
            break;
          case 2:
          case 3:
            outIsAlive = true; // Content :-)
          break;
          case 4:
          case 5:
          case 6:
          case 7:
          case 8:
          outIsAlive = false; // Overcrowded :-(
            break;
        };
      }
      else
      {
         if ( inNeighbourCount == 3 ) // populate if 3 neighours around it
            outIsAlive = true;
      }

      setBitForRowCol(gameBoardOut,r,c,outIsAlive);
      
    }// end for r
  }//end of for c
}

/** Run Length encoding: *************************/

void initEncodingFIFO()
{
 //int nextFiFoSpace = 0;
  //int ageFifo[0];
  for (int i=0; i < 10; i++)
  {
    ageFifo[i] = 0;
  }
  nextFiFoSpace = 0;
  for (long l =0; l < 10000; l++)
  {
     encodingsFIFO[l] = 0;
  }
  gameStepsSinceNewRLE = 0;
}

void runLengthEncodeAndTestIfNew(byte* gameBoard)
{
  
  // Start at zero
  // How long is the run before we change from zero to 1?

  byte runs[1000];
  int runIndex = 0;
  byte currentZeroRunLength = 0;
  bool bitSet = false;

  for (int r=0; r < 1000; r++)
  runs[r] = 0;
  
  for (int c=0; c < gameCols; c++){
    for (int r=0; r < gameRows; r++)
    {
      // Run ends when we hit a one...
      bitSet = getBitForRowCol(gameBoard, r, c);
      if (bitSet || currentZeroRunLength == 254 )
      {
        // end of current run
        if (runIndex > 999)
        {
          Serial.print("Too many runs!");
          Serial.println(runIndex);
        }
        else
        {
          runs[runIndex] = currentZeroRunLength;
        }
        // reset run length
        currentZeroRunLength = 0;
        // prepare to store next run length
        runIndex++;        
      }
      else
      {
        currentZeroRunLength++;
      }
    }
  } 


  // Given this encoding, does it match any encoding in the FIFO?
  // Check all 10 FIFO entries - does this match any?
  int indexInFIFO = IsRLEInFIFO(runs);
  if (indexInFIFO == -1)
  {
    //Serial.println("RLE does not match any stored");
    gameStepsSinceNewRLE = 0;
    // 'new' entry
    if (nextFiFoSpace < 9)
    {
      writeRLEToFifo(runs,nextFiFoSpace);
      incrementAllOtherFIFOAges(nextFiFoSpace);      
      nextFiFoSpace++;
    }
    else
    {
      // Replace oldest in FIFO
      int replaceIndex = whichIsOldestFIFOEntry();
      writeRLEToFifo(runs,replaceIndex);
      incrementAllOtherFIFOAges(replaceIndex);
    }    
  }
  else
  {
    // The RLE is already in the array, set age to zero and increment all other ages...
    incrementAllOtherFIFOAges(indexInFIFO);
    gameStepsSinceNewRLE++;
  }
  /*
  
  //Serial.print("Run length encoding length:");
  //Serial.println(runIndex);
  for (int r=0; r < runIndex; r++)
  {
    Serial.print(runs[r]);
    Serial.print(',');
  }
  Serial.println();
  */  
}
void incrementAllOtherFIFOAges(int latestIndex)
{
  //Serial.print("Ages:[");
  for (int i=0; i < 10; i++)
  {
     ageFifo[i]++;
     if (i == latestIndex)
      ageFifo[i] = 0; // Most recent
      //Serial.print(ageFifo[i]);
      //Serial.print(',');
  }
  //Serial.println("]");
}

int whichIsOldestFIFOEntry()
{
  int index = 0;
  int maxAge = ageFifo[0];
  
  for (int i=1; i < 10; i++)
  {
    if (ageFifo[i] > maxAge)
    {
      index = i;
      maxAge = ageFifo[i];
    }
  }
  
  return index;
}

void writeRLEToFifo(byte* runArrayPtr, int fifoRow)
{
    //Serial.print("Writing RLE to row ");
    //Serial.println(fifoRow);
    int startIndex = (fifoRow * 1000);
    for (int i=0; i < 1000; i++)
    {
      encodingsFIFO[startIndex+i] = runArrayPtr[i];
    }
}


bool compareFifo(byte* runArrayPtr, int fifoRow)
{
    int startIndex = (fifoRow * 1000);
    bool exactMatch = true;
    for (int i=0; i < 1000; i++)
    {
      /*if (i < 10)
      {
        Serial.print(runArrayPtr[i]);
        Serial.print(" = ");
        Serial.println(encodingsFIFO[startIndex+i]);
      }*/
        if ( runArrayPtr[i] != encodingsFIFO[startIndex+i] )
        {
          //Serial.println(i);
          //Serial.print(runArrayPtr[i]);
          //Serial.print(" = ");
          //Serial.println(encodingsFIFO[startIndex+i]);
          exactMatch = false;
          break;
        }
        
    }
    return exactMatch;
}

int IsRLEInFIFO(byte* runArrayPtr)
{
  for (int i=0; i < 10; i++)
  {
    if (compareFifo(runArrayPtr,i))
    {
      return i;
    }
  }
  return -1;
}

/** Overall program logic: *************************/

void setup(void) {
  Serial.begin(9600); 
  u8g2.begin();  
  initNewGame();  
}

long staleMateCount0 = 0;
long staleMateCount1 = 0;
int nGenBeforeDeclareStaleMate = 50;
int nCurrentGenWithNoDiff = 0;

void checkForGameStaleMate()
{
  if (boardToDraw == 0)
  {
    runLengthEncodeAndTestIfNew(gameBoard00);     
  }
  else
  {
    runLengthEncodeAndTestIfNew(gameBoard01);     
  }

  if (gameStepsSinceNewRLE > 50)
  {
    Serial.println("Play again");
    initNewGame();
  }
   
  
}

void gameStep()
{
  // Caluate new board
if (true)
{
  if (boardToDraw == 0)
  {
    calculateNextGeneration(gameBoard00,gameBoard01);
    boardToDraw = 1;
  }
  else if (boardToDraw == 1)
  {
    calculateNextGeneration(gameBoard01,gameBoard00);
    boardToDraw = 0;
  }
  checkForGameStaleMate(); 
} 
}

void initNewGame()
{
  initEncodingFIFO();
  
  randomiseGameBoard(gameBoard00);
  //preSetGameBoard(gameBoard00,5);
  boardToDraw = 0;
}


void loop(void) {
  // picture loop  
  u8g2.clearBuffer();
  // Game should always be initialised
  draw();
  // Setup next step..
  gameStep();
  u8g2.sendBuffer();
  
  // deley between each page
  //delay(10);  

 
}
