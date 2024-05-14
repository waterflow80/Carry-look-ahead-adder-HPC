/**
 * ----- Carry Look Ahead Adder -----
 * Important formulas:
 * - [ Gb = Gl + (Pl . Gr) ] => The 'Generate' of a block equals the 'Generate' of the left side OR the ('Propagate' of the left side AND 'Generate' of the right side)
 * - [ Pb = Pl . Pr] => The 'Propagate' of a block equals the 'Propagate' of the left side AND the 'Propagate' of the right side
 * - P = x + y (x OR y). Can also work with x XOR y (Propagate of a block of 1-bit)
 * - G = x . y (x AND y) (Genreate of a block of 1-bit)*/ 

#include<bits/stdc++.h>

using namespace std;


const int N=128; // The number of bits of each number to add (this will be input by the user)

// Input Data
string a = "1100100110100110001111000111111001011000101011110110100010011100110001101010010110100100110101001111000111011111011101000111111111110";
string b = "1010111111011010100110011011001010101110100100111000101111011010110010101011110100100111111101001110111011110111100011001011001101";
int c0 = 0; // initial carry in

// Utils
bool* P; // Propagate
bool* G; // Generate


bool* init_array(int n) {
	bool *a = (bool*) malloc(n * sizeof(bool));
	for (int i=0; i<n; i++) {
		a[i] = NULL;
	}
	return a;
}

void display_array(bool* a, int n) {
	cout << "------------------------------\n";
	for (int i=0; i<n; i++) {
			cout << a[i] << " | ";	
	}
	cout << "\n------------------------------\n";
}

// Working only on stage 0, because we don't other need stages in this implenentation
void init_P_and_G_iter(string& a, string& b, int n) {
	for (int i=n-1; i>=0; i--) {
		P[i] = a[i] - '0' || b[i] - '0';
		G[i] = a[i] - '0' && b[i] - '0';
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
			temp = temp && P[N-j-1]; // N-j-1 to get the reverse order. Eg P0 (Propagate0) will be P[N-1] in our P array representation 
		}
		// add g[k-i-1]
		temp = temp && G[N-(n-i)-1];
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
		temp = temp && P[N-i-1];
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


// Consider only stage 0. Becuase we don't need other stages in this implenetation
void full_adder_iter(string &a, string &b, bool c0, bool* s, int N){	
	// r = -1
	s[N-1] = binaryAddition(binaryAddition(a[N-1] - '0', b[N-1] - '0'), c0); // adding the least significant bit
	for (int i=N-2; i>=0; i--) {	
		// other bits than the less significant
		int j = N-i-1; // This is the index of the j_th carry we're looking for to perform the addition
		bool ci = 0;  // This is the i_th carry we're looking for to perform the addition
		ci = ci || G[i+1]; // i+1 because i-1(from left to right) means less significant, so we have to advance in bits to the right (is is reversed)
		if (j == 1) {
			// we can calculate C1 using C0
			ci += P[N-1] && c0;
		} else {
			ci = ci || getFirstPatternVal(j, N);
			ci = ci || getSecondPatternVal(j, c0);
		}
		s[i] = binaryAddition(binaryAddition(a[i] - '0', b[i] - '0'), ci);
	}	
}


int main() {
	P = init_array(N);
	G = init_array(N);
	
	init_P_and_G_iter(a, b, N);
	cout << "Propagate:\n";
	display_array(P, N);
	cout << "Generate:\n";
	display_array(G ,N);

	cout << "------ADDITION------\n";
	cout << "a=" << a << ", b=" << b << endl;
	bool* s = (bool*) malloc(N * sizeof(bool));
	full_adder_iter(a, b, c0, s, N);
	
	for (int i=0; i<N; i++) {
		cout << s[i] << " | ";
	}
	cout << endl;

}
