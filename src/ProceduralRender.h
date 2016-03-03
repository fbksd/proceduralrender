#ifndef PROCEDURALRENDER_H
#define PROCEDURALRENDER_H

#include "SceneReader.h"
#include "Benchmark/RenderingServer/RenderingServer.h"


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
