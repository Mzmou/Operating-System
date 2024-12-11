#pragma once
#include <vector>
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
using namespace std;
struct ThreadData {
    const std::vector<float>* input;  // Pointer to the input vector
    std::vector<float>* output;      // Pointer to the thread's output vector
    size_t start;                    // Start index
    size_t end;                      // End index
    float bandwidth;                 // Bandwidth parameter
};
float band_pass_filter_one(float data,float bandwidth);
float find_bandwidth(vector <float> data);
vector<float> band_pass_filter_all(vector <float> data);
float apply_notch_one(float data,float freq_init,float n);
vector <float> apply_notch_all(vector<float> data,float freq_init,float n);
void* process_segment(void* arg);
vector<float> apply_bandpass_filter_with_threads(const vector<float>& input,size_t num_threads,float bandwidth); 