/**
 * ----- Carry Look Ahead Adder -----
 * Important formulas:
 * - [ Gb = Gl + (Pl . Gr) ] => The 'Generate' of a block equals the 'Generate' of the left side OR the ('Propagate' of the left side AND 'Generate' of the right side)
 * - [ Pb = Pl . Pr] => The 'Propagate' of a block equals the 'Propagate' of the left side AND the 'Propagate' of the right side
 * - P = x + y (x OR y). Can also work with x XOR y (Propagate of a block of 1-bit)
 * - G = x . y (x AND y) (Genreate of a block of 1-bit)*/ 

#include<bits/stdc++.h>
#include<omp.h>

using namespace std;


const int N=128; // The number of bits of each number to add (this will be input by the user)

// Input Data
string a = "1100100110100110001111000111111001011000101011110110100010011100110001101010010110100100110101001111000111011111011101000111111111110";
string b = "1010111111011010100110011011001010101110100100111000101111011010110010101011110100100111111101001110111011110111100011001011001101";
int c0 = 0; // initial carry in

// Utils
bool** P; // Propagate
bool** G; // Generate


bool** init_2D_array(int nRows, int nCols) {
	bool **a = (bool**) malloc(nRows * sizeof(bool*));
	for (int i=0; i<nRows; i++) {
		a[i] = (bool*) malloc(nCols * sizeof(bool));
		for (int j=0; j<nCols; j++) {
			a[i][j] = NULL;
		}
	}
	return a;
}

void display_2D_array(bool** a, int nRows, int nCols) {
	cout << "------------------------------\n";
	for (int i=0; i<nRows; i++) {
		for (int j=0; j<nCols; j++) {
			cout << a[i][j] << " | ";
		}
		cout << endl;
	}
	cout << "------------------------------\n";
}

/**
 * -> This function assumes that the P and G matrices are initialized (with false)
 * -> This function will initialize the Generate & the Propagate values for each adder block
 * -> For N-bit numbers, we'll have log(N) stages. In other words, the depth of the adders' blocks' 
 * tree will be equal to log(N)
 * -> l: the left index of the current working block (at first call, l=0)
 * -> r: the right index of the current working block (at first call, r=N-1)
 * -> The complexity of this function is O(?) TODO: is it O(N) ? we should calculate it
 * -> The block id is defined by [stage][leftIndx]*/
void init_P_and_G(string& a, string& b, int stage, int l, int r) {
	if (stage == 0) {
		// In this case r=-1, We only have 1-bit block
		P[stage][l] = a[l] - '0' || b[l] - '0';
		G[stage][l] = a[l] - '0' && b[l] - '0';
		
	} else if (stage == 1) {
		// The next recursive call will process 1-bit blocks
		init_P_and_G(a, b, stage - 1, l, -1); // left side of the block
		init_P_and_G(a, b, stage - 1, r, -1); // right side of the block	
		//printf("Thread %d\n", omp_get_thread_num());
		P[stage][l] = P[stage-1][l] && P[stage-1][r];
		G[stage][l] = G[stage-1][l] || (P[stage-1][l] && P[stage-1][r]);
	} else {
		// stage > 1	
		#pragma omp task shared(P) shared(G)
		{		
			init_P_and_G(a, b, stage - 1, l, (l+r)/2); // left

		}
		#pragma omp task shared(P) shared(G)
		{		
			init_P_and_G(a, b, stage - 1, (l+r)/2+1, r); // right
		}
		P[stage][l] = P[stage-1][l] && P[stage-1][(l+r)/2+1];
		G[stage][l] = G[stage-1][l] || (P[stage-1][l] && G[stage-1][(l+r)/2+1]);
	}
}

/**
 * Return the value for the first pattern in the i_th carryIn formula
 * See: https://www.cs.umd.edu/~meesh/cmsc311/clin-cmsc311/Lectures/lecture22/lookahead.pdf to learn more on
 * how to calculate Ck
 * N: the number of bits that compose a and b
 * NOTE: This is currently working only for stage=0 (blocks of 1-bit). You can notice the G[0][...]*/
