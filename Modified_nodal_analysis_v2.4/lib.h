//lib.h
#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include "component.h"

constexpr double PI = 3.14159265358979323846;


//========================================================================
//display a txt file, as raw as possible. Just for test/debugging purposes
void display_txt(const std::string filename) {

    std::ifstream myfile(filename);
    if (myfile.is_open())
    {
        std::cout << filename + " file:\n=============" << std::endl;
        std::cout << myfile.rdbuf() << std::endl;
    }
    else std::cout << "Unable to open file\n";
}
//========================================================================
//========================================================================



std::pair<std::vector<double>, std::vector<double>> myLinearchirp(double Fstart, double Fstop, double duration, int Fs) {
    int N = static_cast<int>(duration * Fs);
    double T = static_cast<double>(N) / Fs;
    std::vector<double> t(N), rx(N);

    double alpha = Fstart;
    double beta = (Fstop - Fstart) / T;

    for (int i = 0; i < N; ++i) {
        t[i] = i / static_cast<double>(Fs);
        double phi = 2 * PI * (alpha * t[i] + beta * std::pow(t[i], 2) / 2) + PI / 2;
        rx[i] = std::cos(phi);
    }

    return { t, rx };
}