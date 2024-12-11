
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
vector<float> apply_filter_with_threads(const vector<float>& input,size_t num_threads,float bandwidth) 
{
    size_t segment_size = input.size() / num_threads;
    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];
    vector<vector<float>> results(num_threads);

    for (size_t i = 0; i < num_threads; ++i) {
        size_t start = i * segment_size;
        size_t end = (i == num_threads - 1) ? input.size() : start + segment_size;

        results[i].resize(end - start);
        thread_data[i] = {&input, &results[i], start, end, bandwidth};

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