# Carry Look Ahead Adder - HPC
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
We've made two different approaches to implement the carry look ahead algorithm. One is **recursive**, in which we divide the problem into different stages, and each stage contains a different set of binary blocks (portions of the initial ‘a’ and ‘b to be added). After implementing the recursive approach, we found out that we don’t need other stages than 0 to make the addition, so the other approach is the **iterative** approach, in which we used a simple for loop to initialize the Propagate and Generate values, on one hand, and also to make the sum, on the other hand.
### 1. Recursive approach
#### 1.1. Calculate the Propagate and the Generate
We're going to use two matrices to hold the values of **Propagate** and **Generate**, in order to avoid calculating the P and G for the same block twice. Here's an example of that logic for two **4-bit** numbers `a=1010` and `b=1100`:

![genrate-progapage-hpc-2 drawio](https://github.com/waterflow80/Carry-look-ahead-adder-HPC/assets/82417779/20f2c938-5143-4bb4-b703-94a1972e3df0)

Both `P` and `G` matrices will be of size `[log(N)+1][N]`, where **N** is the number of bits in each number (assuming both numbers are of equal number of bits). 

Here's the content of those two matrices after executing the program for that same example:

![Screenshot from 2024-05-03 17-37-34](https://github.com/waterflow80/Carry-look-ahead-adder-HPC/assets/82417779/5bf9b343-ae64-4d21-97ac-ac5effe5817b)

**Complexity:** todo.

### 1.2. Calculate the sum
Now that we have the `Propagate` and the `Genreate` of each block, we can make the sum of each block independently, without waiting for carry out of the previous block. Using the Carry formulas described [here](https://www.cs.umd.edu/~meesh/cmsc311/clin-cmsc311/Lectures/lecture22/lookahead.pdf).

### 2. Iterative approach
#### 2.1. Calculate the Propagate and the Generate
For the iterative approach, it's basically the same idea as the recursive one but for only stage 0. So We're going to use two arrays to hold the values of **Propagate** and **Generate**. Here's an example of that logic for two **4-bit** numbers `a=1010` and `b=1100`:

![Copy of hpc-carry-look-ahead-adder2-iter drawio](https://github.com/waterflow80/Carry-look-ahead-adder-HPC/assets/82417779/b6777fa9-2272-4ed0-a778-ea14addc86a0)


Both `P` and `G` arrays will be of size `[N]`, where **N** is the number of bits in each number (assuming both numbers are of equal number of bits).

Here's the content of those two arrays after executing the program for that same example:

![Screenshot from 2024-05-14 17-43-58](https://github.com/waterflow80/Carry-look-ahead-adder-HPC/assets/82417779/c57142b7-2834-4f82-ac8f-a58b104cee5b)

### 2.2. Calculate the sum
So the logic for the iterative approach, is basically the same as the recursive one, except that we'll only use stage 0 blocks. We'll be using a simple for loop.

## The Code
In this section we'll put some explanations about some terms used in the code:
- `bool getFirstPatternVal(int k, int N){}`: this will return the value of the following section of the Ci formula:
   ![first-pattern](https://github.com/waterflow80/Carry-look-ahead-adder-HPC/assets/82417779/b4269967-5cbe-468e-8568-55875db603e2)

Here, i=k=4 (in our code). You can learn more about the carry look ahead carries' formulas [here](https://www.cs.umd.edu/~meesh/cmsc311/clin-cmsc311/Lectures/lecture22/lookahead.pdf).

- `bool getSecondPatternVal(int k, int c0)`: this will return the value of the following section of the Ci formula:
  ![second-pattern](https://github.com/waterflow80/Carry-look-ahead-adder-HPC/assets/82417779/17a43bc2-b656-4b89-a00a-8702d6e58e43)

**Note**: if you want to change the arguments, you can do it by referring to: `a`, `b`, `N` **Important** (you should update the N). 

**Note**: `N` should be a **power of 2**.
## Parallelization
Now that we have a much less decoupling dependecy graph, we can profit from **Parallelization** using **OpenMP** directives.
### Parallelizing the recursive approach
We used `OMP Tasks` to parallelize both the Propagate and Generate initialization and the addition process. Here's a portion of how we implemented it in th code:
```cpp
// main() 
#pragma omp parallel
{
   #pragma omp single
   {
      init_P_and_G(a, b, log2(N), 0, N-1);
   }
}
...

#pragma omp parallel
{
   #pragma omp single
   {
      full_adder(a, b, c0, s, 0, N-1, log2(N), N);
   }
}
```
```cpp
// init_P_and_G()
#pragma omp task shared(P) shared(G)
{
   init_P_and_G(a, b, stage - 1, l, (l+r)/2); // left
}
#pragma omp task shared(P) shared(G)
{
   init_P_and_G(a, b, stage - 1, (l+r)/2+1, r); // right
}
```
```cpp
// full_adder()
#pragma omp task shared(s) 
{
   full_adder(a, b, c0, s, l, (l+r)/2, stage-1, N);
}
#pragma omp task shared(a)
{
   full_adder(a, b, c0, s, (l+r)/2+1, r, stage-1, N);
}
```
### Parallelizing the iterative approach
For the iterative approach, we can use two ways of paralellization: using `prallel for` or using `tasks`.
#### Using Parallel for
```cpp
// init_P_and_G()
#pragma omp parallel for
for (int i=n-1; i>=0; i--) {
   //printf("Thread %d\n", omp_get_thread_num());  
   P[i] = a[i] - '0' || b[i] - '0';
   G[i] = a[i] - '0' && b[i] - '0';
}
```
```cpp
// full_adder()
...
#pragma omp parallel for
for (int i=N-2; i>=0; i--) {
   // other bits than the less significant
   int j = N-i-1;
...
}
```
#### Using Tasks
//TODO
## Evaluation & Comparison
The `benchmark.sh` is a bash script that executes the differnt approaches, both the sequential and the parallel one, and displays the execution time for each.
However, we still have to update it to make it free the cache after each execution to avoid and misleading values.
Here's a sample output of the `benchmark.sh` script:

![Screenshot from 2024-05-15 00-52-20](https://github.com/waterflow80/Carry-look-ahead-adder-HPC/assets/82417779/5f2a1cc4-fadf-4b1b-a51a-92f70ff5e6df)

## Important Formulas and Notations
Here are some important formulas used in this implementation:

**Gb = Gl + (Pl . Gr)**: The `Generate` of a block equals the `Generate` of the `left side` **OR** the (`Propagate` of the `left side` **AND** `Generate` of the `right side`)

**Pb = Pl . Pr**: The `Propagate` of a block equals the `Propagate` of the `left side` **AND** the `Propagate` of the `right side`

**P = x + y** (x OR y): Propagate of a block of 1-bit (Can also work with x XOR y)

**G = x . y** (x AND y): Genreate of a block of 1-bit

## Finally
If you find it useful, please consider giving it a star :)
