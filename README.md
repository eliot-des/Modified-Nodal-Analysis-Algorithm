# Modified Nodal Analysis Algorithm
---
This repository is just the implementation of the Modified Nodal Analysis for **transient analysis** of electronic circuit.
The aim is to create a functional program capable of loading a netlist in the form of a `.txt` file, where all the components of the circuit are referenced by their node connections, their values (Ohms, henrys, Farads...), and to simulate the behavior of the circuit for any input signal, and any location where we want to know the output voltage or current.

## Documentation

The MNA Algorithm is quite well documented over different books/papers as the following ones :

- Laurence W. Nagel - SPICE2: A computer program to simulate semiconductor circuits 
- Lawrence T. Pillage, Ronald A. Rohrer, Chandramouli Visweswariah -  Electronic Ciruit And System Simulation Methods
- Stefan Jahn, Michael Margraf, Vincent Habchi, Raimund Jacob - QUCS Technical Papers

## Under the hood of the algorithm

Many different kinds of network element are encountered in network analysis. For circuit analysis
it is necessary to formulate equations for circuits containing as many different types of network
elements as possible. There are various methods for equation formulation for a circuit. These
are based on three types of equations found in circuit theory:

* equations based on Kirchhoff’s voltage law (KVL)
* equations based on Kirchhoff’s current law (KCL)
* branch constitutive equations
