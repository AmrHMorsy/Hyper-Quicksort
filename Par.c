#include "Par.h"


int main (int argc, char *argv[])
{
    double t ;
    int n, p, *A, i, * tmp, * Buffer, * final, * C, h, counter = 0, size, total = 0, * recv_data, * G, * send_data ;

    // Initialize MPI
    MPI_Init(&argc,&argv) ;
    
    // Get the number of processes
    MPI_Comm_size(MPI_COMM_WORLD,&p) ;
    
    // Get the ID of the process
    MPI_Comm_rank(MPI_COMM_WORLD,&ID) ;
    
    // Calculate the dimensions of the hypercube
    d = log((double)p) / log(2) ;
    
    // The input size of the array must be entered
    if( argc < 2 ){
        if( ID == MASTER )
            printf("\033[1;31m \nERROR: MISSING COMMAND LINE ARGUMENTS \nFORMAT: mpirun -np <p> Par <n> \n\n \033[0m") ;
    }
    
    // The input size of the array must be greater than the number of processes
    else if( atoi(argv[1]) < p ){
        if( ID == MASTER )
            printf("\033[1;31m \nERROR: NUMBER OF PROCESSES MUST BE LESS THAN n\n\n \033[0m") ;
    }
    
    // The number of processes must be a power of 2
    else if( pow(2,d) != p ){
        if( ID == MASTER )
            printf("\033[1;31m \nERROR: NUMBER OF PROCESSES MUST BE A POWER OF 2\n\n \033[0m") ;
    }
    else{
        // Input Size
        n = atoi(argv[1]) ;
        N = n ;
        
        // Recv_Data is the array to store the message recieved from other processes during HyperCubeQuickSort algorithm
        recv_data = (int*)malloc(sizeof(int)*N) ;
        
        // G is the array to store the message recieved from other processes during HyperCube QuickSort algorithm
        G = (int*)malloc(sizeof(int)*N) ;
        
        // Size is the portion of the array the process is responsible for
        size = n/p ;
                
        // This is the array in which all the arrays from all the processes are merged
        final = (int*)malloc(sizeof(int)*(N)) ;
        tmp = GenerateRandomNumbers(n) ;
        Buffer = (int*) malloc(sizeof(int)*N) ;
        
        // This is the array that will be used to store the message that will be sent to other processes
        send_data = (int*) malloc( sizeof(int) * N ) ;
        
        // This condition handles the situation in which the input size doesn't divide the number of processes
        if( (ID==MASTER) && (n%p != 0) )
            size = N - ((p-1)*size) ;
        
        // This is the array that will store the portion of the array to sort
        A = (int*) malloc(sizeof(int)*(size)) ;
        
        // Fill the array with the elements to be sorted
        for( i = (ID*(size)) ; i < ((ID*(size))+(size)) ; i++ )
            A[i-(ID*(size))] = tmp[i] ;
        
        // A barrier to ensure all the processes have arrived before starting the timer
        MPI_Barrier( MPI_COMM_WORLD ) ;
        if( ID == MASTER )
            t = MPI_Wtime() ;
        C = HyperQuickSort(A, 0, size-1, d-1, recv_data, G,send_data) ;
        // A barrier to ensure all the processes have arrived before ending the timer
        MPI_Barrier( MPI_COMM_WORLD ) ;
        if( ID == MASTER )
            t = MPI_Wtime() - t ;
        
        // Merging the arrays together. Every process will send to the MASTER their portion. The MASTER will collect these arrays and stack them in the array final and then pass it the IsSorted Function to check that it is sorted
        if( ID != MASTER )
            MPI_Send(C, N, MPI_INT, MASTER, 1, MPI_COMM_WORLD );
        else{
            for( h = 0 ; h < N ; h++ ){
                if( C[h] == -1 )
                    break ;
                final[counter] = C[h] ;
                counter++ ;
            }
            // The MASTER process recieves the arrays by order from process 1 to process P
            for( i = 1 ; i < p ; i++ ){
                MPI_Recv(Buffer, N, MPI_INT, i, 1, MPI_COMM_WORLD, &status ) ;
                for( h = 0 ; h < N ; h++ ){
                    if( Buffer[h] == -1 )
                        break  ;
                    final[counter] = Buffer[h] ;
                    counter++ ;
                }
            }
        }
        // Display the final result
        if( ID == MASTER )
            Display( IsSorted(final,N), t, n) ;
    }
    MPI_Finalize() ;
    
    return 0 ;
}

// Sequential Quick Sort Algorithm
void SeqQuickSort( int * A, int l, int h )
{
    if( l < h ){
        int P = Partition(A,l,h) ;
        SeqQuickSort(A,l,P-1) ;
        SeqQuickSort(A,P+1,h) ;
    }
}

// Partition function, called by the sequential quick sort algorithm function SeqQuickSort()
int Partition( int * A, int l, int h )
{
    int j, i = l-1, P = A[h] ;
    for ( j = l; j <= h-1; j++ ) {
        if (A[j] < P){
            i++ ;
            Swap(&A[i], &A[j]) ;
        }
    }
    Swap(&A[i+1], &A[h]) ;
    return i+1 ;
}

