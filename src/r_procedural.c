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
#define NOFLOAT 1
// generate water texture using voronoi noise with specified pallete range

typedef struct
{
	byte x, y;
} Point ;

byte* R_GenerateWater(byte min, byte max, byte* source)
{

	// try to generate a general voronoi and store
	// then copy and recolor as needed for liquid textures
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
	int cellIndexX = -1, cellIndexY=-1;
	fixed_t time = I_GetTime() * FRACUNIT*8;
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
			fixed_t minDistance = FRACUNIT;
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
					fixed_t xCoord = (x * (FRACUNIT)*numCellsXY)>>6;
					fixed_t yCoord = (y * (FRACUNIT)*numCellsXY)>>6;
					fixed_t pointXCoord = xIndex * (FRACUNIT) + (offset.x * (FRACUNIT))/255;
					angle_t angleX = ((time * 32 + 1024 * pointXCoord<<22 /numCellsXY));
					
						pointXCoord += (finesine[angleX >> ANGLETOFINESHIFT])>>2;

					fixed_t pointYCoord = yIndex * (FRACUNIT) + (offset.y * (FRACUNIT))/255;
					angle_t angleY = ((time * 32 + 1024*pointYCoord<<22 / numCellsXY));
						pointYCoord += (finecosine[angleY >> ANGLETOFINESHIFT]) >> 2;
					fixed_t dx = pointXCoord - xCoord;
					if (pointXCoord < xCoord)
						dx = xCoord - pointXCoord;
					fixed_t dy = pointYCoord - yCoord;
					if (pointYCoord < yCoord)
						dy = yCoord - pointYCoord;
					fixed_t half = (numCellsXY * (FRACUNIT))>>1;
					if (dx > half)
						dx = (FRACUNIT*numCellsXY) - dx;
					if (dy > half)
						dy = (FRACUNIT * numCellsXY) - dy;
					fixed_t distance = FixedMul(dx, dx) + FixedMul(dy, dy);//P_AproxDistance(dx, dy)<<0;
					if (minDistance > distance)
						minDistance = distance;
					
					colIndex = (minDistance*255) >> FRACBITS;
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