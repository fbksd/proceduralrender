#include "server.h"
#include "sampler.h"
#include <QCoreApplication>
#include <QDebug>
#include <iostream>


Server::Server(const QString& scenefile)
{
    server = new RenderingServer;
    QObject::connect(server, &RenderingServer::getSceneInfo, this, &Server::getSceneInfo);
    QObject::connect(server, &RenderingServer::evaluateSamples, this, &Server::evaluateSamples);
    QObject::connect(server, &RenderingServer::evaluateSamplesCrop, this, &Server::evaluateSamplesCrop);
    QObject::connect(server, &RenderingServer::evaluateSamplesPDF, this, &Server::evaluateSamplesPDF);
    QObject::connect(server, &RenderingServer::finishRender, this, &Server::finish);

    memset(params, 0, NUM_RANDOM_PARAMETERS * sizeof(float));
    memset(features, 0, NUM_FEATURES * sizeof(float));

    scene = readScene(scenefile, params, features);
    if(!scene)
    {
        qDebug() << "Error reading scene file.";
        exit(EXIT_FAILURE);
    }
}

Server::~Server()
{
    delete server;
}

void Server::start(int port)
{
    server->startServer(port);
}

void Server::getSceneInfo(SceneInfo *sceneinfo)
{
    sceneinfo->set("width", scene->width);
    sceneinfo->set("height", scene->height);

    std::vector<bool> usedParamsMask(NUM_RANDOM_PARAMETERS, false);
    for(RandomParameter p: scene->randomParameters)
        usedParamsMask[p] = true;

    sceneinfo->set("has_dof", ((bool)usedParamsMask[LENS_U]) || ((bool)usedParamsMask[LENS_V]));
    sceneinfo->set("has_motion_blur", (bool)usedParamsMask[TIME]);
}

void Server::evaluateSamples(bool isSPP, int numSamples, int* resultSize)
{
    int w = scene->width;
    int h = scene->height;
    int totalNumSamples = isSPP ? w * h * numSamples : numSamples;
    *resultSize = totalNumSamples;

    Sampler* sampler = nullptr;
    if(isSPP)
        sampler = new PixelSampler(0, w, 0, h, numSamples);
    else
        sampler = new SparseSampler(0, w, 0, h, numSamples);

    SamplesPipe pipe;
    render(sampler, pipe);
    delete sampler;
}

void Server::evaluateSamplesCrop(bool isSPP, int numSamples, const CropWindow &crop, int *resultSize)
{
    int w = scene->width;
    int h = scene->height;
    int totalNumSamples = isSPP ? w * h * numSamples : numSamples;
    *resultSize = totalNumSamples;

    Sampler* sampler = nullptr;
    if(isSPP)
        sampler = new PixelSampler(crop.beginX, crop.endX, crop.beginY, crop.endY, numSamples);
    else
        sampler = new SparseSampler(crop.beginX, crop.endX, crop.beginY, crop.endY, numSamples);

    SamplesPipe pipe;
    render(sampler, pipe);
    delete sampler;
}

void Server::evaluateSamplesPDF(bool isSPP, int numSamples, const float* pdf, int *resultSize)
{
}

void Server::finish()
{
    QCoreApplication::quit();
}

void Server::render(Sampler* sampler, SamplesPipe& pipe)
{
    while(sampler->next(params))
    {
        SampleBuffer buffer = pipe.getBuffer();

        params[0] = buffer.set(IMAGE_X, params[0]);
        params[1] = buffer.set(IMAGE_Y, params[1]);
        params[2] = buffer.set(LENS_U, params[2]);
        params[3] = buffer.set(LENS_V, params[3]);
        params[4] = buffer.set(TIME, params[4]);
        params[5] = buffer.set(LIGHT_X, params[5]);
        params[6] = buffer.set(LIGHT_Y, params[6]);

        memset(features, 0, NUM_FEATURES * sizeof(float));

        scene->evaluate();
        buffer.set(COLOR_R, features[0]);
        buffer.set(COLOR_G, features[1]);
        buffer.set(COLOR_B, features[2]);
        buffer.set(WORLD_X, features[3]);
        buffer.set(WORLD_Y, features[4]);
        buffer.set(WORLD_Z, features[5]);
        buffer.set(NORMAL_X, features[6]);
        buffer.set(NORMAL_Y, features[7]);
        buffer.set(NORMAL_Z, features[8]);
        buffer.set(TEXTURE_COLOR_R, features[9]);
        buffer.set(TEXTURE_COLOR_G, features[10]);
        buffer.set(TEXTURE_COLOR_B, features[11]);
        buffer.set(WORLD_X_1, features[12]);
        buffer.set(WORLD_Y_1, features[13]);
        buffer.set(WORLD_Z_1, features[14]);
        buffer.set(NORMAL_X_1, features[15]);
        buffer.set(NORMAL_Y_1, features[16]);
        buffer.set(NORMAL_Z_1, features[17]);
        buffer.set(TEXTURE_COLOR_R_1, features[18]);
        buffer.set(TEXTURE_COLOR_G_1, features[19]);
        buffer.set(TEXTURE_COLOR_B_1, features[20]);

        pipe << buffer;
    }
}
