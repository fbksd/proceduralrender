#include "sampler.h"
#include "Benchmark/RenderingServer/RenderingServer.h"


//=====================================================================
//                             PlainFor2
//=====================================================================
PlainFor2::PlainFor2(int bx, int ex, int by, int ey):
    beginX(bx), endX(ex),
    beginY(by), endY(ey),
    x(bx), y(by)
{}

bool PlainFor2::next(int &xi, int &yi)
{
    xi = x;
    yi = y;

    if(x++ >= endX || y >= endY)
    {
        if((yi = ++y) >= endY)
        {
            x = beginX;
            y = beginY;
            return false;
        }
        if(x >= endX)
        {
            xi = x = beginX;
            ++x;
        }
    }

    return true;
}


//=====================================================================
//                             PlainFor3
//=====================================================================
PlainFor3::PlainFor3(int bx, int ex, int by, int ey, int bz, int ez):
    for2(bx, ex, by, ey),
    beginZ(bz), endZ(ez),
    z(bz)
{}

bool PlainFor3::next(int &xi, int &yi, int &zi)
{
    zi = z;

    if(!for2.next(xi, yi) || z >= endZ)
    {
        if((zi = ++z) >= endZ)
        {
            z = beginZ;
            return false;
        }
        for2.next(xi, yi);
    }

    return true;
}


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
    for3(0, spp, bx, ex, by, ey)
{}

bool PixelSampler::next(float *sample)
{
    int x, y, s;
    if(!for3.next(s, x, y))
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
