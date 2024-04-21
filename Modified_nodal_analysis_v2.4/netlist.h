//netlist.h
#pragma once
#include <Eigen/Dense>
#include <vector>
#include <string>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <memory>

// Forward declarations to avoid circular dependencies
class Component;
class Resistance;
class ReactiveComponent;
class Capacitor;
class Inductance;
class VoltageSource;
class ExternalVoltageSource;
class CurrentSource;
class IdealOPA;


class Netlist {//: public std::enable_shared_from_this<Netlist> 
public:
    std::vector<std::shared_ptr<Component>> components;
    std::vector<std::shared_ptr<Resistance>> resistances;
    std::vector<std::shared_ptr<ReactiveComponent>> reactiveComponents;
    std::vector<std::shared_ptr<IdealOPA>> idealOPAs;
    std::vector<std::shared_ptr<VoltageSource>> voltageSources;
    std::vector<std::shared_ptr<CurrentSource>> currentSources;

    Eigen::MatrixXd A;
    Eigen::VectorXd x, b;

    unsigned m; 
    unsigned n; // Number of unique nodes including the ground node (0)

    // Constructor
    Netlist() = default;                            // Default constructor
    explicit Netlist(const std::string& filename);  // Constructor with filename
    
    // Public methods
    void init(const std::string& filename);         
    Eigen::MatrixXd solve_system(double Ts);	
    std::vector<double> update_system(unsigned start_node, unsigned end_node, const std::vector<double>& audio_sample, double Ts = 0.0);


    // Generic function to get components of a specific type
    template <typename T>
    std::vector<std::shared_ptr<T>> getComponents() {
        std::vector<std::shared_ptr<T>> specificComponents;

        for (const auto& component : components) {
            std::shared_ptr<T> specificComponent = std::dynamic_pointer_cast<T>(component);
            if (specificComponent) {
                specificComponents.push_back(specificComponent);
            }
        }
        return specificComponents;
    }

private:
    // Private methods
    std::vector<std::string> split(const std::string& s, char delimiter);
    std::vector<std::shared_ptr<Component>> createComponentListFromTxt(const std::string& filename);
    std::shared_ptr<Component> createComponent(const std::string& netlistLine, unsigned idx);
    unsigned getNodeNbr();
};