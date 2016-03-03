#ifndef SERVER_H
#define SERVER_H

#include "Benchmark/RenderingServer/RenderingServer.h"
#include "SceneReader.h"
#include <memory>
#include <QObject>

class Sampler;

class Server: public QObject
{
    Q_OBJECT
public:
    Server(const QString& scenefile);
    ~Server();

    void start(int port);

private slots:
    void getSceneInfo(SceneInfo* scene);
    void evaluateSamples(bool isSPP, int numSamples, int* resultSize);
    void evaluateSamplesCrop(bool isSPP, int numSamples, const CropWindow& crop, int* resultSize);
    void evaluateSamplesPDF(bool isSPP, int numSamples, const float* pdf, int* resultSize);
    void finish();

private:
    void render(Sampler* sampler, SamplesPipe& pipe);

    RenderingServer* server;

    std::unique_ptr<Scene> scene;
    float params[NUM_RANDOM_PARAMETERS];
    float features[NUM_FEATURES];
};

#endif // SERVER_H
