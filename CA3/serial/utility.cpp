#include "utility.h"
void print_vector(const vector<float>& vec) {
    std::cout << "[";
    for (size_t i = vec.size()-1; i >vec.size()-11; i--) {
        std::cout << vec[i];
        if (i < vec.size() - 1) {
            std::cout << ", "; // Add a comma between elements
        }
    }
    std::cout << "]" << std::endl;
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