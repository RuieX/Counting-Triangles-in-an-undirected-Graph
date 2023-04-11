#include <iostream>   	// input output
#include <chrono>		// prendere i tempi
#include <cstdlib>		// random
#include <vector>		// vector


// g++-6 -std=c++11 -march=native -O3 matrix-mult-bw-col.cpp -o mm-bw



int main (int argc, char* argv[]) {

	if (argc!=3) {
		std::cout << " !! Error !! " << std::endl;
		std::cout << " !! Usage: " << std::endl;
		std::cout << "\t ./mm-bw-col <matrix_rows_cols> <blocksize>" << std::endl;
		return 1;
	}

	long rows = atol(argv[1]);
	long size = rows*rows;
	long block = atol(argv[2]);
	long block_size = block*block;
	const long n_blocks = rows / block;

	std::cout << "Matrix row/cols : " << rows << std::endl;
	std::cout << "Matrix size     : " << size << std::endl;
	std::cout << "Matrix size (MB): " << size*sizeof(double)/1000.0/1000.0 << std::endl;
	std::cout << "Block row/cols  : "  << block << std::endl;
	std::cout << "Block size      : "  << block_size << std::endl;
	std::cout << "Block size (MB) : "  << block_size*sizeof(double)/1000.0/1000.0 << std::endl;
	std::cout << "N blocks        : " << n_blocks << std::endl;


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


	// transpose b
	for (long row=0; row<rows; row++) {
		for (long col=row+1; col<rows; col++) {
			std::swap(b[row*rows + col], b[col*rows + row]);
		}
	}

	std::cout << " - End of initialization " << std::endl;

	// --------------------------------------
	// compute Row-Maj matrix multiplication

	// initializzo chrono
	auto begin = std::chrono::high_resolution_clock::now();


	// iterate over c_row blocks
	for (long c_row_block=0; c_row_block<n_blocks; c_row_block++) {
		// iterate over c_col blocks
		for (long c_col_block=0; c_col_block<n_blocks; c_col_block++) {
			// sum over a rows and b cols  in blocks
			for (long i_block=0; i_block<n_blocks; i_block++) {

				// same old loop
				// would be good to put this into a micro kernel
				for (long row=c_row_block*block; row<(c_row_block+1)*block; row++) {
					for (long col=c_col_block*block; col<(c_col_block+1)*block; col++) {
						for (long i=i_block*block; i<(i_block+1)*block; i++) {
							c[row*rows + col] += a[row*rows + i] * b[col*rows + i];
						}
					}		
				}
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
	std::cout << "Block-Wise Col major Mat Mul" << std::endl;
	std::cout << "Time     : " << elapsed.count() << "ms." << std::endl;
	std::cout << "Check value = " << check << std::endl;
	std::cout << std::endl;

}