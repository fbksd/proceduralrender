#include "sampler.h"
#include "Benchmark/RenderingServer/RenderingServer.h"


//=====================================================================
//                             Sampler
//=====================================================================
Sampler::Sampler(int bx, int ex, int by, int ey):
    beginX(bx), endX(ex),
    beginY(by), endY(ey)
{}


//=====================================================================
//                             PixelSampler
//=====================================================================
PixelSampler::PixelSampler(int bx, int ex, int by, int ey, int spp):
    Sampler(bx, ex, by, ey),
    spp(spp),
    for3(by, ey, bx, ex, 0, spp)
{}

bool PixelSampler::next(float *sample)
{
    int x, y, s;
    if(!for3.next(y, x, s))
        return false;

    for(int i = 0; i < NUM_RANDOM_PARAMETERS; ++i)
        sample[i] = randFloat();

    sample[0] += x;
    sample[1] += y;

    // Check limits to a void sample spilling in another pixel
    if(((int)sample[0]) < x)
        sample[0] = x + 0.0001f;
    else if(((int)sample[0]) > x)
        sample[0] = (x + 1) - 0.0001f;
    if(((int)sample[1]) < y)
        sample[1] = y + 0.0001f;
    else if(((int)sample[1]) > y)
        sample[1] = (y + 1) - 0.0001f;

    return true;
}


//=====================================================================
//                             SparseSampler
//=====================================================================
SparseSampler::SparseSampler(int bx, int ex, int by, int ey, int n):
    Sampler(bx, ex, by, ey),
    nSamples(n),
    s(0)
{}

bool SparseSampler::next(float *sample)
{
    if(s++ == nSamples)
        return false;

    for(int i = 0; i < NUM_RANDOM_PARAMETERS; ++i)
        sample[i] = randFloat();

    sample[0] *= endX - beginX;
    sample[1] *= endY - beginY;

    // Check limits
    if(((int)sample[0]) < beginX)
        sample[0] = beginX + 0.0001f;
    else if(((int)sample[0]) >= endX)
        sample[0] = endX - 0.0001f;
    if(((int)sample[1]) < beginY)
        sample[1] = beginY + 0.0001f;
    else if(((int)sample[1]) >= endY)
        sample[1] = endY - 0.0001f;

    return true;
}
