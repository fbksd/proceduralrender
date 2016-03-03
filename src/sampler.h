#ifndef SAMPLER_H
#define SAMPLER_H

#include <random>


inline float randFloat()
{
    static std::mt19937 gen;
    static std::uniform_real_distribution<float> dist(0, 1);
    return dist(gen);
}


/**
 *  for(int y = beginY; y < endY; ++y)
 *  for(int x = beginX; x < endX; ++x)
 */
class PlainFor2
{
public:
    PlainFor2(int bx, int ex, int by, int ey);

    bool next(int& xi, int& yi);

private:
    int beginX, endX;
    int beginY, endY;
    int x, y;
};


/**
 *  for(int z = beginZ; y < endZ; ++z)
 *  for(int y = beginY; y < endY; ++y)
 *  for(int x = beginX; x < endX; ++x)
 */
class PlainFor3
{
public:
    PlainFor3(int bx, int ex, int by, int ey, int bz, int ez);

    bool next(int& xi, int& yi, int& zi);

private:
    PlainFor2 for2;
    int beginZ, endZ;
    int z;
};


class Sampler
{
public:
    // range is [begin, end)
    Sampler(int bx, int ex, int by, int ey);

    virtual ~Sampler() = default;

    virtual bool next(float* sample) = 0;

    int beginX, endX;
    int beginY, endY;
};


class PixelSampler: public Sampler
{
public:
    PixelSampler(int bx, int ex, int by, int ey, int spp);

    virtual bool next(float* sample);

private:
    int spp;
    PlainFor3 for3;
};


class SparseSampler: public Sampler
{
public:
    SparseSampler(int bx, int ex, int by, int ey, int n);

    virtual bool next(float* sample);

private:
    int nSamples;
    int s;
};

#endif // SAMPLER_H