//Hypercube Quick Sort Algorithm
int * HyperQuickSort( int * A, int l, int h, int k, int * recv_data, int * G, int * send_data )
{
    // If recursion depth is equal to d( dimensions of hypercube), stop the recursion and call sequential quick sort algorithm
    // Each process will sort its portion sequentially
    if( k == -1 ){
        SeqQuickSort(A,l,h) ;
        return A ;
    }
    // The master process will define the pivot
    int dest, j = 1, i = l-1, p, pivot = (ID==MASTER)?A[h]:0, z = 0 ;
    
    // The master process will broadcast the pivot to all the other processes
    MPI_Bcast(&pivot,1,MPI_INT,MASTER,MPI_COMM_WORLD) ;
    // If the master is idle ( doesn't have any element ), the pivot will be invalid. In this case, the next process will pick the pivot and broadcast it to the other processes. This is repeated till a valid pivot is picked
    while( pivot <= 0 ){
        pivot = (ID==j)?A[h]:0 ;
        MPI_Bcast(&pivot,1,MPI_INT,j,MPI_COMM_WORLD) ;
        j++ ;
    }
    // Each process will partition its array around the pivot
    for ( j = l; j <= h-1; j++ ) {
        if (A[j] < pivot){
            i++ ;
            Swap(&A[i], &A[j]) ;
        }
    }
    Swap(&A[i+1], &A[h]) ;
    p = i+1 ;
    if( A[p] < pivot && h >= l )
        p++ ;
        
    // This is the communication step.
    if( ((ID>>k)&1) == 0 ){
        // The process calculates the destination process in which it will send the portion of the array to
        dest = ID + pow(2,k) ;
        // The process copies the portion of the array to send to send_data array
        memcpy( send_data, A+p, sizeof(int)*(h-p+1)) ;
        // The process deleted the elements of the array that will be sent
        for( j = p ; j <= h ; j++ )
            A[j] = -1 ;
    }
    else{
        // The process calculates the destination process in which it will send the portion of the array to
        dest = ID - pow(2,k) ;
        // The process copies the portion of the array to send to send_data array
        memcpy( send_data, A+l, sizeof(int)*(p-l)) ;
        // The process deleted the elements of the array that will be sent
        for( j = l ; j < p ; j++ )
            A[j] = -1 ;
    }

    for( j = 0 ; j < N ; j++ ){
        recv_data[j] = -1 ;
        G[j] = -1 ;
    }
    
    // Along the kth communication link, processes with 0 kth bit will send processes with  1 kth bit the portion of the array greater than the pivot
    // Along the kth communication link, processes with 0 kth bit will receive from processes with 1 kth bit the portion of the array smaller than the pivot
    if( ((ID>>k)&1) == 0 ){
        MPI_Send(send_data, h-p+1, MPI_INT, dest, 1, MPI_COMM_WORLD );
        MPI_Recv(recv_data, N, MPI_INT, dest, 1, MPI_COMM_WORLD, &status ) ;
    }
    // Along the kth communication link, processes with 1 kth bit will send processes with 0 kth bit the portion of the array smaller than the pivot
    // Along the kth communication link, processes with 1 kth bit will receive from processes with 0 kth bit the portion of the array greater than the pivot
    else{
        MPI_Recv(recv_data, N, MPI_INT, dest, 1, MPI_COMM_WORLD, &status ) ;
        MPI_Send(send_data, p-l, MPI_INT, dest, 1, MPI_COMM_WORLD );
    }
    
    // Each process will merge the recieved elements with the elements they already have stored
    j = 0 ;
    while( recv_data[j] != -1 ){
        G[z] = recv_data[j] ;
        z++ ;
        j++ ;
    }
    for( j = l ; j <= h ; j++ ){
        if( A[j] != -1 ){
            G[z] = A[j] ;
            z++ ;
        }
    }
    // Recursion call to operate in the next hypercube dimension
    return HyperQuickSort( G, 0, z-1, k-1, recv_data, G, send_data ) ;
}

// This function swaps two entries in the array. This will be used in the partitioning step
void Swap( int * A, int * B )
{
    int T = *A ;
    *A = *B ;
    *B = T ;
}

// This function generates random number
int * GenerateRandomNumbers( int n )
{
    int i ;
    int * A = (int *) malloc(sizeof(int)*n) ;
    for( i = 0 ; i < n ; i++ )
        A[i] = rand()%n ;
    return A ;
}

// This function checks if the array is sorted or not
bool IsSorted( int * A, int n )
{
    int i ;
    for( i = 0 ; i < n-1 ; i++ ){
        if( A[i] > A[i+1] )
            return false ;
    }
    return true ;
}

// This function is responsible for displaying final results: Execution Time & Was the array sorted or not ?
void Display( bool IsSorted, double t, int n )
{
    printf("\n\n------------------------------------\n") ;
    printf("\033[1;35mHypercube Dimension = %d \n\033[0m", d) ;
    printf("\033[1;36mInput Size = %d \n\033[0m", n) ;
    if( IsSorted )
        printf("\033[1;32mArray Is Sorted ✓\n\033[0m") ;
    else
        printf("\033[1;31mArray Is Not Sorted X\n\033[0m") ;
    printf("\033[1;31mExecution Time = %lf seconds \n\033[0m", t ) ;
    printf("------------------------------------\n\n\n") ;
}
