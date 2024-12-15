#include "utility.h"

struct ThreadData {
    SNDFILE *inFile;
    std::vector<float> *data;
    sf_count_t startFrame;
    sf_count_t framesToRead;
    sf_count_t channels;
};

void *readChunk(void *arg) {
    ThreadData *threadData = (ThreadData *)arg;
    sf_count_t start = threadData->startFrame * threadData->channels;
    sf_count_t count = threadData->framesToRead * threadData->channels;

    if (sf_seek(threadData->inFile, threadData->startFrame, SEEK_SET) == -1) {
        std::cerr << "Error seeking in file." << std::endl;
        pthread_exit(nullptr);
    }

    sf_count_t readCount = sf_readf_float(threadData->inFile, threadData->data->data() + start, threadData->framesToRead);
//cout<<readCount<<" "<<threadData->framesToRead<<'\n';
    if (readCount != threadData->framesToRead) {
        std::cerr << "Error reading frames in thread.\n" << std::endl;
    }

    pthread_exit(nullptr);
}
void readWavFile(const std::string &inputFile, std::vector<float> &data, SF_INFO &fileInfo)
{
    SNDFILE *inFile = sf_open(inputFile.c_str(), SFM_READ, &fileInfo);
    if (!inFile)
    {
        std::cerr << "Error opening input file: " << sf_strerror(NULL) << std::endl;
        exit(1);
    }

    data.resize(fileInfo.frames * fileInfo.channels);
    sf_count_t numFrames = sf_readf_float(inFile, data.data(), fileInfo.frames);
    if (numFrames != fileInfo.frames)
    {
        std::cerr << "Error reading frames from file." << std::endl;
        sf_close(inFile);
        exit(1);
    }

    sf_close(inFile);
    std::cout << "Successfully read " << numFrames << " frames from " << inputFile << std::endl;
}
void readWavFile_threaded(const std::string &inputFile, std::vector<float> &data, SF_INFO &fileInfo, int numThreads) {
    SNDFILE *inFile = sf_open(inputFile.c_str(), SFM_READ, &fileInfo);
    if (!inFile) {
        std::cerr << "Error opening input file: " << sf_strerror(nullptr) << std::endl;
        exit(1);
    }

    data.resize(fileInfo.frames * fileInfo.channels);

    std::vector<pthread_t> threads(numThreads);
    std::vector<ThreadData> threadData(numThreads);

    sf_count_t framesPerThread = fileInfo.frames / numThreads;
    sf_count_t remainingFrames = fileInfo.frames % numThreads;

    for (int i = 0; i < numThreads; ++i) {
        threadData[i].inFile = inFile;
        threadData[i].data = &data;
        threadData[i].startFrame = i * framesPerThread;
        threadData[i].framesToRead = (i == numThreads - 1) ? framesPerThread + remainingFrames : framesPerThread;
        threadData[i].channels = fileInfo.channels;

        if (pthread_create(&threads[i], nullptr, readChunk, &threadData[i]) != 0) {
            std::cerr << "Error creating thread " << i << std::endl;
            sf_close(inFile);
            exit(1);
        }
    }

    for (int i = 0; i < numThreads; ++i) {
        if (pthread_join(threads[i], nullptr) != 0) {
            std::cerr << "Error joining thread " << i << std::endl;
            sf_close(inFile);
            exit(1);
        }
    }

    sf_close(inFile);
    std::cout << "Successfully read " << fileInfo.frames << " frames from " << inputFile << " using " << numThreads << " threads." << std::endl;
}

void writeWavFile(const std::string &outputFile, const std::vector<float> &data, SF_INFO &fileInfo)
{
    sf_count_t originalFrames = fileInfo.frames;
    SNDFILE *outFile = sf_open(outputFile.c_str(), SFM_WRITE, &fileInfo);
    if (!outFile)
    {
        std::cerr << "Error opening output file: " << sf_strerror(NULL) << std::endl;
        exit(1);
    }

    sf_count_t numFrames = sf_writef_float(outFile, data.data(), originalFrames);
    if (numFrames != originalFrames)
    {
        std::cerr << "Error writing frames to file." << std::endl;
        sf_close(outFile);
        exit(1);
    }

    sf_close(outFile);
    std::cout << "Successfully wrote " << numFrames << " frames to " << outputFile << std::endl;
}