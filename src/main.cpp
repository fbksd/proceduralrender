/*
 * Copyright (c) 2018 Jonas Deyson
 *
 * This software is released under the MIT License.
 *
 * You should have received a copy of the MIT License
 * along with this program. If not, see <https://opensource.org/licenses/MIT>
 */

#include "server.h"
#include "ProceduralRender.h"
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <tuple>
#include <iostream>
#include <clocale>


int main(int argc, char *argv[])
{
    setlocale(LC_NUMERIC,"C");
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("A procedural renderer.");
    QCoreApplication::setApplicationVersion("0.1");
    QCommandLineParser parser;
    parser.setApplicationDescription("A renderer that uses mathematical expressions as scenes." );
    parser.addHelpOption();
    parser.addPositionalArgument("scene", "Scene to render.");
    QCommandLineOption outputOption({"output", "o"}, "Output file name. (if the file already exists, it well be overwritten)", "path");
    parser.addOption(outputOption);

    parser.process(app);
    auto args = parser.positionalArguments();
    if(args.size() != 1)
        parser.showHelp(1);
    QString scenefile = args[0];

    QString outfile;
    if(parser.isSet(outputOption))
        outfile = parser.value(outputOption);
    else
        outfile = QDir::currentPath() + '/' + QFileInfo(scenefile).baseName() + ".exr";

#ifdef SERVER_MODE
    Server server(scenefile);
    server.start();
    return EXIT_SUCCESS;
#else
    ProceduralRender render(scenefile);
    render.render(outfile);
    return EXIT_SUCCESS;
#endif

}
