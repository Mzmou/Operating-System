#pragma once
#include <vector>
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <sndfile.h>

#include <pthread.h>

using namespace std;
void print_vector(const vector<float>& vec);
void readWavFile(const std::string &inputFile, std::vector<float> &data, SF_INFO &fileInfo);
void readWavFile_threaded(const std::string &inputFile, std::vector<float> &data, SF_INFO &fileInfo, int numThreads);
void writeWavFile(const std::string &outputFile, const std::vector<float> &data, SF_INFO &fileInfo);