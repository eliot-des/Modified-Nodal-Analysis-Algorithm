# Modified Nodal Analysis Algorithm
---
This repository is just the implementation of the Modified Nodal Analysis for transient analysis of electronic circuit.
The aim is to create a functional program capable of loading a netlist in the form of a `.txt` file, where all the components of the circuit are referenced by their node connections, their values (Ohms, henrys, Farads...), and to simulate the behavior of the circuit for any input signal, and any location where we want to know the output voltage or current.

### Under the hood of the algorithm

The MNA Algorithm is quite well documented over different books/papers as the following ones :
*Laurence W. Nagel - SPICE2: A computer program to simulate semiconductor circuits 
*Lawrence T. Pillage, Ronald A. Rohrer, Chandramouli Visweswariah-  Electronic Ciruit And System Simulation Methods 
*Stefan Jahn, Michael Margraf, Vincent Habchi, Raimund Jacob - QUCS Technical Papers

