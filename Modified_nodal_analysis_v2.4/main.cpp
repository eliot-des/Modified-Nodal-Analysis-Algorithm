// main.cpp
#pragma once
#include <iostream>
#include <iomanip>
#include "lib.h"
#include "component.h"
#include "netlist.h"
#include "chrono"

int main() {

    //std::string filename = "Netlist.txt";

    display_txt("Netlist.txt");


    Netlist netlist;        //declaration
    netlist.init("Netlist.txt"); //initialization

   
    int Fs = 48000;
    double Ts = 1.0 / Fs;
    /*
    int Fstart = 10;
    int Fstop = 20000;
    float duration = 5.0;
    auto temp = myLinearchirp(Fstart, Fstop, duration, Fs);
    */
    int A = 2;
    int F = 100;
    float duration = 0.01;
    auto temp = mySine(A, F, duration, Fs);

    std::vector<double> t   = temp.first;
    std::vector<double> Vin = temp.second;
    std::vector<double> Vout = netlist.update_system(Vin, Ts, 0, 32);

    std::ofstream outFile("C:/Users/eliot/OneDrive/Bureau/MNA Algorithm/Output_reader/Data_cpp.txt");

    /*
    Write the time, input voltage and output voltage vectors in a text file with a column associate to each variable.
    This is used to plot the input and output voltage over time with a python script.
    */

    outFile << std::setw(13) << std::left << "Time [s]" << " ";
    outFile << std::setw(13) << std::left << "Vin [V] " << " ";
    outFile << std::setw(13) << std::left << "Vout [V]" << std::endl;

    for (int i = 0; i < t.size(); ++i) {
        outFile << std::setw(13) << std::left <<    t[i] << " ";
        outFile << std::setw(13) << std::left <<  Vin[i] << " ";
        outFile << std::setw(13) << std::left << Vout[i] << std::endl;
	}

    return 0;
}














/*
    double A = 1.0;                         // Amplitude
    double f = 10.0;                        // Frequency in Hz
    double tmax = 1.0;                      // Maximum time, assuming 1 second for example
    int Fs = 48000;                         // Sampling frequency in Hz
    double Ts = 1.0 / Fs;                   // Sampling period in seconds

    int n = static_cast<int>(tmax * Fs);    // Number of samples

    std::vector<double> t(n);               // Time vector
    std::vector<double> Vin(n);             // Input voltage vector

    for (int i = 0; i < n; ++i) {
        t[i] = i * Ts;
        Vin[i] = A * std::sin(2 * PI * f * t[i]);
    }

    std::vector<double> Vout = netlist.update_system(2, 0, Vin, Ts);
    */