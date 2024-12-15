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

void applyAndMeasureBandpass(const std::vector<float> &audioData, size_t num_threads,SF_INFO fileInfo) {
    auto bandwidth = find_bandwidth(audioData);
     string output_filename="bandpass_non_threaded";
    // auto start_threaded = std::chrono::high_resolution_clock::now();
    // auto bandpass_thread = apply_bandpass_filter_with_threads(audioData, num_threads, bandwidth);
    // auto end_threaded = std::chrono::high_resolution_clock::now();
   //string output_filename="bandpass_non_threaded";
    auto start_non_threaded = std::chrono::high_resolution_clock::now();
    auto bandpass_without_thread = band_pass_filter_all(audioData);
    auto end_non_threaded = std::chrono::high_resolution_clock::now();

   // auto duration_threaded = std::chrono::duration_cast<std::chrono::milliseconds>(end_threaded - start_threaded).count();
    auto duration_non_threaded = std::chrono::duration_cast<std::chrono::milliseconds>(end_non_threaded - start_non_threaded).count();

   // std::cout << "Time taken with threads: " << duration_threaded << " ms" << std::endl;
    std::cout << "Bandpas ->  Time taken without threads: " << duration_non_threaded << " ms" << std::endl;
       writeWavFile(output_filename,bandpass_without_thread, fileInfo);

    // print_vector(bandpass_thread);
    // print_vector(bandpass_without_thread);
}
void applyAndMeasureNotch(const std::vector<float> &audioData,SF_INFO fileInfo) {
    // auto start_threaded = std::chrono::high_resolution_clock::now();
    // auto notch_threaded = apply_notch_filter_with_threads(audioData, 7, 50.0, 1.0);
    // auto end_threaded = std::chrono::high_resolution_clock::now();
    string output_filename="notch_non_threaded";
    auto start_non_threaded = std::chrono::high_resolution_clock::now();
    auto notch_without_thread = apply_notch_all(audioData, 50.0, 1.0);
    auto end_non_threaded = std::chrono::high_resolution_clock::now();

  //  auto duration_threaded = std::chrono::duration_cast<std::chrono::milliseconds>(end_threaded - start_threaded).count();
    auto duration_non_threaded = std::chrono::duration_cast<std::chrono::milliseconds>(end_non_threaded - start_non_threaded).count();

   // std::cout << "Time taken with threads: " << duration_threaded << " ms" << std::endl;
    std::cout << "notch -> Time taken without threads: " << duration_non_threaded << " ms" << std::endl;
    writeWavFile(output_filename,notch_without_thread, fileInfo);

   // print_vector(notch_threaded);
  //  print_vector(notch_without_thread);
}
void applyAndMeasureFIR(const std::vector<float> &audioData,SF_INFO fileInfo) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);
     string output_filename="fir_non_threaded";
    vector<float> coefficients(10);
    for (size_t i = 0; i < 10; ++i) {
        coefficients[i] = dis(gen);
    }

    auto start_threaded = std::chrono::high_resolution_clock::now();
   // vector<float> fir_threaded = apply_fir_filter_with_threads(audioData, coefficients, 7);
      vector<float> fir_without_thread = apply_fir_filter(audioData, coefficients);
    auto end_threaded = std::chrono::high_resolution_clock::now();

    auto duration_non_threaded = std::chrono::duration_cast<std::chrono::milliseconds>(end_threaded - start_threaded).count();
    std::cout << "FIR -> Time taken without threads: " << duration_non_threaded << " ms" << std::endl;
    writeWavFile(output_filename, fir_without_thread, fileInfo);

}
void applyAndMeasureIIR(const std::vector<float> &audioData, SF_INFO fileInfo) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);

    string output_filename="IIR_non_threaded";
    vector<float> feedforward_coefficients(100);
    vector<float> feedback_coefficients(100);

    for (size_t i = 0; i < 100; ++i) {
        feedforward_coefficients[i] = dis(gen);
        feedback_coefficients[i] = dis(gen);
    }
    // cout<<"hiii\n";
    auto start_non_threaded = std::chrono::high_resolution_clock::now();
    vector<float> iir_without_threaded = apply_iir_filtter(audioData, feedforward_coefficients, feedback_coefficients,feedforward_coefficients.size(),feedback_coefficients.size());
    auto end_non_threaded = std::chrono::high_resolution_clock::now();

    auto duration_non_threaded = std::chrono::duration_cast<std::chrono::milliseconds>(end_non_threaded - start_non_threaded).count();
    std::cout << "IIR -> Time taken without threads: " << duration_non_threaded << " ms" << std::endl;
    writeWavFile(output_filename, iir_without_threaded, fileInfo);
}
int main(int argc, char *argv[]) {
    // Ensure at least one argument is provided
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file> " << std::endl;
        return 1;
    }

    std::string inputFile = argv[1]; // Input file from command-line argument
 // Optional output file
    const size_t num_threads = 7;
    SF_INFO fileInfo;
    std::vector<float> audioData;
 auto start_exec= std::chrono::high_resolution_clock::now();
    std::memset(&fileInfo, 0, sizeof(fileInfo));
  auto start_read = std::chrono::high_resolution_clock::now();
    readWavFile(inputFile, audioData, fileInfo);
     auto end_read = std::chrono::high_resolution_clock::now();
auto duration_read = std::chrono::duration_cast<std::chrono::milliseconds>(end_read- start_read).count();
    std::cout << "read  : " << duration_read << " ms" << std::endl;
    applyAndMeasureBandpass(audioData, num_threads,fileInfo);
    applyAndMeasureNotch(audioData,fileInfo);
     applyAndMeasureFIR(audioData,fileInfo);
    applyAndMeasureIIR(audioData,fileInfo);
    auto end_exec= std::chrono::high_resolution_clock::now();
    auto duration_exec = std::chrono::duration_cast<std::chrono::milliseconds>(end_exec- start_exec).count();
      std::cout << "exec  -> Time taken without threads: " << duration_exec << " ms" << std::endl;

    

    return 0;
}
