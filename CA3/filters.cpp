
#include "filters.h"


float band_pass_filter_one(float data,float bandwidth){
return (data*data)/((data*data)+(bandwidth*bandwidth));
}
float find_bandwidth(vector <float> data){

    auto min_freq=min_element(data.begin(),data.end());
    auto max_freq=max_element(data.begin(),data.end());
     std::cout << "Maximum value: " << *max_freq << '\n';
    std::cout << "Minimum value: " << *min_freq << '\n';
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