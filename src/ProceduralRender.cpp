/*
 * Copyright (c) 2018 Jonas Deyson
 *
 * This software is released under the MIT License.
 *
 * You should have received a copy of the MIT License
 * along with this program. If not, see <https://opensource.org/licenses/MIT>
 */

#include "ProceduralRender.h"
#include "SceneReader.h"
#include "sampler.h"
#include <memory>

#include <ImfOutputFile.h>
#include <ImfChannelList.h>
using namespace Imath;

namespace
{
    void saveResult(const QString& filename, float* result, int w, int h)
    {
        Imf::Header header(w, h);
        header.channels().insert ("R", Imf::Channel(Imf::FLOAT));
        header.channels().insert ("G", Imf::Channel(Imf::FLOAT));
        header.channels().insert ("B", Imf::Channel(Imf::FLOAT));

        Imf::OutputFile file(filename.toStdString().data(), header);
        Imf::FrameBuffer frameBuffer;

        frameBuffer.insert ("R", Imf::Slice( Imf::PixelType(Imf::FLOAT), (char*)result, sizeof(*result)*3, sizeof(*result)*w*3));
        frameBuffer.insert ("G", Imf::Slice( Imf::PixelType(Imf::FLOAT), (char*)(result + 1), sizeof(*result)*3, sizeof(*result)*w*3));
        frameBuffer.insert ("B", Imf::Slice( Imf::PixelType(Imf::FLOAT), (char*)(result + 2), sizeof(*result)*3, sizeof(*result)*w*3));

        file.setFrameBuffer(frameBuffer);
        file.writePixels(h);
    }
}

ProceduralRender::ProceduralRender(const QString& scenefile)
{
    scene = readScene(scenefile, params, features);
}

void ProceduralRender::render(const QString &resultFilename)
{
    if(!scene) return;

    int w = scene->width;
    int h = scene->height;
    std::unique_ptr<float[]> result(new float[w * h * 3]);
    memset(result.get(), 0, w*h*3*sizeof(float));

    PixelSampler sampler(0, scene->width, 0, scene->height, scene->spp);
    while(sampler.next(params))
    {
        memset(features, 0, 3 * sizeof(float));
        scene->evaluate();

        int x = params[0];
        int y = params[1];

        result[x*3 + y*w*3 + 0] += features[0];
        result[x*3 + y*w*3 + 1] += features[1];
        result[x*3 + y*w*3 + 2] += features[2];
    }

    float fac = 1.f / scene->spp;
    for(int y = 0; y < h; ++y)
    for(int x = 0; x < w; ++x)
    {
        result[x*3 + y*w*3 + 0] *= fac;
        result[x*3 + y*w*3 + 1] *= fac;
        result[x*3 + y*w*3 + 2] *= fac;
    }

    saveResult(resultFilename, result.get(), w, h);
}
