/*
 * Copyright (c) 2018 Jonas Deyson
 *
 * This software is released under the MIT License.
 *
 * You should have received a copy of the MIT License
 * along with this program. If not, see <https://opensource.org/licenses/MIT>
 */

#ifndef EXPREADER_H
#define EXPREADER_H

#include <fbksd/renderer/samples.h>
#include <memory>
#include <QString>
using namespace fbksd;

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