bool getFirstPatternVal(int k, int N) {
	bool temp = 1;
	int n = k-1;
	int i = n;
	int start = 1;
	while (i >= 1) {
		for (int j=start; j<=n; j++) {
			temp = temp && P[0][N-j-1]; // N-j-1 to get the reverse order. Eg P0 (Propagate0) will be P[N-1] in our P array representation 
		}
		// add g[k-i-1]
		temp = temp && G[0][N-(n-i)-1];
		i--;
		start++;
	}
	return temp;
}

/*
 * *Return the value for the first pattern in the i_th carryIn formula
*/
bool getSecondPatternVal(int k, int c0) {
	bool temp = 1;
	for (int i=0; i<k; i++) {
		temp = temp && P[0][N-i-1];
	}
	temp = temp && c0;
	return temp;
}

/**
 * Add two bits and don't consider the carry*/
bool binaryAddition(bool a, bool b) {
	if (a == 1 && b == 1) 
		return 0;
	return (a || b);
}

/**
 * Perform a full adder using the Propagate and Genereate arrays to calculate the carry in for each block.
 * The Generate and Propagate arrays should be initialized before calling this function.
 * s: will contain the result of the final addition
 * N: the number of bits of which composed a and b
 * For more information about how to calculate the carry, plese see: https://www.cs.umd.edu/~meesh/cmsc311/clin-cmsc311/Lectures/lecture22/lookahead.pdf
 * TODO this function should return the carry out of the added block (we'll only need the last one)*/
void full_adder(string &a, string &b, bool c0, bool* s, int l, int r, int stage, int N) {
	if (stage == 0) {
		// r = -1
		if (l == N-1) {
			// we're at the least significant bit
			s[l] = binaryAddition(binaryAddition(a[l] - '0', b[l] - '0'), c0);
			//TODO return the carry out	
		} else {
			// other bits than the less significant
			int i = N-l-1; // This is the index of the i_th carry we're looking for to perform the addition
			bool ci = 0;  // This is the i_th carry we're looking for to perform the addition
			ci = ci || G[0][l+1]; // l+1 because i-1 means less significant, so we have to advance in bits to the right (is is reversed)
			if (i == 1) {
				// we can calculate C1 using C0
				ci += P[0][N-1] && c0;
			} else {
				ci = ci || getFirstPatternVal(i, N);
				ci = ci || getSecondPatternVal(i, c0);
				//s[l] = a[l] - '0' || b[l] - '0' || ci;
			}
			s[l] = binaryAddition(binaryAddition(a[l] - '0', b[l] - '0'), ci);
			//TODO return the carry out
		}
	} else {
		// stage > 0
		#pragma omp task shared(s) 
		{
			full_adder(a, b, c0, s, l, (l+r)/2, stage-1, N);
		}
		#pragma omp task shared(s)
		{
			full_adder(a, b, c0, s, (l+r)/2+1, r, stage-1, N);
		}
		// TODO return the carry out
	}	
}


int main() {
	P = init_2D_array(log2(N)+1 ,N);
	G = init_2D_array(log2(N)+1, N);

	// Parallelization
	#pragma omp parallel
	{
		#pragma omp single
		{
			init_P_and_G(a, b, log2(N), 0, N-1);	
		}
	}	

	//init_P_and_G(a, b, log2(N), 0, N-1);
	//cout << "Propagate:\n";
	//display_2D_array(P, log2(N)+1 ,N);
	//cout << "Generate:\n";
	//display_2D_array(G, log2(N)+1 ,N);

	#pragma omp barrier // Wait for the P and G to complete initialization
	cout << "------ADDITION------\n";

	bool* s = (bool*) malloc(N * sizeof(bool));
	#pragma omp parallel
	{
		#pragma omp single
		{
			full_adder(a, b, c0, s, 0, N-1, log2(N), N);
		}
	}

	
	for (int i=0; i<N; i++) {
		cout << s[i] << " | ";
	}
	cout << endl;

}
