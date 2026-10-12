#include "doomtype.h"
#include "z_zone.h"
#include "m_random.h"
#include "w_wad.h"
#include "m_fixed.h"
#include <math.h>
#include "tables.h"
#include "i_system.h"
#include "p_local.h"
//64 should be divisible by this value
#define numCellsXY 8

// generate water texture using voronoi noise with specified pallete range

typedef struct
{
	byte x, y;
} Point ;

byte* R_GenerateWater(byte min, byte max, byte* source)
{


	Point cells[numCellsXY][numCellsXY];
	for (int x = 0; x < numCellsXY; x++)
	{
		for (int y = 0; y < numCellsXY; y++)
		{
			byte a = Seed_Random(x + y * 4);
			byte b = Seed_Random((x + y * 4) * 2);
			cells[x][y].x = a;
			cells[x][y].y = b;

		}
	}
	int cellIndexX = -1, cellIndexY=0;
	fixed_t time = I_GetTime() * FRACUNIT*64;
	for (int x = 0; x < 64; x++)
	{
		cellIndexY = -1;
		if (x % (64/numCellsXY) == 0)
		{
			cellIndexX++;
		}
		for (int y = 0; y < 64; y++)
		{
			byte colIndex = 0;
			if (y % (64 / numCellsXY) == 0)
			{
				cellIndexY++;
			}
#ifdef NOFLOAT
			fixed_t minDistance = MAXINT;
#else
			float minDistance = 1.f;
#endif
			for (int x2 = -1; x2 <= 1; x2++)
			{
				
				for (int y2 = -1; y2 <= 1; y2++)
				{

					
				//	int xIndex = (cellIndexX + x2) % numCellsXY;
					int xIndex = (((cellIndexX + x2)%numCellsXY)+ numCellsXY)% numCellsXY;
				//	int yIndex = (cellIndexY + y2) % numCellsXY;
					int yIndex = (((cellIndexY + y2) % numCellsXY) + numCellsXY) % numCellsXY;

					Point offset = cells[xIndex][yIndex];
					
#ifdef NOFLOAT
					fixed_t xCoord = 16*(x * (FRACUNIT)* numCellsXY)/64;
					fixed_t yCoord = 16*(y * (FRACUNIT)*numCellsXY)/64;
					fixed_t pointXCoord = xIndex * (FRACUNIT>>0) +offset.x * (FRACUNIT >> 5);
				//	pointXCoord += finesine[(xIndex + time) >> ANGLETOFINESHIFT];

					fixed_t pointYCoord = yIndex * (FRACUNIT >>0) + offset.y * (FRACUNIT >> 5);
				//	pointYCoord += finecosine[(yIndex + time) >> ANGLETOFINESHIFT];
					fixed_t dx = pointXCoord - xCoord;
					fixed_t dy = pointYCoord - yCoord;
					fixed_t half = numCellsXY * (FRACUNIT >> 1);
					if (dx > half)
						dx = dx - half;
					if (dy > half)
						dy = dy - half;
					fixed_t distance = P_AproxDistance(dx, dy)<<0;
					if (minDistance >= distance)
						minDistance = distance;
					
					colIndex = minDistance >> FRACBITS;
#else
					float xCoord = numCellsXY * x  / 64.0f;
					float yCoord = numCellsXY * y / 64.0f;
					float pointXCoord = xIndex + (offset.x/255.0f);
					pointXCoord += 0.2f*cosf((float)(3* xIndex + SDL_GetTicks()*0.005f)*0.5f);

					float pointYCoord = yIndex  + (offset.y/255.0f);
						pointYCoord += 0.3f*cosf((float)(3* yIndex +SDL_GetTicks()*0.005f)*0.5f);
					float dx = fabsf(pointXCoord - xCoord);
					float dy = fabsf(pointYCoord - yCoord);
					float half = (float)numCellsXY /2.0f;
					if (dx > half)
						dx = numCellsXY - dx;
					if (dy > half)
						dy = numCellsXY - dy;
					float distance = ((dx * dx) + (dy * dy));

					if (minDistance >= distance)
 						minDistance = distance;

					colIndex = (byte) ((minDistance)*255 );
#endif
				}
			}
			
			colIndex = 255 - colIndex;
			colIndex= (((colIndex) * (max - min)) / 255) + min;
			source[x +(y * 64)] = colIndex; //(byte)(M_Random());



		}

	}


	return source;
}