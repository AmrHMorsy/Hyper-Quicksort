#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#define MASTER 0

int N, ID, d ;
MPI_Status status;

// This function swaps two entries in the array. This will be used in the partitioning step
void Swap( int * A, int * B ) ;

// This function checks if the array is sorted or not
bool IsSorted( int * A, int n ) ;

// This function generates random number
int * GenerateRandomNumbers( int n ) ;

// Partition function, called by the sequential quick sort algorithm function SeqQuickSort()
int Partition( int * A, int l, int h ) ;

// Sequential Quick Sort Algorithm
void SeqQuickSort( int * A, int l, int h ) ;

// This function is responsible for displaying final results: Execution Time - Was the array sorted or not ?
void Display( bool IsSorted, double t, int n ) ;

//Hypercube Quick Sort Algorithm
int * HyperQuickSort( int * A, int l, int h, int k, int * recv_data, int * G, int * send_data ) ;
