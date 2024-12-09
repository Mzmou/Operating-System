#pragma once
#include <vector>
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <math.h>
using namespace std;

float band_pass_filter_one(float data,float bandwidth);
float find_bandwidth(vector <float> data);
vector<float> band_pass_filter_all(vector <float> data);