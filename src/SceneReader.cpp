#include "SceneReader.h"
#include "exprtk.hpp"
#include <QTextStream>
#include <QDebug>
#include <QFile>
#include <tuple>

using ExpSymbolTable = exprtk::symbol_table<float>;
using ExpExpression = exprtk::expression<float>;
using ExpParser = exprtk::parser<float>;
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

std::unique_ptr<ExpExpression> compileExpression(const std::string expStr, float* params, float* features)
{
    ExpSymbolTable symbol_table;
    symbol_table.add_constants();

    symbol_table.add_constant("width", 500);
    symbol_table.add_constant("height", 500);

    symbol_table.add_variable("IMAGE_X", params[0]);
    symbol_table.add_variable("IMAGE_Y", params[1]);
    symbol_table.add_variable("LENS_U", params[2]);
    symbol_table.add_variable("LENS_V", params[3]);
    symbol_table.add_variable("TIME", params[4]);
    symbol_table.add_variable("LIGHT_X", params[5]);
    symbol_table.add_variable("LIGHT_Y", params[6]);

    symbol_table.add_variable("COLOR_R", features[0]);
    symbol_table.add_variable("COLOR_G", features[1]);
    symbol_table.add_variable("COLOR_B", features[2]);
    symbol_table.add_variable("WORLD_X", features[3]);
    symbol_table.add_variable("WORLD_Y", features[4]);
    symbol_table.add_variable("WORLD_Z", features[5]);
    symbol_table.add_variable("NORMAL_X", features[6]);
    symbol_table.add_variable("NORMAL_Y", features[7]);
    symbol_table.add_variable("NORMAL_Z", features[8]);
    symbol_table.add_variable("TEXTURE_COLOR_R", features[9]);
    symbol_table.add_variable("TEXTURE_COLOR_G", features[10]);
    symbol_table.add_variable("TEXTURE_COLOR_B", features[11]);
    symbol_table.add_variable("WORLD_X_1", features[12]);
    symbol_table.add_variable("WORLD_Y_1", features[13]);
    symbol_table.add_variable("WORLD_Z_1", features[14]);
    symbol_table.add_variable("NORMAL_X_1", features[15]);
    symbol_table.add_variable("NORMAL_Y_1", features[16]);
    symbol_table.add_variable("NORMAL_Z_1", features[17]);
    symbol_table.add_variable("TEXTURE_COLOR_R_1", features[18]);
    symbol_table.add_variable("TEXTURE_COLOR_G_1", features[19]);
    symbol_table.add_variable("TEXTURE_COLOR_B_1", features[20]);

    std::unique_ptr<ExpExpression> expression(new ExpExpression);
    expression->register_symbol_table(symbol_table);

    ExpParser parser;

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

    return expression;
}

}



//=====================================================================
//                            SCENE
//=====================================================================
Scene::Scene(int w, int h, int spp, ExpPtr exp):
    width(w),
    height(h),
    spp(spp),
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
    auto expPtr = compileExpression(exp.toStdString(), params, features);
    std::unique_ptr<Scene> scene(new Scene(w, h, spp, std::move(expPtr)));

    return scene;
}