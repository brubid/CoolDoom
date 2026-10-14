#include "doomtype.h"
extern byte min, max;
extern boolean waterGenerated;
extern byte waterData[];

void R_GenerateWater();
byte R_ClampToRange(byte sample);