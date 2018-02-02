#define GREEN 0
#define RED 1

int idx=0;
int noOfGeneration = 0;
int iCount = 0;
int setCount = 2;  //8;   //higher value will speed up game generation but display gets more jittery

/*

Cell Neighboorhoods
The cells which have an influence on the state of the automate are called neighborhood.
There are two commonly used types of neighborhoods: The Moore Neighborhood and the Van Neumann Neighborhood.
The Moore neighborhood contains all neighboring cells even if they share only a cornerpoint with a cell whilst the Van Neumann Neighborhood contains only cells that share an edge with a cell.

Boundary Conditions
Cellular automata often use a toroidal topology of the simulation domain. This means that opposing edges of the grid are connected.
The rightmost column is the neighbor of the leftmost column and the topmost row is the neighbor of the bottommost row and vice versa.
This allows the unrestricted transfer of state information across the boundaries.

Another type of boundary condition treats nonexisting cells as if they all had the same state.
In the Game of Life this would mean that nonexisting cells are treated as if they were dead (as opposed to the second state "alive").
The advantage of this boundary condition in the Game of Life is that it prevents gliders from wrapping around the edges of the simulation domain.
This will prevent the destruction of a glider gun by the gliders it produces (see text below below for details about what gliders are).

In the Game of Life each grid cell can have either one of two states: dead or alive. The Game of Life is controlled by four simple rules which are applied to each grid cell in the simulation domain:

A live cell dies if it has fewer than two live neighbors.
A live cell with two or three live neighbors lives on to the next generation.
A live cell with more than three live neighbors dies.
A dead cell will be brought back to live if it has exactly three live neighbors.

Conway uses Van Neumann Neighborhood (NE,N,NW,E,SE,S,SW,W)

Can use either toroidal world or not

Still Lifes:

Block:
0, 0, 0, 0,
0, 1, 1, 0,
0, 1, 1, 0,
0, 0, 0, 0,

Beehive:
0, 0, 0, 0, 0, 0,
0, 0, 1, 1, 0, 0,
0, 1, 0, 0, 1, 0,
0, 0, 1, 1, 0, 0,
0, 0, 0, 0, 0, 0,

Loaf:
0, 0, 0, 0, 0, 0,
0, 0, 1, 1, 0, 0,
0, 1, 0, 0, 1, 0,
0, 0, 1, 0, 1, 0,
0, 0, 0, 1, 0, 0,

Oscillators:

Blinker

0, 1, 0,
0, 1, 0,
0, 1, 0,

Toad
0, 0, 0, 0, 0, 0,
0, 0, 1, 1, 1, 0,
0, 1, 1, 1, 0, 0,
0, 0, 0, 0, 0, 0,

Beacon
0, 0, 0, 0, 0,
0, 1, 1, 0, 0,
0, 1, 0, 1, 0,
0, 0, 1, 1, 0,
0, 0, 0, 0, 0,

Spaceships

The Glider
0, 0, 0, 0, 0,
0, 0, 0, 1, 0,
0, 1, 0, 1, 0,
0, 0, 1, 1, 0,
0, 0, 0, 0, 0,

Lightweight spaceship (LWSS)
0, 0, 0, 0, 0, 0, 0,
0, 0, 1, 0, 0, 1, 0,
0, 1, 0, 0, 0, 0, 0,
0, 1, 0, 0, 0, 1, 0,
0, 1, 1, 1, 1, 0, 0,
0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0,
*/



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

