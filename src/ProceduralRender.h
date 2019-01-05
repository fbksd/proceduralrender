/*
 * Copyright (c) 2018 Jonas Deyson
 *
 * This software is released under the MIT License.
 *
 * You should have received a copy of the MIT License
 * along with this program. If not, see <https://opensource.org/licenses/MIT>
 */

#ifndef PROCEDURALRENDER_H
#define PROCEDURALRENDER_H

#include "SceneReader.h"
#include <fbksd/renderer/RenderingServer.h>
using namespace fbksd;


class ProceduralRender
{
public:
    ProceduralRender(const QString& scenefile);

    void render(const QString& resultFilename);

private:
    std::unique_ptr<Scene> scene;
    float params[NUM_RANDOM_PARAMETERS];
    float features[NUM_FEATURES];
};

#endif // PROCEDURALRENDER_H
