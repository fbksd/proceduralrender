/*
 * Copyright (c) 2018 Jonas Deyson
 *
 * This software is released under the MIT License.
 *
 * You should have received a copy of the MIT License
 * along with this program. If not, see <https://opensource.org/licenses/MIT>
 */

#include "server.h"
#include "sampler.h"
#include <fbksd/core/SceneInfo.h>
#include <QDebug>
#include <iostream>
using namespace fbksd;


Server::Server(const QString& scenefile)
{
    memset(params, 0, NUM_RANDOM_PARAMETERS * sizeof(float));
    memset(features, 0, NUM_FEATURES * sizeof(float));

    scene = readScene(scenefile, params, features);
    if(!scene)
    {
        qDebug() << "Error reading scene file.";
        exit(EXIT_FAILURE);
    }
}

void Server::start()
{
    RenderingServer server;
    server.onSetParameters([this](const SampleLayout& layout){
        m_layout = layout;
    });

    server.onGetSceneInfo([this](){
        SceneInfo info;
        this->getSceneInfo(&info);
        return info;
    });

    server.onEvaluateSamples([this](int64_t spp, int64_t remainingCount){
        return this->evaluateSamples(spp, remainingCount);
    });

    server.run();
}

void Server::getSceneInfo(SceneInfo *sceneinfo)
{
    sceneinfo->set<int64_t>("width", scene->width);
    sceneinfo->set<int64_t>("height", scene->height);
    sceneinfo->set<int64_t>("max_spp", scene->spp);
    sceneinfo->set<int64_t>("max_samples", scene->spp * scene->width * scene->height);

    std::vector<bool> usedParamsMask(NUM_RANDOM_PARAMETERS, false);
    for(RandomParameter p: scene->randomParameters)
        usedParamsMask[p] = true;

    sceneinfo->set<bool>("has_dof", ((bool)usedParamsMask[LENS_U]) || ((bool)usedParamsMask[LENS_V]));
    sceneinfo->set<bool>("has_motion_blur", (bool)usedParamsMask[TIME]);
}

bool Server::evaluateSamples(int64_t spp, int64_t remainingCount)
{
    int64_t w = scene->width;
    int64_t h = scene->height;
    int64_t totalNumSamples = w * h * spp + remainingCount;

    std::unique_ptr<Sampler> sampler;
    if(remainingCount == 0)
        sampler = std::make_unique<PixelSampler>(0, w, 0, h, spp);
    else
        sampler = std::make_unique<SparseSampler>(0, w, 0, h, totalNumSamples);

    SamplesPipe pipe;
    render(sampler.get(), pipe);
    return true;
}

void Server::render(Sampler* sampler, SamplesPipe& pipe)
{
    while(sampler->next(params))
    {
        SampleBuffer buffer = pipe.getBuffer();

        params[0] = buffer.set(IMAGE_X, params[0]);
        params[1] = buffer.set(IMAGE_Y, params[1]);
        params[2] = buffer.set(LENS_U, params[2]);
        params[3] = buffer.set(LENS_V, params[3]);
        params[4] = buffer.set(TIME, params[4]);
        params[5] = buffer.set(LIGHT_X, params[5]);
        params[6] = buffer.set(LIGHT_Y, params[6]);

        memset(features, 0, NUM_FEATURES * sizeof(float));

        scene->evaluate();
        buffer.set(COLOR_R, features[0]);
        buffer.set(COLOR_G, features[1]);
        buffer.set(COLOR_B, features[2]);
        buffer.set(WORLD_X, features[3]);
        buffer.set(WORLD_Y, features[4]);
        buffer.set(WORLD_Z, features[5]);
        buffer.set(NORMAL_X, features[6]);
        buffer.set(NORMAL_Y, features[7]);
        buffer.set(NORMAL_Z, features[8]);
        buffer.set(TEXTURE_COLOR_R, features[9]);
        buffer.set(TEXTURE_COLOR_G, features[10]);
        buffer.set(TEXTURE_COLOR_B, features[11]);
        buffer.set(WORLD_X_1, features[12]);
        buffer.set(WORLD_Y_1, features[13]);
        buffer.set(WORLD_Z_1, features[14]);
        buffer.set(NORMAL_X_1, features[15]);
        buffer.set(NORMAL_Y_1, features[16]);
        buffer.set(NORMAL_Z_1, features[17]);
        buffer.set(TEXTURE_COLOR_R_1, features[18]);
        buffer.set(TEXTURE_COLOR_G_1, features[19]);
        buffer.set(TEXTURE_COLOR_B_1, features[20]);

        pipe << buffer;
    }
}
