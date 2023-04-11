#include <iostream>   	// input output
#include <chrono>		// prendere i tempi
#include <cstdlib>		// random
#include <vector>		// vector


// g++-6 -std=c++11 -march=native -O3 matrix-mult-bw-k


template <size_t BLK>
void micro_kernel ( double* __restrict__ a, 
					double* __restrict__ b, 
					double* __restrict__ c ) {
	size_t a_i = 0;
	size_t b_i = 0;
	size_t c_i = 0;

	for (size_t i=0; i<BLK; ++i) { // for each "row" in a
		b_i = 0;

		for (size_t j=0; j<BLK; ++j) { // for each "col" in a
			a_i = i*BLK;

			// below should be properly unrolled
			for (size_t k=0; k<BLK; ++k) { // for each item in the row/col
				c[c_i] += a[a_i+k] * b[b_i+k]; 
			}

			b_i += BLK;
			++c_i;
		}
	}
}


int main (int argc, char* argv[]) {

	if (argc!=2) {
		std::cout << " !! Error !! " << std::endl;
		std::cout << " !! Usage: " << std::endl;
		std::cout << "\t ./mm-bw-col <matrix_rows_cols>" << std::endl;
		return 1;
	}

	long rows = atol(argv[1]);
	long size = rows*rows;

	std::cout << "Matrix row/cols : " << rows << std::endl;
	std::cout << "Matrix size     : " << size << std::endl;
	std::cout << "Matrix size (MB): " << size*sizeof(double)/1000.0/1000.0 << std::endl;

	const long STATIC_BLOCK = 8;
	const long STATIC_BLOCK_SIZE = STATIC_BLOCK*STATIC_BLOCK;
	const long STATIC_N_BLOCKS = rows / STATIC_BLOCK;

	std::cout << "Block row/cols  : "  << STATIC_BLOCK << std::endl;
	std::cout << "Block size      : "  << STATIC_BLOCK_SIZE << std::endl;
	std::cout << "Block size (MB) : "  << STATIC_BLOCK_SIZE*sizeof(double)/1000.0/1000.0 << std::endl;
	std::cout << "N blocks: " << STATIC_N_BLOCKS << std::endl;


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

	// convert A to blockwise format
	std::vector<double> a_blockwise(size); 
	long a_i = 0;
	for (long c_row_block=0; c_row_block<STATIC_N_BLOCKS; c_row_block++) {
		// iterate over c_col blocks
		for (long c_col_block=0; c_col_block<STATIC_N_BLOCKS; c_col_block++) {
			// same old loop
			for (long row=c_row_block*STATIC_BLOCK; row<(c_row_block+1)*STATIC_BLOCK; row++) {
				for (long col=c_col_block*STATIC_BLOCK; col<(c_col_block+1)*STATIC_BLOCK; col++) {
					a_blockwise[a_i++] = a[row*rows + col];
				}		
			}
		}
	}

	// convert B to blockwise format
	// iterate over c_row blocks
	std::vector<double> b_blockwise(size); 
	long b_i = 0;
	for (long c_row_block=0; c_row_block<STATIC_N_BLOCKS; c_row_block++) {
		// iterate over c_col blocks
		for (long c_col_block=0; c_col_block<STATIC_N_BLOCKS; c_col_block++) {
			// same old loop
			for (long row=c_row_block*STATIC_BLOCK; row<(c_row_block+1)*STATIC_BLOCK; row++) {
				for (long col=c_col_block*STATIC_BLOCK; col<(c_col_block+1)*STATIC_BLOCK; col++) {
					b_blockwise[b_i++] = b[row*rows + col];
				}		
			}
		}
	}
	

	std::cout << " - End of initialization " << std::endl;

	// initializzo chrono
	auto begin = std::chrono::high_resolution_clock::now();


	// initializzo chrono
	begin = std::chrono::high_resolution_clock::now();


	// iterate over c_row blocks
	for (long c_row_block=0; c_row_block<STATIC_N_BLOCKS; c_row_block++) {
		// iterate over c_col blocks
		for (long c_col_block=0; c_col_block<STATIC_N_BLOCKS; c_col_block++) {
			// sum over a rows and b cols  in blocks
			for (long i_block=0; i_block<STATIC_N_BLOCKS; i_block++) {
				long a_start = (c_row_block*STATIC_N_BLOCKS + i_block)* STATIC_BLOCK_SIZE;
				long b_start = (c_col_block*STATIC_N_BLOCKS + i_block)* STATIC_BLOCK_SIZE;
				long c_start = (c_row_block*STATIC_N_BLOCKS + c_col_block) * STATIC_BLOCK_SIZE;

				micro_kernel<STATIC_BLOCK> (&a_blockwise[a_start], &b_blockwise[b_start], &c[c_start]);
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
	std::cout << "Block-Wise micro_kernel Mat Mul" << std::endl;
	std::cout << "Time     : " << elapsed.count() << "ms." << std::endl;
	std::cout << "Check value = " << check << std::endl;
	std::cout << std::endl;

}