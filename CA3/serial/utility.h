#pragma once
#include <vector>
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <sndfile.h>
using namespace std;
void print_vector(const vector<float>& vec);
void readWavFile(const std::string &inputFile, std::vector<float> &data, SF_INFO &fileInfo);
void writeWavFile(const std::string &outputFile, const std::vector<float> &data, SF_INFO &fileInfo);