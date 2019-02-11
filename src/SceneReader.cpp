/*
 * Copyright (c) 2018 Jonas Deyson
 *
 * This software is released under the MIT License.
 *
 * You should have received a copy of the MIT License
 * along with this program. If not, see <https://opensource.org/licenses/MIT>
 */

#include "SceneReader.h"
#include "exprtk.hpp"
#include <QTextStream>
#include <QDebug>
#include <QFile>
#include <tuple>
#include <iostream>
#include <boost/algorithm/string.hpp>

using ExpSymbolTable = exprtk::symbol_table<float>;
using ExpExpression = exprtk::expression<float>;
using ExpParser = exprtk::parser<float>;
using ExpSymbol = ExpParser::dependent_entity_collector::symbol_t;
using ExpError = exprtk::parser_error::type;
using ExpPtr = std::unique_ptr<ExpExpression>;


//=====================================================================
//                        AUXILIARY FUNCTIONS
//=====================================================================
namespace
{

void extractInfo(const std::string infoStr, int& width, int& height, int& spp)
{
    ExpSymbolTable symbol_table;
    float w = 0.f, h = 0.f;
    float spp_f = 0.f;
    symbol_table.add_variable("width", w);
    symbol_table.add_variable("height", h);
    symbol_table.add_variable("spp", spp_f);

    ExpExpression expression;
    expression.register_symbol_table(symbol_table);

    ExpParser parser;

    if (!parser.compile(infoStr, expression))
    {
        printf("Error: %s\tExpression: %s\n",
               parser.error().c_str(),
               infoStr.c_str());

        for (std::size_t i = 0; i < parser.error_count(); ++i)
        {
            ExpError error = parser.get_error(i);
            printf("Error: %02d Position: %02d Type: [%s] Msg: %s Expr: %s\n",
                   static_cast<int>(i),
                   static_cast<int>(error.token.position),
                   exprtk::parser_error::to_str(error.mode).c_str(),
                   error.diagnostic.c_str(),
                   infoStr.c_str());
        }

        fflush(stdout);
    }

    expression.value();
    width = std::round(w);
    height = std::round(h);
    spp = std::round(spp_f);
}

std::tuple< std::unique_ptr<ExpExpression>,
            std::vector<RandomParameter>,
            std::vector<Feature> >
compileExpression(const std::string expStr, int width, int height, int spp, float* params, float* features)
{
    ExpSymbolTable symbol_table;
    symbol_table.add_constants();

    symbol_table.add_constant("width", width);
    symbol_table.add_constant("height", height);
    symbol_table.add_constant("spp", spp);

    symbol_table.add_variable("IMAGE_X", params[0]);
    symbol_table.add_variable("IMAGE_Y", params[1]);
    symbol_table.add_variable("LENS_U", params[2]);
    symbol_table.add_variable("LENS_V", params[3]);
    symbol_table.add_variable("TIME", params[4]);
    symbol_table.add_variable("LIGHT_X", params[5]);
    symbol_table.add_variable("LIGHT_Y", params[6]);

    symbol_table.add_variable("COLOR_R", features[COLOR_R]);
    symbol_table.add_variable("COLOR_G", features[COLOR_G]);
    symbol_table.add_variable("COLOR_B", features[COLOR_B]);
    symbol_table.add_variable("DEPTH", features[DEPTH]);
    symbol_table.add_variable("DIRECT_LIGHT_R", features[DIRECT_LIGHT_R]);
    symbol_table.add_variable("DIRECT_LIGHT_G", features[DIRECT_LIGHT_G]);
    symbol_table.add_variable("DIRECT_LIGHT_B", features[DIRECT_LIGHT_B]);
    symbol_table.add_variable("WORLD_X", features[WORLD_X]);
    symbol_table.add_variable("WORLD_Y", features[WORLD_Y]);
    symbol_table.add_variable("WORLD_Z", features[WORLD_Z]);
    symbol_table.add_variable("NORMAL_X", features[NORMAL_X]);
    symbol_table.add_variable("NORMAL_Y", features[NORMAL_Y]);
    symbol_table.add_variable("NORMAL_Z", features[NORMAL_Z]);
    symbol_table.add_variable("TEXTURE_COLOR_R", features[TEXTURE_COLOR_R]);
    symbol_table.add_variable("TEXTURE_COLOR_G", features[TEXTURE_COLOR_G]);
    symbol_table.add_variable("TEXTURE_COLOR_B", features[TEXTURE_COLOR_B]);
    symbol_table.add_variable("WORLD_X_1", features[WORLD_X_1]);
    symbol_table.add_variable("WORLD_Y_1", features[WORLD_Y_1]);
    symbol_table.add_variable("WORLD_Z_1", features[WORLD_Z_1]);
    symbol_table.add_variable("NORMAL_X_1", features[NORMAL_X_1]);
    symbol_table.add_variable("NORMAL_Y_1", features[NORMAL_Y_1]);
    symbol_table.add_variable("NORMAL_Z_1", features[NORMAL_Z_1]);
    symbol_table.add_variable("TEXTURE_COLOR_R_1", features[TEXTURE_COLOR_R_1]);
    symbol_table.add_variable("TEXTURE_COLOR_G_1", features[TEXTURE_COLOR_G_1]);
    symbol_table.add_variable("TEXTURE_COLOR_B_1", features[TEXTURE_COLOR_B_1]);
    symbol_table.add_variable("WORLD_X_NS", features[WORLD_X_NS]);
    symbol_table.add_variable("WORLD_Y_NS", features[WORLD_Y_NS]);
    symbol_table.add_variable("WORLD_Z_NS", features[WORLD_Z_NS]);
    symbol_table.add_variable("NORMAL_X_NS", features[NORMAL_X_NS]);
    symbol_table.add_variable("NORMAL_Y_NS", features[NORMAL_Y_NS]);
    symbol_table.add_variable("NORMAL_Z_NS", features[NORMAL_Z_NS]);
    symbol_table.add_variable("TEXTURE_COLOR_R_NS", features[TEXTURE_COLOR_R_NS]);
    symbol_table.add_variable("TEXTURE_COLOR_G_NS", features[TEXTURE_COLOR_G_NS]);
    symbol_table.add_variable("TEXTURE_COLOR_B_NS", features[TEXTURE_COLOR_B_NS]);

    std::unique_ptr<ExpExpression> expression(new ExpExpression);
    expression->register_symbol_table(symbol_table);

    ExpParser parser;
    parser.dec().collect_variables() = true;
    parser.dec().collect_assignments() = true;

    if (!parser.compile(expStr, *expression))
    {
        printf("Error: %s\tExpression: %s\n",
               parser.error().c_str(),
               expStr.c_str());

        for (std::size_t i = 0; i < parser.error_count(); ++i)
        {
            ExpError error = parser.get_error(i);
            printf("Error: %02d Position: %02d Type: [%s] Msg: %s Expr: %s\n",
                   static_cast<int>(i),
                   static_cast<int>(error.token.position),
                   exprtk::parser_error::to_str(error.mode).c_str(),
                   error.diagnostic.c_str(),
                   expStr.c_str());
        }

        fflush(stdout);
    }

    // Find list of used variables
    std::deque<ExpSymbol> symbol_list;
    parser.dec().symbols(symbol_list);
    auto newEnd = std::remove_if(symbol_list.begin(), symbol_list.end(),
        [](ExpSymbol& symbol)
        { return symbol.second != ExpParser::e_st_variable; }
    );
    symbol_list.erase(newEnd, symbol_list.end());
    // Create list of used random parameters from the list of used variables
    std::vector<RandomParameter> usedParameters;
    for (std::size_t i = 0; i < symbol_list.size(); ++i)
    {
        const ExpSymbol& symbol = symbol_list[i];
        RandomParameter p;
        if(stringToRandomParameter(boost::to_upper_copy(symbol.first), &p))
            usedParameters.push_back(p);
    }

    // Find list of variables assigned to
    symbol_list = std::deque<ExpSymbol>();
    parser.dec().assignment_symbols(symbol_list);
    // Create list of used features from the list of variables assigned to
    std::vector<Feature> usedFeatures;
    for (std::size_t i = 0; i < symbol_list.size(); ++i)
    {
        const ExpSymbol& symbol = symbol_list[i];
        Feature f;
        if(stringToFeature(boost::to_upper_copy(symbol.first), &f))
            usedFeatures.push_back(f);
    }

    return std::make_tuple(std::move(expression), usedParameters, usedFeatures);
}

}



