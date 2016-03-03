#include "server.h"
#include "ProceduralRender.h"
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <tuple>
#include <iostream>


#define SERVER_MODE


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QStringList argList = app.arguments();
    if(argList.size() != 2)
    {
        std::cout << "Wrong number of parameters." << std::endl;
        exit(EXIT_FAILURE);
    }

    QString scenefile = argList.at(1);

#ifdef SERVER_MODE
    Server server(scenefile);
    server.start(2228);
    return app.exec();
#else
    ProceduralRender render(scenefile);
    render.render(scenefile + ".exr");
    return EXIT_SUCCESS;
#endif

}
