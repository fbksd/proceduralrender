#ifndef EXPREADER_H
#define EXPREADER_H

#include "Benchmark/RenderingServer/RenderingServer.h"
#include <memory>
#include "QString"

namespace exprtk {
    template<typename T>
    class expression;
}

struct Scene
{
    Scene(int w, int h, int spp, std::unique_ptr<exprtk::expression<float>> exp, const std::vector<RandomParameter>& parameters, const std::vector<Feature>& features);
    ~Scene();

    int width, height;
    int spp;
    const std::vector<RandomParameter> randomParameters;
    const std::vector<Feature> features;

    void evaluate();

private:
    std::unique_ptr<exprtk::expression<float>> exp;
};

std::unique_ptr<Scene> readScene(const QString& filename, float* params, float* features);

#endif // EXPREADER_H
