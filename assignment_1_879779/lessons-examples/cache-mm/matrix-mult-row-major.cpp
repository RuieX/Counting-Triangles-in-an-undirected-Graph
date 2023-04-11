#include <iostream>   	// input output
#include <chrono>		// prendere i tempi
#include <cstdlib>		// random
#include <vector>		// vector


//  g++-6 -std=c++11 -march=native -O3 matrix-mult-row-major.cpp -o mm-row-major 


int main (int argc, char* argv[]) {

	if (argc!=2) {
		std::cout << " !! Error !! " << std::endl;
		std::cout << " !! Usage: " << std::endl;
		std::cout << "\t ./mm-row-major <matrix_rows_cols>" << std::endl;
		return 1;
	}

	long rows = atol(argv[1]);
	long size = rows*rows;

	std::cout << "Matrix row/cols : " << rows << std::endl;
	std::cout << "Matrix size     : " << size << std::endl;
	std::cout << "Matrix size (MB): " << size*sizeof(double)/1000.0/1000.0 << std::endl;


	// init matrices
	std::cout << " + Initialization " << std::endl;
	std::vector<double> a (size);
	std::vector<double> b (size);
	std::vector<double> c (size, 0.0);

	srand(42);
	for (long i=0; i<size; i++) {
		a[i] = (rand() % 3) - 1; // -1, 0 , 1
		b[i] = (rand() % 3) - 1; // -1, 0 , 1
	}
	std::cout << " - End of initialization " << std::endl;

	// --------------------------------------
	// compute Row-Maj matrix multiplication

	// initializzo chrono
	auto begin = std::chrono::high_resolution_clock::now();


	for (long row=0; row<rows; row++) {
		for (long col=0; col<rows; col++) {
			// compute c[row,col] 
			for (long i=0; i<rows; i++) {
				c[row*rows + col] += a[row*rows + i] * b[i*rows + col];
			}
		}		
	}

	// prendo i tempi
	auto end = std::chrono::high_resolution_clock::now();
	auto elapsed = 
		std::chrono::duration_cast<std::chrono::milliseconds>
		(end - begin);

	// compute check value
	double check = 0.0;
	for (long i=0; i<size; i++) {
		check += c[i];
	}	

	std::cout << "-------------------------------" << std::endl;
	std::cout << "Row major Mat Mul" << std::endl;
	std::cout << "Time     : " << elapsed.count() << "ms." << std::endl;
	std::cout << "Check value = " << check << std::endl;
	std::cout << std::endl;

}