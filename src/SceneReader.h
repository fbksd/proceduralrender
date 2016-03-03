#ifndef EXPREADER_H
#define EXPREADER_H

#include <memory>
#include "QString"

namespace exprtk {
    template<typename T>
    class expression;
}

struct Scene
{
    Scene(int w, int h, int spp, std::unique_ptr<exprtk::expression<float>> exp);
    ~Scene();

    int width, height;
    int spp;

    void evaluate();

private:
    std::unique_ptr<exprtk::expression<float>> exp;
};

std::unique_ptr<Scene> readScene(const QString& filename, float* params, float* features);

#endif // EXPREADER_H
