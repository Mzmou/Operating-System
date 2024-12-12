
#include "filters.h"


float band_pass_filter_one(float data,float bandwidth){
return (data*data)/((data*data)+(bandwidth*bandwidth));
}
void* process_segment(void* arg) {
      // Cast the argument back to ThreadData
    ThreadData* data = static_cast<ThreadData*>(arg);
    const vector<float>& input = *(data->input);
    vector<float>& output = *(data->output);

    // Process the assigned segment
    for (size_t i = data->start; i < data->end; ++i) {
        output[i - data->start] = band_pass_filter_one(input[i], data->bandwidth);
    }
        return nullptr;
}
float find_bandwidth(vector <float> data){

    auto min_freq=min_element(data.begin(),data.end());
    auto max_freq=max_element(data.begin(),data.end());
     cout << "Maximum value: " << *max_freq << '\n';
    cout << "Minimum value: " << *min_freq << '\n';
    return *max_freq-*min_freq;

}
vector <float> band_pass_filter_all(vector <float> data){
    vector <float> result;
    auto band_width=find_bandwidth(data);
    for(int i=0;i<data.size();i++){
        result.push_back(band_pass_filter_one(data[i],band_width));
    }
    return result;
}
float apply_notch_one(float data,float freq_init,float n){
    return 1/(pow((data/freq_init),n)+1);
}
vector <float> apply_notch_all(vector<float> data,float freq_init,float n){
    vector<float> result;
    for (const auto& element : data) {
        result.push_back(apply_notch_one(element, freq_init, n));
    }
    return result;
}
vector<float> apply_bandpass_filter_with_threads(const vector<float>& input,size_t num_threads,float bandwidth) 
{
    size_t segment_size = input.size() / num_threads;
    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];
    vector<vector<float>> results(num_threads);

    for (size_t i = 0; i < num_threads; ++i) {
        size_t start = i * segment_size;
        size_t end = (i == num_threads - 1) ? input.size() : start + segment_size;

        results[i].resize(end - start);
        thread_data[i] = {&input, &results[i], start, end, bandwidth,0,0};

        if (pthread_create(&threads[i], nullptr, process_segment, &thread_data[i]) != 0) {
            throw runtime_error("Error creating thread " + to_string(i));
        }
    }

    for (size_t i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], nullptr);
    }

    // Merge results
    vector<float> output;
    for (const auto& result : results) {
        output.insert(output.end(), result.begin(), result.end());
    }

    return output;
}
void* process_segment_notch(void* arg) {
      // Cast the argument back to ThreadData
    ThreadData* data = static_cast<ThreadData*>(arg);
    const vector<float>& input = *(data->input);
    vector<float>& output = *(data->output);

    // Process the assigned segment
    for (size_t i = data->start; i < data->end; ++i) {
        output[i - data->start] = apply_notch_one(input[i], data->init_freq,data->n);
    }
        return nullptr;
}
vector<float> apply_notch_filter_with_threads(const vector<float>& input,size_t num_threads,float init_freq,float n) 
{
    size_t segment_size = input.size() / num_threads;
    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];
    vector<vector<float>> results(num_threads);

    for (size_t i = 0; i < num_threads; ++i) {
        size_t start = i * segment_size;
        size_t end = (i == num_threads - 1) ? input.size() : start + segment_size;

        results[i].resize(end - start);
        thread_data[i] = {&input, &results[i], start, end,0,init_freq,n};

        if (pthread_create(&threads[i], nullptr, process_segment_notch, &thread_data[i]) != 0) {
            throw runtime_error("Error creating thread " + to_string(i));
        }
    }

    for (size_t i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], nullptr);
    }

    // Merge results
    vector<float> output;
    for (const auto& result : results) {
        output.insert(output.end(), result.begin(), result.end());
    }

    return output;
}

std::vector<float> apply_fir_filter(const std::vector<float>& input, const std::vector<float>& coefficients) {
    size_t N = coefficients.size();
    size_t input_size = input.size();
    std::vector<float> output(input_size, 0);

    for (size_t n = 0; n < input_size; ++n) {
        float sum = 0;
        for (size_t i = 0; i < N; ++i) {
            if (n >= i) {
                sum += coefficients[i] * input[n - i];
            }
        }
        output[n] = sum;
        cout<<sum<<'\n';
    }

    return output;
}
void* process_fir_segment(void* arg) {
    FIRThreadData* data = static_cast<FIRThreadData*>(arg);
    const vector<float>& input = *(data->input);
    const vector<float>& coefficients = *(data->coefficients);
    vector<float>& output = *(data->output);
    size_t N = coefficients.size();

    for (size_t n = data->start; n < data->end; ++n) {
        float sum = 0;
        for (size_t i = 0; i < N; ++i) {
            if (n >= i) {
                sum += coefficients[i] * input[n - i];
            }
        }
        output[n - data->start] = sum;
    }
    return nullptr;
}



