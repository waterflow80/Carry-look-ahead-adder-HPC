/**
 * ----- Carry Look Ahead Adder -----
 * Important formulas:
 * - [ Gb = Gl + (Pl . Gr) ] => The 'Generate' of a block equals the 'Generate' of the left side OR ('Propagate' of the left side AND 'Generate' of the right side)
 * - [ Pb = Pl . Pr] => The 'Propagate' of a block equals the 'Propagate' of the left side AND the 'Propagate' of the right side
 * - P = x + y (x OR y). Can also work with x XOR y (Propagate of a block of 1-bit)
 * - G = x . y (x AND y) (Genreate of a block of 1-bit)*/ 

#include<bits/stdc++.h>

using namespace std;


const int N=4; // The number of bits of each number to add (this will be input by the user)

// Input Data
string a = "1010";
string b = "1100";
int c0 = 1; // initial carry in

// Utils
bool** P; // Propagate
bool** G; // Generate


bool** init_2D_array(int nRows, int nCols) {
	bool **a = (bool**) malloc(nRows * sizeof(bool*));
	for (int i=0; i<nRows; i++) {
		a[i] = (bool*) malloc(nCols * sizeof(bool));
		for (int j=0; j<nCols; j++) {
			//cout << "Init a[" << i << "][" << j << "]=" << a[i][j] << endl;  
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
	cout << "a=" << a << ", b=" << b << ", stage=" << stage << ", l="<< l << ", r=" << r << endl;
	if (stage == 0) {
		// In this case r=-1, We only have 1-bit block
		P[stage][l] = a[l] - '0' || b[l] - '0';
		cout << a[l] - '0' << endl;
		cout << b[l] - '0' << endl;
		G[stage][l] = a[l] - '0' && b[l] - '0';
		cout << "Propagate[0][" << l << "]=" << P[stage][l] << endl;
		cout << "Generate[0][" << l << "]=" << G[stage][l] << endl;
		cout << "Propagate:\n";
		display_2D_array(P, log2(N)+1 ,N);
		cout << "Generate:\n";
		display_2D_array(G, log2(N)+1 ,N);
		cout << "===========================\n";
		
	} else if (stage == 1) {
		// The next recursive call will process 1-bit blocks
		init_P_and_G(a, b, stage - 1, l, -1); // left side of the block
		init_P_and_G(a, b, stage - 1, r, -1); // right side of the block
		cout << "stage = 1, completed right and left side \n";
		P[stage][l] = P[stage-1][l] && P[stage-1][r];
		G[stage][l] = G[stage-1][l] || (P[stage-1][l] && P[stage-1][r]);
	} else {
		// stage > 1
		init_P_and_G(a, b, stage - 1, l, (l+r)/2); // left
		init_P_and_G(a, b, stage - 1, (l+r)/2+1, r); // right
		P[stage][l] = P[stage-1][l] && P[stage-1][(l+r)/2+1];
		G[stage][l] = G[stage-1][l] || (P[stage-1][l] && G[stage-1][(l+r)/2+1]);
	}
}



int main() {
	P = init_2D_array(log2(N)+1 ,N);
	G = init_2D_array(log2(N)+1, N);
	display_2D_array(P, log2(N)+1 ,N);
	display_2D_array(G, log2(N)+1 ,N);
	
	init_P_and_G(a, b, log2(N), 0, N-1);
	cout << "Propagate:\n";
	display_2D_array(P, log2(N)+1 ,N);
	cout << "Generate:\n";
	display_2D_array(G, log2(N)+1 ,N);
	


}
