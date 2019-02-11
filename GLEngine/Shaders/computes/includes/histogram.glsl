#define HISTOGRAM_SAMPLES 256

//=================================================================================
layout(std430, binding = 3) buffer histogram
{
    uint data_SSBO[HISTOGRAM_SAMPLES];
    uint highestValue; // 
    uint lowestIndex;
    uint highestIndex;
    uint numSamples;
};