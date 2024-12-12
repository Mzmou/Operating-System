#pragma once
#include <vector>
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
using namespace std;
struct ThreadData
{
    const vector<float> *input;
    vector<float> *output;
    size_t start;
    size_t end;
    float bandwidth;
    float init_freq;
    float n;
};
struct FIRThreadData
{
    const vector<float> *input;
    const vector<float> *coefficients;
    vector<float> *output;
    size_t start;
    size_t end;
};
struct IIRThreadData {
    const std::vector<float>* input;
    const std::vector<float>* feedforward_coefficients;
    const std::vector<float>* feedback_coefficients;
    std::vector<float>* result;
    size_t start_index;
    size_t end_index;
};
float band_pass_filter_one(float data, float bandwidth);
float find_bandwidth(vector<float> data);
vector<float> band_pass_filter_all(vector<float> data);
float apply_notch_one(float data, float freq_init, float n);
vector<float> apply_notch_all(vector<float> data, float freq_init, float n);
void *process_segment(void *arg);
vector<float> apply_bandpass_filter_with_threads(const vector<float> &input, size_t num_threads, float bandwidth);
void *process_segment_notch(void *arg);
vector<float> apply_notch_filter_with_threads(const vector<float> &input, size_t num_threads, float init_freq, float n);
vector<float> apply_fir_filter(const vector<float> &input, const vector<float> &coefficients);
void *process_fir_segment(void *arg);
vector<float> apply_fir_filter_with_threads(const vector<float> &input, const vector<float> &coefficients, size_t num_threads);
vector<float> apply_iir_filtter(const vector<float> &input_signal,const vector<float> &feedforward_coefficients,  const vector<float> &feedback_coefficients,
                                size_t feedforward_order,size_t feedback_order);
void* process_iir_segment(void* arg) ;
std::vector<float> apply_iir_filter_with_threads(const std::vector<float>& input,
                                                 const std::vector<float>& feedforward_coefficients,
                                                 const std::vector<float>& feedback_coefficients,
                                                 size_t num_threads);