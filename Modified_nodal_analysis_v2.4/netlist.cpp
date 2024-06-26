//netlist.cpp
#include "Netlist.h"
#include "component.h"
#include "chrono"

Netlist::Netlist(const std::string& filename) {
    init(filename);
}

void Netlist::init(const std::string& filename) {

    components = createComponentListFromTxt(filename);

    resistances        = getComponents<Resistance>();
    reactiveComponents = getComponents<ReactiveComponent>();
    idealOPAs          = getComponents<IdealOPA>();
    voltageSources     = getComponents<VoltageSource>();
    currentSources     = getComponents<CurrentSource>();
    voltageProbes	   = getComponents<VoltageProbe>();
    diodes 		       = getComponents<Diode>();

    m = std::size(voltageSources) + std::size(reactiveComponents) + std::size(idealOPAs);
    n = getNodeNbr();       // Total number of unique nodes

    A.resize(n + m, n + m);
    x.resize(n + m);
    b.resize(n + m);

    A.setZero();
    x.setZero();
    b.setZero();
}



void Netlist::solve_system(double Ts) {
    for (const auto& comp : reactiveComponents) comp->setResistance(Ts);
    for (const auto& comp : components) comp->stamp(*this);
    luDecomp.compute(A.bottomRightCorner(A.rows() - 1, A.cols() - 1));
}

std::vector<double> Netlist::update_system(const std::vector<double>& audio_sample, const double Ts, const unsigned int v_Probe_idx, const unsigned imax = 32) {
    std::vector<double> output(audio_sample.size(), 0.0);

    std::cout << "A matrix:\n" << A << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    
    //the if condition is used to check if the circuit is linear or not
    //Far from being optimal, because if we call this function multiple times, we will have to re-stamp the system each time
    //and to check this condition each time
    if (diodes.size()==0){ // if the circuit is linear
        solve_system(Ts);

        for (size_t i = 0; i < audio_sample.size(); ++i) {
            for (auto& source : voltageSources) {
                std::shared_ptr<ExternalVoltageSource> externalSource = std::dynamic_pointer_cast<ExternalVoltageSource>(source);
                if (externalSource) {
                    externalSource->update(audio_sample[i]);
                }
                source->stamp(*this);
            }

            for (auto& comp : reactiveComponents) {
                comp->updateVoltage(*this);
                comp->stamp(*this);
            }

            x.tail(x.size() - 1) = luDecomp.solve(b.tail(b.size() - 1));

            //actualize the voltage value on the voltage probes
            for (auto& voltageProbe : voltageProbes) {
                voltageProbe->getVoltage(*this);
            }

            output[i] = voltageProbes[v_Probe_idx]->value;
        }
    }
    else { // if the circuit includes non-linear components such as diodes
        for (auto& comp : reactiveComponents) {
			comp->setResistance(Ts);
		}

        for (size_t i = 0; i < audio_sample.size(); ++i) {
            for (auto& source : voltageSources) {
				std::shared_ptr<ExternalVoltageSource> externalSource = std::dynamic_pointer_cast<ExternalVoltageSource>(source);
                if (externalSource) {
					externalSource->update(audio_sample[i]);
				}
			}

            for (auto& comp : reactiveComponents) {
				comp->updateVoltage(*this);
			}

            //Newton-Raphson method
            for (unsigned k = 1; k < imax; k++) {
                /*
                Have to improve the way to update the values of the diodes,
                since re - stamping the whole system at each iteration is not efficient.
                In theory, we should just stamp the new values of the diodes in the A matrix and b vector.
                Here, we are oblige to reset all the system, since the stamping operation
                is done by a "+=" operation, and we can't just remove the contribution of the diodes
                in the A matrix and b vector
                */

                A.setZero();
                b.setZero();

                for (auto& diode : diodes) {
                    diode->update_voltage(*this);
                    diode->update_Id(*this);
                    diode->update_Geq(*this);
                    diode->update_Ieq(*this);
                }

                for (auto& comp : components) {
                    comp->stamp(*this);
                }
                luDecomp.compute(A.bottomRightCorner(A.rows() - 1, A.cols() - 1));

                Eigen::VectorXd x_old = x;
                x.tail(x.size() - 1) = luDecomp.solve(b.tail(b.size() - 1));

                if ((x_old.tail(x_old.size() - 1) - x.tail(x.size() - 1)).norm() < 1e-6) {
                    break;
                }
            }

			//actualize the voltage value on the voltage probes
            for (auto& voltageProbe : voltageProbes) {
				voltageProbe->getVoltage(*this);
			}   

			output[i] = voltageProbes[v_Probe_idx]->value;
		}
	}   

    auto stop = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
    std::cout << "Time: " << time << " us" << std::endl;

    return output;
}


std::vector<std::string> Netlist::split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}


// Factory method to create components from a definition string
std::shared_ptr<Component> Netlist::createComponent(const std::string& netlistLine, unsigned idx) {
    auto tokens = split(netlistLine, ' ');
    std::string symbol = tokens[0];
    unsigned start_node = std::stoi(tokens[1]);
    unsigned end_node = std::stoi(tokens[2]);
    double value = std::stod(tokens[3]);

    // Depending on the symbol, instantiate the appropriate component
    // Example for a resistor:
    switch (symbol[0]) {
    case 'V':
        if (symbol[1] == 'i') {
            return std::make_shared<ExternalVoltageSource>(start_node, end_node, value, idx);
        }
        else if (symbol[1] == 'o') {
            return std::make_shared<VoltageProbe>(start_node, end_node);
        }
        else {
            return std::make_shared<VoltageSource>(start_node, end_node, value, idx);
        }
    case 'R':
        return std::make_shared<Resistance>(start_node, end_node, value);
    case 'C':
        return std::make_shared<Capacitor>(start_node, end_node, value, idx);
    case 'L':
        return std::make_shared<Inductance>(start_node, end_node, value, idx);
    case 'I':
        return std::make_shared<CurrentSource>(start_node, end_node, value);
    case 'O':
        return std::make_shared<IdealOPA>(start_node, end_node, value, idx);
    case 'D':
        return std::make_shared<Diode>(start_node, end_node);
    case '#':
        return nullptr; // Comment line, return null
    default:
        throw std::runtime_error("Unknown component symbol: " + symbol);
    }
}

std::vector<std::shared_ptr<Component>> Netlist::createComponentListFromTxt(const std::string& filename) {
    std::vector<std::shared_ptr<Component>> components;
    std::ifstream netlistTxt(filename);
    std::string line;
    unsigned idx = 0;

    if (netlistTxt.is_open()) {
        while (std::getline(netlistTxt, line)) {
            if (!line.empty()) {
                auto component = createComponent(line, idx);
                if (dynamic_cast<VoltageSource*>(component.get()) != nullptr ||
                    dynamic_cast<ReactiveComponent*>(component.get()) != nullptr ||
                    dynamic_cast<IdealOPA*>(component.get()) != nullptr) {
                    idx++;
                }
                if (component) {
                    components.push_back(std::move(component));
                }
            }
        }
        netlistTxt.close();
    }
    else {
        std::cout << "Unable to open the netlist file" << std::endl;
    }
    return components;
}

// Calculate the total number of unique nodes (including ground)
unsigned Netlist::getNodeNbr() {
    std::unordered_set<unsigned> nodes;
    for (const auto& comp : components) {
        nodes.insert(comp->start_node);
        nodes.insert(comp->end_node);
    }
    return nodes.size();
}
