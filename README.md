# Carry Look Ahead-adder-HPC
This is a parallelized full adder implented in **C++** using **OpenMP** libraries.

This project aims at implementing a fast way for calculating the sum of two numbers (binary numbers), and parallelize that implementation using **OpenMP** directives in C++.

## Problem Definition
### Simple Full Adder
Adding two n-bit numbers, using a **full adder**, requires waiting for the generated carray affter the addition of the previous bit (least significant bit), before adding the current working bit.  

The following diagram illustrates how the **Full Adder** works:

![4-bit-adder](https://github.com/waterflow80/Carry-look-ahead-adder-HPC/assets/82417779/726c30d5-665f-4b6a-9820-fbfd397fe50a)

As we can see, each adder block will have to wait for the **Cout**(carry out) of the previous adder block.

Using this implementation, it will be **impossible for us to parallelize** the calculation, because the **Cirtical Path Length** of the dependency graph will be of size N.

### Carry Look Ahead Adder
The new approach will be using the **carry look ahead adder**'s algorithm in order to reduce the dependency between adder blocks. 
The main idea is to create another circuit that generates the carry out, of any given block, using only **C0** and **Ai** and **Bi**.

The floowing diagram illustrates the idea:

![comb-adder3 drawio](https://github.com/waterflow80/Carry-look-ahead-adder-HPC/assets/82417779/9d33817f-30e9-4324-9081-d569cab9304f)

The combinational ciruit will be using the **Generate** and the **Propagate** logic to calculate the the carry out of any given block.

## Implementation
In out first implementation of the carry look ahead, we'll have two major steps:
### 1. Calculate the Propagate and the Generate
We're going to use two matrices to hold the values of **Propagate** and **Generate**, in order to avoid calculating the P and G for the same block twice. Here's an example of that logic for two **4-bit** numbers `a=1010` and `b=1100`:

![genrate-progapage-hpc-2 drawio](https://github.com/waterflow80/Carry-look-ahead-adder-HPC/assets/82417779/20f2c938-5143-4bb4-b703-94a1972e3df0)

Both `P` and `G` matrices will be of size `[log(N)][N]`, where **N** is the number of bits in each number (assuming both numbers are of equal number of bits). 

Here's the content of those two matrices after executing the program for that same example:

![Screenshot from 2024-05-03 17-37-34](https://github.com/waterflow80/Carry-look-ahead-adder-HPC/assets/82417779/5bf9b343-ae64-4d21-97ac-ac5effe5817b)

**Complexity:** todo.

### 1. Calculate the sum
// TODO
  
## Important Formulas and Notations
Here are some important formulas used in this implementation:

**Gb = Gl + (Pl . Gr)**: The `Generate` of a block equals the `Generate` of the `left side` **OR** the (`Propagate` of the `left side` **AND** `Generate` of the `right side`)

**Pb = Pl . Pr**: The `Propagate` of a block equals the `Propagate` of the `left side` **AND** the `Propagate` of the `right side`

**P = x + y** (x OR y): Propagate of a block of 1-bit (Can also work with x XOR y)

**G = x . y** (x AND y): Genreate of a block of 1-bit