//=====================================================================
//                            SCENE
//=====================================================================
Scene::Scene(int w, int h, int spp, ExpPtr exp, const std::vector<RandomParameter>& parameters, const std::vector<Feature>& features):
    width(w),
    height(h),
    spp(spp),
    randomParameters(parameters),
    features(features),
    exp(std::move(exp))
{}

void Scene::evaluate()
{
    exp->value();
}

Scene::~Scene() = default;



//=====================================================================
//                            READ SCENE
//=====================================================================
std::unique_ptr<Scene> readScene(const QString& filename, float* params, float* features)
{
    QString info;
    QString exp;

    QString all;
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        all = stream.readAll();

        QStringList sections = all.split("#");
        if(sections.size() != 2)
        {
            qDebug() << "Scene file should contain two sections separated by a #.";
            return nullptr;
        }

        info = sections.at(0);
        exp = sections.at(1);
    }
    else
    {
        qDebug() << "Couldn't open file " << filename;
        qDebug() << file.errorString();
        return nullptr;
    }

    int w = 0, h = 0, spp = 0;
    extractInfo(info.toStdString(), w, h, spp);

    ExpPtr expPtr;
    std::vector<RandomParameter> usedParameters;
    std::vector<Feature> usedFeatures;
    std::tie(expPtr, usedParameters, usedFeatures) = compileExpression(exp.toStdString(), w, h, spp, params, features);
    std::unique_ptr<Scene> scene(new Scene(w, h, spp, std::move(expPtr), usedParameters, usedFeatures));

    return scene;
}
