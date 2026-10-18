#include "doomtype.h"
extern byte min, min2, max, max2;
extern byte rangeSplit;
extern boolean waterGenerated;
extern byte waterData[];

void R_GenerateWater();
byte R_ClampToRange(byte sample);
byte R_ClampToTwoRanges(byte sample);