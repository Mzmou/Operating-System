#include <iostream>
#include <sndfile.h>
#include <vector>
#include <string>
#include <cstring>
#include "filters.h"
#include "utility.h"
using namespace std;
void readWavFile(const std::string& inputFile, std::vector<float>& data, SF_INFO& fileInfo) {
    SNDFILE* inFile = sf_open(inputFile.c_str(), SFM_READ, &fileInfo);
    if (!inFile) {
        std::cerr << "Error opening input file: " << sf_strerror(NULL) << std::endl;
        exit(1);
    }

    data.resize(fileInfo.frames * fileInfo.channels);
    sf_count_t numFrames = sf_readf_float(inFile, data.data(), fileInfo.frames);
    if (numFrames != fileInfo.frames) {
        std::cerr << "Error reading frames from file." << std::endl;
        sf_close(inFile);
        exit(1);
    }

    sf_close(inFile);
    // std:: cout<<"frame num is"<<fileInfo.frames<<'\n';
    std::cout << "Successfully read " << numFrames << " frames from " << inputFile << std::endl;
}

void writeWavFile(const std::string& outputFile, const std::vector<float>& data,  SF_INFO& fileInfo) {
   // std:: cout<<"frame num is"<<fileInfo.frames<<'\n';
    sf_count_t originalFrames = fileInfo.frames;
    SNDFILE* outFile = sf_open(outputFile.c_str(), SFM_WRITE, &fileInfo);
 //  std:: cout<<"frame num is"<<fileInfo.frames<<'\n';
    if (!outFile) {
        std::cerr << "Error opening output file: " << sf_strerror(NULL) << std::endl;
        exit(1);
    }

    sf_count_t numFrames = sf_writef_float(outFile, data.data(), originalFrames);
    if (numFrames != originalFrames) {
        std::cerr << "Error writing frames to file." << std::endl;
        sf_close(outFile);
        exit(1);
    }

    sf_close(outFile);
    std::cout << "Successfully wrote " << numFrames << " frames to " << outputFile << std::endl;
}

int main() {
    std::string inputFile = "input.wav";
    std::string outputFile = "output_first_filter.wav";

    SF_INFO fileInfo;
    std::vector<float> audioData;

    std::memset(&fileInfo, 0, sizeof(fileInfo));

    readWavFile(inputFile, audioData, fileInfo);
 //   std:: cout<<"frame num is"<<fileInfo.frames<<'\n';
    

    // for(int i=0;i<audioData.size();i++)
    // std::cout<<audioData[i]<<'\n';
    print_vector((audioData));
  //cout<< band_pass_filter_one(1.0,2.0)<<'\n';
  //cout<<find_bandwidth(audioData)<<'\n';
  auto first_filter_data=band_pass_filter_all(audioData);
  //print_vector(band_pass_filter_all(audioData));
    writeWavFile(outputFile, first_filter_data, fileInfo);


      auto first_filter_data=band_pass_filter_all(audioData);
  //print_vector(band_pass_filter_all(audioData));
    writeWavFile(outputFile, first_filter_data, fileInfo);

    return 0;
}