vector<float> apply_fir_filter_with_threads(const vector<float>& input, const vector<float>& coefficients, size_t num_threads) {
    size_t input_size = input.size();
    size_t segment_size = input_size / num_threads;

    pthread_t threads[num_threads];
    FIRThreadData thread_data[num_threads];
    vector<vector<float>> results(num_threads);

    // Create threads to process segments
    for (size_t i = 0; i < num_threads; ++i) {
        size_t start = i * segment_size;
        size_t end = (i == num_threads - 1) ? input_size : start + segment_size;

        results[i].resize(end - start);
        thread_data[i] = {&input, &coefficients, &results[i], start, end};

        if (pthread_create(&threads[i], nullptr, process_fir_segment, &thread_data[i]) != 0) {
            throw runtime_error("Error creating thread " + to_string(i));
        }
    }

    // Wait for all threads to finish
    for (size_t i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], nullptr);
    }

    // Merge results
    vector<float> output;
    for (const auto& result : results) {
        output.insert(output.end(), result.begin(), result.end());
    }

    return output;
}

std::vector<double> apply_iir_filtter(const std::vector<double>& input_signal, // Input signal
                                                 const std::vector<double>& feedforward_coefficients, // Feedforward coefficients
                                                 const std::vector<double>& feedback_coefficients, // Feedback coefficients
                                                 size_t feedforward_order,                     // Feedforward order
                                                 size_t feedback_order) {                   // Feedback order
    // Output signal y, initialized to zero
    std::vector<double> output_signal(input_signal.size(), 0.0);

    // Iterate through each sample of the input signal
    for (size_t n = 0; n < input_signal.size(); ++n) {
        // Compute feedforward part
        for (size_t i = 0; i <= feedforward_order; ++i) {
            if (n >= i) {
                output_signal[n] += feedforward_coefficients[i] * input_signal[n - i];
            }
        }

        // Compute feedback part
        for (size_t i = 1; i <= feedback_order; ++i) {
            if (n >= i) {
                output_signal[n] += feedback_coefficients[i] * output_signal[n - i];
            }
        }
    }

    return output_signal;
}
void* process_iir_segment(void* arg) {
    IIRThreadData* data = static_cast<IIRThreadData*>(arg);

    const std::vector<float>& input = *(data->input);
    const std::vector<float>& feedforward_coefficients = *(data->feedforward_coefficients);
    const std::vector<float>& feedback_coefficients = *(data->feedback_coefficients);
    std::vector<float>& result = *(data->result);

    size_t feedforward_order = feedforward_coefficients.size();
    size_t feedback_order = feedback_coefficients.size();

    for (size_t n = data->start_index; n < data->end_index; ++n) {
        float sum_feedforward = 0.0f;
        float sum_feedback = 0.0f;

        // Compute feedforward part
        for (size_t k = 0; k < feedforward_order; ++k) {
            if (n >= k) {
                sum_feedforward += feedforward_coefficients[k] * input[n - k];
            }
        }

        // Compute feedback part
        for (size_t k = 1; k < feedback_order; ++k) {
            if (n >= k) {
                sum_feedback += feedback_coefficients[k] * result[n - k];
            }
        }

        result[n - data->start_index] = sum_feedforward + sum_feedback;
    }

    return nullptr;
}
std::vector<float> apply_iir_filter_with_threads(const std::vector<float>& input,
                                                 const std::vector<float>& feedforward_coefficients,
                                                 const std::vector<float>& feedback_coefficients,
                                                 size_t num_threads) {
    size_t input_size = input.size();
    size_t segment_size = input_size / num_threads;

    pthread_t threads[num_threads];
    IIRThreadData thread_data[num_threads];
    std::vector<std::vector<float>> results(num_threads);

    // Create threads to process segments
    for (size_t i = 0; i < num_threads; ++i) {
        size_t start = i * segment_size;
        size_t end = (i == num_threads - 1) ? input_size : start + segment_size;

        results[i].resize(end - start);
        thread_data[i] = {&input, &feedforward_coefficients, &feedback_coefficients, &results[i], start, end};

        if (pthread_create(&threads[i], nullptr, process_iir_segment, &thread_data[i]) != 0) {
            throw std::runtime_error("Error creating thread " + std::to_string(i));
        }
    }

    // Wait for all threads to finish
    for (size_t i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], nullptr);
    }

    // Merge results
    std::vector<float> output(input_size, 0.0f);
    for (size_t i = 0; i < num_threads; ++i) {
        size_t start = i * segment_size;
        size_t end = (i == num_threads - 1) ? input_size : start + segment_size;

        for (size_t n = start; n < end; ++n) {
            output[n] = results[i][n - start];
        }
    }

    return output;
}
