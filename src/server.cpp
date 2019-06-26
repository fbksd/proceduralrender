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

    server.onGetTileSize([](){ return 16; });

    server.onSetParameters([this](const SampleLayout& layout){
        m_layout = layout;
    });

    server.onGetSceneInfo([this](){
        SceneInfo info;
        this->getSceneInfo(&info);
        return info;
    });

    server.onEvaluateSamples([this](int64_t spp, int64_t remainingCount, int tileSize){
        if(m_thread && m_thread->joinable())
            m_thread->join();
        m_thread = std::make_unique<std::thread>(&Server::evaluateSamples, this, spp, remainingCount, tileSize);
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

void Server::evaluateSamples(int64_t spp, int64_t remainingCount, int tileSize)
{
    int64_t w = scene->width;
    int64_t h = scene->height;
    int64_t totalNumSamples = w * h * spp + remainingCount;

    if(spp)
        render(w, h, spp);
    if(remainingCount)
        renderRemaining(w, h, spp, remainingCount, tileSize);
}

void Server::render(int w, int h, int spp)
{
    int nTilesX = std::ceil(float(w) / 16);
    int nTilesY = std::ceil(float(h) / 16);
    for(int tileY = 0; tileY < nTilesY; ++tileY)
    for(int tileX = 0; tileX < nTilesX; ++tileX)
    {
        int beginX = tileX*16;
        int beginY = tileY*16;
        int endX = std::min((tileX+1)*16, w);
        int endY = std::min((tileY+1)*16, h);
        int tileSize = (endX - beginX) * (endY - beginY);
        auto sampler = std::make_unique<PixelSampler>(beginX, endX, beginY, endY, spp);

        SamplesPipe pipe({beginX, beginY}, {endX, endY}, spp * tileSize);
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
            buffer.set(COLOR_R, features[COLOR_R]);
            buffer.set(COLOR_G, features[COLOR_G]);
            buffer.set(COLOR_B, features[COLOR_B]);
            buffer.set(DEPTH, features[DEPTH]);
            buffer.set(DIRECT_LIGHT_R, features[DIRECT_LIGHT_R]);
            buffer.set(DIRECT_LIGHT_G, features[DIRECT_LIGHT_G]);
            buffer.set(DIRECT_LIGHT_B, features[DIRECT_LIGHT_B]);
            buffer.set(WORLD_X, features[WORLD_X]);
            buffer.set(WORLD_Y, features[WORLD_Y]);
            buffer.set(WORLD_Z, features[WORLD_Z]);
            buffer.set(NORMAL_X, features[NORMAL_X]);
            buffer.set(NORMAL_Y, features[NORMAL_Y]);
            buffer.set(NORMAL_Z, features[NORMAL_Z]);
            buffer.set(TEXTURE_COLOR_R, features[TEXTURE_COLOR_R]);
            buffer.set(TEXTURE_COLOR_G, features[TEXTURE_COLOR_G]);
            buffer.set(TEXTURE_COLOR_B, features[TEXTURE_COLOR_B]);
            buffer.set(WORLD_X_1, features[WORLD_X_1]);
            buffer.set(WORLD_Y_1, features[WORLD_Y_1]);
            buffer.set(WORLD_Z_1, features[WORLD_Z_1]);
            buffer.set(NORMAL_X_1, features[NORMAL_X_1]);
            buffer.set(NORMAL_Y_1, features[NORMAL_Y_1]);
            buffer.set(NORMAL_Z_1, features[NORMAL_Z_1]);
            buffer.set(TEXTURE_COLOR_R_1, features[TEXTURE_COLOR_R_1]);
            buffer.set(TEXTURE_COLOR_G_1, features[TEXTURE_COLOR_G_1]);
            buffer.set(TEXTURE_COLOR_B_1, features[TEXTURE_COLOR_B_1]);
            buffer.set(WORLD_X_NS, features[WORLD_X_NS]);
            buffer.set(WORLD_Y_NS, features[WORLD_Y_NS]);
            buffer.set(WORLD_Z_NS, features[WORLD_Z_NS]);
            buffer.set(NORMAL_X_NS, features[NORMAL_X_NS]);
            buffer.set(NORMAL_Y_NS, features[NORMAL_Y_NS]);
            buffer.set(NORMAL_Z_NS, features[NORMAL_Z_NS]);
            buffer.set(TEXTURE_COLOR_R_NS, features[TEXTURE_COLOR_R_NS]);
            buffer.set(TEXTURE_COLOR_G_NS, features[TEXTURE_COLOR_G_NS]);
            buffer.set(TEXTURE_COLOR_B_NS, features[TEXTURE_COLOR_B_NS]);
            buffer.set(DIFFUSE_COLOR_R, features[COLOR_R]);
            buffer.set(DIFFUSE_COLOR_G, features[COLOR_G]);
            buffer.set(DIFFUSE_COLOR_B, features[COLOR_B]);

            pipe << buffer;
        }
    }
}

void Server::renderRemaining(int w, int h, int spp, int64_t n, int tileSize)
{
    int nTiles = std::ceil(float(n) / tileSize);
    for(int tile = 0; tile < nTiles; ++tile)
    {
        int numSamples = tile == 0 ? n % tileSize : tileSize;
        auto sampler = std::make_unique<SparseSampler>(0, w, 0, h, numSamples);

        SamplesPipe pipe({0, 0}, {w, h}, numSamples);
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
            buffer.set(COLOR_R, features[COLOR_R]);
            buffer.set(COLOR_G, features[COLOR_G]);
            buffer.set(COLOR_B, features[COLOR_B]);
            buffer.set(DEPTH, features[DEPTH]);
            buffer.set(DIRECT_LIGHT_R, features[DIRECT_LIGHT_R]);
            buffer.set(DIRECT_LIGHT_G, features[DIRECT_LIGHT_G]);
            buffer.set(DIRECT_LIGHT_B, features[DIRECT_LIGHT_B]);
            buffer.set(WORLD_X, features[WORLD_X]);
            buffer.set(WORLD_Y, features[WORLD_Y]);
            buffer.set(WORLD_Z, features[WORLD_Z]);
            buffer.set(NORMAL_X, features[NORMAL_X]);
            buffer.set(NORMAL_Y, features[NORMAL_Y]);
            buffer.set(NORMAL_Z, features[NORMAL_Z]);
            buffer.set(TEXTURE_COLOR_R, features[TEXTURE_COLOR_R]);
            buffer.set(TEXTURE_COLOR_G, features[TEXTURE_COLOR_G]);
            buffer.set(TEXTURE_COLOR_B, features[TEXTURE_COLOR_B]);
            buffer.set(WORLD_X_1, features[WORLD_X_1]);
            buffer.set(WORLD_Y_1, features[WORLD_Y_1]);
            buffer.set(WORLD_Z_1, features[WORLD_Z_1]);
            buffer.set(NORMAL_X_1, features[NORMAL_X_1]);
            buffer.set(NORMAL_Y_1, features[NORMAL_Y_1]);
            buffer.set(NORMAL_Z_1, features[NORMAL_Z_1]);
            buffer.set(TEXTURE_COLOR_R_1, features[TEXTURE_COLOR_R_1]);
            buffer.set(TEXTURE_COLOR_G_1, features[TEXTURE_COLOR_G_1]);
            buffer.set(TEXTURE_COLOR_B_1, features[TEXTURE_COLOR_B_1]);
            buffer.set(WORLD_X_NS, features[WORLD_X_NS]);
            buffer.set(WORLD_Y_NS, features[WORLD_Y_NS]);
            buffer.set(WORLD_Z_NS, features[WORLD_Z_NS]);
            buffer.set(NORMAL_X_NS, features[NORMAL_X_NS]);
            buffer.set(NORMAL_Y_NS, features[NORMAL_Y_NS]);
            buffer.set(NORMAL_Z_NS, features[NORMAL_Z_NS]);
            buffer.set(TEXTURE_COLOR_R_NS, features[TEXTURE_COLOR_R_NS]);
            buffer.set(TEXTURE_COLOR_G_NS, features[TEXTURE_COLOR_G_NS]);
            buffer.set(TEXTURE_COLOR_B_NS, features[TEXTURE_COLOR_B_NS]);

            pipe << buffer;
        }
    }
}
