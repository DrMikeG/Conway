const byte gameBytesPerRow = 8; // 31bits < 32bits = 4 bytes

void getGameBoardByteIndex(byte r, byte c, byte& byteIndex, byte& bitIndex)
{  
  // Which one of 1953 pixels? (r*4)+c  
  // With right hand padding to 128 pixels per row, there are 16 bytes per line
  // So it's more useful to calculate the byte based on the row first
  // Each row of 126 pixels uses 7 whole bytes  
  
  byteIndex       =   ( gameBytesPerRow*r ) + (c/8);  // Which one of 124 bytes?
  bitIndex        =   7 - ( c % 8);  
}


void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600); 
byte byteIndex = 0;
byte bitIndex = 0;
Serial.println("          r    c    byte =");
getGameBoardByteIndex(  1,    19, byteIndex, bitIndex);Serial.print("Test A     1    19     10 =");Serial.println(byteIndex);
getGameBoardByteIndex(  2,     4, byteIndex, bitIndex);Serial.print("Test B     2     4     16 =");Serial.println(byteIndex);
getGameBoardByteIndex(  4,    39, byteIndex, bitIndex);Serial.print("Test C     4    39     36 =");Serial.println(byteIndex);
getGameBoardByteIndex(  4,    46, byteIndex, bitIndex);Serial.print("Test D     4    46     37 =");Serial.println(byteIndex);
getGameBoardByteIndex(  9,    26, byteIndex, bitIndex);Serial.print("Test E     9    26     75 =");Serial.println(byteIndex);
getGameBoardByteIndex(  9,    51, byteIndex, bitIndex);Serial.print("Test F     9    51     78 =");Serial.println(byteIndex);
getGameBoardByteIndex( 13,    11, byteIndex, bitIndex);Serial.print("Test G    13    11    105 =");Serial.println(byteIndex);
getGameBoardByteIndex( 16,    57, byteIndex, bitIndex);Serial.print("Test H    16    57    135 =");Serial.println(byteIndex);
getGameBoardByteIndex( 18,    41, byteIndex, bitIndex);Serial.print("Test I    18    41    149 =");Serial.println(byteIndex);
getGameBoardByteIndex( 19,    31, byteIndex, bitIndex);Serial.print("Test J    19    31    155 =");Serial.println(byteIndex);
getGameBoardByteIndex( 21,     3, byteIndex, bitIndex);Serial.print("Test K    21     3    168 =");Serial.println(byteIndex);

}

void loop() {
  // put your main code here, to run repeatedly:

}
