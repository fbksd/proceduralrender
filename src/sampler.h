/*
 * Copyright (c) 2018 Jonas Deyson
 *
 * This software is released under the MIT License.
 *
 * You should have received a copy of the MIT License
 * along with this program. If not, see <https://opensource.org/licenses/MIT>
 */

#ifndef SAMPLER_H
#define SAMPLER_H

#include "plain_for.h"
#include <random>


inline float randFloat()
{
    static std::mt19937 gen;
    static std::uniform_real_distribution<float> dist(0, 1);
    return dist(gen);
}


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
    PlainFor<3> for3;
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
