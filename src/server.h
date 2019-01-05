/*
 * Copyright (c) 2018 Jonas Deyson
 *
 * This software is released under the MIT License.
 *
 * You should have received a copy of the MIT License
 * along with this program. If not, see <https://opensource.org/licenses/MIT>
 */

#ifndef SERVER_H
#define SERVER_H

#include "SceneReader.h"
#include <fbksd/renderer/RenderingServer.h>
#include <memory>
using namespace fbksd;

class Sampler;

class Server
{
public:
    Server(const QString& scenefile);

    void start();

private:
    void getSceneInfo(SceneInfo* scene);
    bool evaluateSamples(int64_t spp, int64_t remainingCount);
    void render(Sampler* sampler, SamplesPipe& pipe);

    std::unique_ptr<Scene> scene;
    float params[NUM_RANDOM_PARAMETERS];
    float features[NUM_FEATURES];
    SampleLayout m_layout;
};

#endif // SERVER_H
