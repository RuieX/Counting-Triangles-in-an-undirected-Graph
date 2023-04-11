#include <iostream>  
#include <chrono>
#include <cstdlib>
#include <vector>

// g++-6 -std=c++11 -fopenmp -O3 matrixmult.cpp -o matrixmult
// srun -v -I -c 38 ./matrixmult 1600 32


int main (int argc, char* argv[]) {
	// leggo il numero di elementi da linea di comando
	if (argc!=3) {
		std::cout << " !! Error !! " << std::endl;
		std::cout << " !! Usage: " << std::endl;
		std::cout << "\t ./matmul <matrix_rows_cols> <num_threads>" << std::endl;
		return 1;
	}

	long rows = atol(argv[1]);
	long size = rows*rows;
	int num_threads = atoi(argv[2]);

	std::cout << "Matrix row/cols : " << rows << std::endl;
	std::cout << "Matrix size     : " << size << std::endl;
	std::cout << "Matrix size (MB): " << size*sizeof(double)/1000.0/1000.0 << std::endl;
	std::cout << "Max Num Threads : " << num_threads << std::endl;


	// init matrices
	std::cout << " + Initialization " << std::endl;
	std::vector<double> a (size);
	std::vector<double> b (size);
	std::vector<double> c;

	for (long i=0; i<size; i++) {
		a[i] = (rand() % 3) - 1; // -1, 0 , 1
		b[i] = (rand() % 3) - 1; // -1, 0 , 1
	}
	std::cout << " - End of initialization " << std::endl;

	
	// --------------------------------------
	// compute Col-Maj matrix multiplication
	// assume B is already ok

	// reset c
	std::vector<double>(size, 0.0).swap(c);

	// initializzo chrono
	auto begin = std::chrono::high_resolution_clock::now();

	for (long row=0; row<rows; row++) {
		for (long col=0; col<rows; col++) {
			// compute c[row,col] 
			for (long i=0; i<rows; i++) {
				c[row*rows + col] += a[row*rows + i] * b[col*rows + i];
			}
		}		
	}

	// prendo i tempi
	auto end = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds> (end - begin);

	// compute check value
	double check = 0.0;
	for (long i=0; i<size; i++) {
		check += c[i];
	}	

	std::cout << "-------------------------------" << std::endl;
	std::cout << "Col major Mat Mul" << std::endl;
	std::cout << "Time     : " << elapsed.count() << "ms." << std::endl;
	std::cout << "Check value = " << check << std::endl;
	std::cout << std::endl;

	// ----------------------------------------------
	// parallelize external loop ( B must be shared )
	// reset c
	std::vector<double>(size, 0.0).swap(c);

	// initializzo chrono
	begin = std::chrono::high_resolution_clock::now();

	#pragma omp parallel for num_threads(num_threads)
	for (long row=0; row<rows; row++) {
		for (long col=0; col<rows; col++) {
			// compute c[row,col] 
			for (long i=0; i<rows; i++) {
				c[row*rows + col] += a[row*rows + i] * b[col*rows + i];
			}
		}		
	}

	// prendo i tempi
	end = std::chrono::high_resolution_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds> (end - begin);

	// compute check value
	check = 0.0;
	for (long i=0; i<size; i++) {
		check += c[i];
	}	

	std::cout << "-------------------------------" << std::endl;
	std::cout << "Parallel on A rows" << std::endl;
	std::cout << "Time     : " << elapsed.count() << "ms." << std::endl;
	std::cout << "Check value = " << check << std::endl;
	std::cout << std::endl;


	// ----------------------------------------------
	// parallelize internal loop ( B is split among different CPUs more threading overhead)
	
	// reset c
	std::vector<double>(size, 0.0).swap(c);

	// initializzo chrono
	begin = std::chrono::high_resolution_clock::now();

	for (long row=0; row<rows; row++) {
		#pragma omp parallel for num_threads(num_threads)
		for (long col=0; col<rows; col++) {
			// compute c[row,col] 
			for (long i=0; i<rows; i++) {
				c[row*rows + col] += a[row*rows + i] * b[col*rows + i];
			}
		}		
	}

	// prendo i tempi
	end = std::chrono::high_resolution_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds> (end - begin);

	// compute check value
	check = 0.0;
	for (long i=0; i<size; i++) {
		check += c[i];
	}	

	std::cout << "-------------------------------" << std::endl;
	std::cout << "Parallel on B cols" << std::endl;
	std::cout << "Time     : " << elapsed.count() << "ms." << std::endl;
	std::cout << "Check value = " << check << std::endl;
	std::cout << std::endl;


	// ----------------------------------------------
	// parallelize over cols of B, create threads once, and improve locality of B

	// reset c
	std::vector<double>(size, 0.0).swap(c);

	// initializzo chrono
	begin = std::chrono::high_resolution_clock::now();


	#pragma omp parallel for num_threads(num_threads)
	for (long col=0; col<rows; col++) {
		for (long row=0; row<rows; row++) {
			// compute c[row,col] 
			for (long i=0; i<rows; i++) {
				c[row*rows + col] += a[row*rows + i] * b[col*rows + i];
			}
		}
	}

	// prendo i tempi
	end = std::chrono::high_resolution_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds> (end - begin);

	// compute check value
	check = 0.0;
	for (long i=0; i<size; i++) {
		check += c[i];
	}	

	std::cout << "-------------------------------" << std::endl;
	std::cout << "Parallel on B cols II" << std::endl;
	std::cout << "Time     : " << elapsed.count() << "ms." << std::endl;
	std::cout << "Check value = " << check << std::endl;
	std::cout << std::endl;


	// ----------------------------------------------
	// ----------------------------------------------
	// parallelize over cols of B, create threads once, 
	// improve locality of B and of A

	// reset c
	std::vector<double>(size, 0.0).swap(c);

	// initializzo chrono
	begin = std::chrono::high_resolution_clock::now();

	#pragma omp parallel num_threads(num_threads)
	for (long row=0; row<rows; row++) {
		#pragma omp for schedule(static)
		for (long col=0; col<rows; col++) {
			// compute c[row,col] 
			for (long i=0; i<rows; i++) {
				c[row*rows + col] += a[row*rows + i] * b[col*rows + i];
			}
		}
		if (row%100==0) {
			#pragma omp barrier
		}
	}

	// prendo i tempi
	end = std::chrono::high_resolution_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds> (end - begin);

	// compute check value
	check = 0.0;
	for (long i=0; i<size; i++) {
		check += c[i];
	}	

	std::cout << "-------------------------------" << std::endl;
	std::cout << "Parallel on B cols III" << std::endl;
	std::cout << "Time     : " << elapsed.count() << "ms." << std::endl;
	std::cout << "Check value = " << check << std::endl;
	std::cout << std::endl;


	// ----------------------------------------------
	// parallelize most internal loop ( each proc gets one line of A and of B)
	// reset c
	std::vector<double>(size, 0.0).swap(c);

	// initializzo chrono
	begin = std::chrono::high_resolution_clock::now();

	for (long row=0; row<rows; row++) {
		for (long col=0; col<rows; col++) {
			// compute c[row,col] 
			double c_ij = 0;
			#pragma omp parallel for num_threads(num_threads) reduction(+:c_ij)
			for (long i=0; i<rows; i++) {
				c_ij += a[row*rows + i] * b[col*rows + i];
			}
			c[row*rows + col] = c_ij;
		}		
	}

	// prendo i tempi
	end = std::chrono::high_resolution_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds> (end - begin);

	// compute check value
	check = 0.0;
	for (long i=0; i<size; i++) {
		check += c[i];
	}	

	std::cout << "-------------------------------" << std::endl;
	std::cout << "Parallel on C[i,j] rows" << std::endl;
	std::cout << "Time     : " << elapsed.count() << "ms." << std::endl;
	std::cout << "Check value = " << check << std::endl;
	std::cout << std::endl;


}