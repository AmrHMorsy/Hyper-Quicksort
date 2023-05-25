# Hyper-QuickSort

This project provides an implementation of the QuickSort algorithm, designed to run in parallel on a d-dimensional hypercube topology. We utilize the Message Passing Interface (MPI) for inter-process communication, and the code is written in the C programming language.

## Motivation

The motivation behind this project is to explore the potentials of parallel sorting algorithms in a structured network topology, such as a d-dimensional hypercube. By utilizing MPI and C, this project aims to demonstrate the performance gains that can be achieved through parallel sorting, and how these gains scale with the dimensionality of the hypercube.

## Features

- QuickSort algorithm implementation
- MPI for effective inter-process communication
- Parallel execution in a d-dimensional hypercube topology
- Written in the versatile and efficient C language

## Implementation 

This parallel quicksort algorithm takes advantage of the topology of a p process hypercube connected parallel computer. Let n be the number of elements to be sorted and $p = 2^d$ be the number of processes in a d -dimensional hypercube. Each process is assigned a block of $n \over p$ elements, and the labels of the processes define the global order of the sorted sequence. 

The algorithm starts by selecting a pivot element, which is broadcast to all processes. Each process, upon receiving the pivot, partitions its local elements into two blocks, one with elements smaller than the pivot and one with elements larger than the pivot. Then the processes connected along the d th communication link exchange appropriate blocks so that one retains elements smaller than the pivot and the other retains elements larger than the pivot. Specifically, each process with a 0 in the d th bit (the most significant bit) position of the binary representation of its process label retains the smaller elements, and each process with a 1 in the d th bit retains the larger elements. After this step, each process in the (d - 1)- dimensional hypercube whose d th label bit is 0 will have elements smaller than the pivot, and each process in the other (d - 1)-dimensional hypercube will have elements larger than the pivot. 

This procedure is performed recursively in each subcube, splitting the subsequences further. After d such splits – one along each dimension – the sequence is sorted with respect to the global ordering imposed on the processes. This does not mean that the elements at each process are sorted. Therefore, each process sorts its local elements by using sequential quicksort.

<img width="695" alt="Screenshot 2023-01-14 at 11 57 45 AM" src="https://user-images.githubusercontent.com/56271967/212485147-783e7b6d-d569-4074-aae4-1db4a2e4bc3b.png">


## Project Setup 

### Prerequisites

Before running the program, ensure you have the following installed:

- An MPI implementation (e.g., OpenMPI or MPICH)
- A C compiler (e.g., gcc)

### Installation

1. Clone the repository:
```
git clone [https://github.com/AmrHMorsy/](https://github.com/AmrHMorsy/Hyper-Quicksort.git)
```
2. Navigate to the project directory: 
```
cd Hyper-Quicksort
```
3. Compile the program, 
```
  mpicc Par.c -o Par
```
4. Run the program,
```
  mpirun -np <NumberOfProcesses> Par <InputSize>
```

  
## Time Complexity 

The time complexity of the parallel quicksort algorithm on a d-dimensional hypercube topology is 

$$ O( {N \over P} + {{N \over P} log {N \over P}} + {{N \over P} log P} + {log^2 P} ) = O( {{N \over P} log {N \over P}} ) $$


## Results 

The execution time of the sequential quick sort algorithm is compared with the execution time of the parallel d-dimensional hypercube quick sort algorithm and the speedup is recorded. A graph of speedup against input size is shown below: 


<img width="965" alt="Screenshot 2023-05-24 at 23 28 32" src="https://github.com/AmrHMorsy/Hyper-Quicksort/assets/56271967/e87ef62d-f038-455d-a0d1-2c9cf7a8fa89">

## Acknowledgements

This project has benefited greatly from numerous resources, both online and offline. I would like to express my special thanks to the book "Introduction to Parallel Computing" by Ananth Grama, published by Addison-Wesley in 2013. The concepts and insights presented in this book have been invaluable in developing this implementation of the parallel QuickSort algorithm.

## Contribute

We appreciate all the help we can get! Feel free to issue pull requests or submit issues if you find something that could be improved.

## License

This project is licensed under the MIT License. See the LICENSE.md file for details.

