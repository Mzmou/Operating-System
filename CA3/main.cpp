#include <iostream>
#include <sndfile.h>
#include <vector>
#include <string>
#include <cstring>
#include "filters.h"
#include "utility.h"
using namespace std;
#include <pthread.h>
#include <chrono>
#include <random>
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

int main()
{
    std::string inputFile = "input.wav";
    std::string outputFile = "output_first_filter.wav";
    const size_t num_threads = 8;
    SF_INFO fileInfo;
    std::vector<float> audioData;

    std::memset(&fileInfo, 0, sizeof(fileInfo));

    readWavFile(inputFile, audioData, fileInfo);
    std::vector<std::vector<float>> results(num_threads);
    auto bandwidth = find_bandwidth(audioData);
    auto start_threaded = std::chrono::high_resolution_clock::now();
    auto bandpass_thread = apply_bandpass_filter_with_threads(audioData, num_threads, bandwidth);
    auto end_threaded = std::chrono::high_resolution_clock::now();

    // Measure time for non-threaded filter
    auto start_non_threaded = std::chrono::high_resolution_clock::now();
    auto bandpass_without_thread = band_pass_filter_all(audioData);
    auto end_non_threaded = std::chrono::high_resolution_clock::now();

    // Calculate durations
    auto duration_threaded = std::chrono::duration_cast<std::chrono::milliseconds>(end_threaded - start_threaded).count();
    auto duration_non_threaded = std::chrono::duration_cast<std::chrono::milliseconds>(end_non_threaded - start_non_threaded).count();

    // Print timing results
    std::cout << "Time taken with threads: " << duration_threaded << " ms" << std::endl;
    std::cout << "Time taken without threads: " << duration_non_threaded << " ms" << std::endl;
    // print_vector(res);
    print_vector(bandpass_thread);
    print_vector(bandpass_without_thread);
    start_threaded = std::chrono::high_resolution_clock::now();
    auto notch_threaded = apply_notch_filter_with_threads(audioData, 7, 50.0, 1.0);
    end_threaded = std::chrono::high_resolution_clock::now();
    start_non_threaded = std::chrono::high_resolution_clock::now();
    auto notch_without_thread = apply_notch_all(audioData, 50.0, 1.0);
    end_non_threaded = std::chrono::high_resolution_clock::now();
    duration_threaded = std::chrono::duration_cast<std::chrono::milliseconds>(end_threaded - start_threaded).count();
    duration_non_threaded = std::chrono::duration_cast<std::chrono::milliseconds>(end_non_threaded - start_non_threaded).count();
    std::cout << "Time taken with threads: " << duration_threaded << " ms" << std::endl;
    std::cout << "Time taken without threads: " << duration_non_threaded << " ms" << std::endl;
    print_vector(notch_threaded);
    print_vector(notch_without_thread);
     random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);


    // Generate random coefficients with the same size
    vector<float> coefficients(audioData.size());
    for (size_t i = 0; i < audioData.size(); ++i) {
        coefficients[i] = dis(gen);
    }

    // Apply the FIR filter
    start_threaded = std::chrono::high_resolution_clock::now();
    vector <float> fir_threaded=apply_fir_filter_with_threads(audioData,coefficients,7);
    //vector<float> fir_without_thread = apply_fir_filter(audioData, coefficients);
    end_threaded = std::chrono::high_resolution_clock::now();
    duration_threaded = std::chrono::duration_cast<std::chrono::milliseconds>(end_threaded - start_threaded).count();
    std::cout << "Time taken with threads: " << duration_threaded << " ms" << std::endl;
    //print_vector(audioData);
    return 0;
}
