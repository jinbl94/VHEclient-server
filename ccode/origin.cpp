#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
//#include <NTL/mat_ZZ_p.h>
#include <NTL/mat_ZZ.h>
//#include <NTL/ZZ_p.h>
#include <NTL/ZZ.h>
//#include <NTL/vec_ZZ_p.h>
#include <NTL/vec_ZZ.h>
#include <cmath>
#include <stack>
#include <time.h>

using namespace std;
using namespace NTL;

const ZZ w(1ll << 40ll);
ZZ aBound(1000), tBound(aBound), eBound(1000);
int l = 100;

const mat_ZZ hCat(const mat_ZZ& A, const mat_ZZ& B);
const mat_ZZ vCat(const mat_ZZ& A, const mat_ZZ& B);

const vec_ZZ decrypt(const mat_ZZ& S, const vec_ZZ& c);

// returns c*
const vec_ZZ getBitVector(const vec_ZZ& c);

// returns S*
const mat_ZZ getBitMatrix(const mat_ZZ& S);

// returns S
const mat_ZZ getSecretKey(const mat_ZZ& T);

// returns M
const mat_ZZ keySwitchMatrix(const mat_ZZ& S, const mat_ZZ& T);

// finds c* then returns Mc*
const vec_ZZ keySwitch(const mat_ZZ& M, const vec_ZZ& c);

// as described, treating I as the secret key and wx as ciphertext
const vec_ZZ encrypt(const mat_ZZ& T, const vec_ZZ& x);

const mat_ZZ getRandomMatrix(long row, long col, const ZZ& bound);

// server side addition with same secret key
const vec_ZZ addn(const vec_ZZ& c1, const vec_ZZ& c2);

// server side linear transformation,
// returns S(Gx) given c=Sx and M (key switch matrix from GS to S)
const vec_ZZ linearTransform(const mat_ZZ& M, const vec_ZZ& c);

// returns M, the key switch matrix from GS to S,
// to be sent to server
const mat_ZZ linearTransformClient(const mat_ZZ& T, const mat_ZZ& G);

// computes an inner product, given two ciphertexts and the keyswitch matrix
const vec_ZZ innerProd(const vec_ZZ& c1, const vec_ZZ& c2, const mat_ZZ& M);

// returns M, the key switch matrix from vec(S^t S) to S,
// to be sent to the server
const mat_ZZ innerProdClient(const mat_ZZ& T);

// returns a column vector
const mat_ZZ vectorize(const mat_ZZ& M);

const mat_ZZ copyRows(const mat_ZZ& row, long numrows);




// finds c* then returns Mc*
const vec_ZZ keySwitch(const mat_ZZ& M, const vec_ZZ& c) {
	vec_ZZ cstar = getBitVector(c);
	return M * cstar;
}


const mat_ZZ getRandomMatrix(long row, long col, const ZZ& bound){
	mat_ZZ A;
	A.SetDims(row, col);
	for (int i=0; i<row; ++i){
		for (int j=0; j<col; ++j){
			A[i][j] = RandomBnd(bound);
		}
	}
	return A;
}




// returns S*
const mat_ZZ getBitMatrix(const mat_ZZ& S) {
	mat_ZZ result;
	int rows = S.NumRows(), cols = S.NumCols();
	result.SetDims(rows, l * cols);

	vec_ZZ powers;
	powers.SetLength(l);
	powers[0] = 1;
	for(int i = 0; i < l - 1; ++i) {
		powers[i+1] = powers[i]*2;
	}

	for(int i = 0; i < rows; ++i) {
		for(int j = 0; j < cols; ++j) {
			for(int k = 0; k < l; ++k) {
				result[i][j*l + k] = S[i][j] * powers[k];
			}
		}
	}

	return result;
}


// returns c*
const vec_ZZ getBitVector(const vec_ZZ& c) {
	vec_ZZ result;
	int length = c.length();
	result.SetLength(length * l);
	for(int i = 0; i < length; ++i) {
		ZZ sign = (c[i] < ZZ(0)) ? ZZ(-1) : ZZ(1);
		ZZ value = c[i] * sign;
		for(int j = 0; j < l; ++j) {
			result[i * l + j] = sign*bit(value, j);
		}
	}
	return result;
}



// returns S
const mat_ZZ getSecretKey(const mat_ZZ& T) {
	mat_ZZ I;
	ident(I, T.NumRows());
	return hCat(I, T);
}


const mat_ZZ hCat(const mat_ZZ& A, const mat_ZZ& B) {
	assert(A.NumRows() == B.NumRows());

	int rows = A.NumRows(), colsA = A.NumCols(), colsB = B.NumCols();
	mat_ZZ result;
	result.SetDims(rows, colsA + colsB);

	// Copy A
	for(int i = 0; i < rows; ++i) {
		for(int j = 0; j < colsA; ++j) {
			result[i][j] = A[i][j];
		}
	}

	// Copy B
	for(int i = 0; i < rows; ++i) {
		for(int j = 0; j < colsB; ++j) {
			result[i][colsA + j] = B[i][j];
		}
	}

	return result;
}

const mat_ZZ vCat(const mat_ZZ& A, const mat_ZZ& B) {
	assert(A.NumCols() == B.NumCols());

	int cols = A.NumCols(), rowsA = A.NumRows(), rowsB = B.NumRows();
	mat_ZZ result;
	result.SetDims(rowsA + rowsB, cols);

	// Copy A
	for(int i = 0; i < rowsA; ++i) {
		for(int j = 0; j < cols; ++j) {
			result[i][j] = A[i][j];
		}
	}

	// Copy B
	for(int i = 0; i < rowsB; ++i) {
		for(int j = 0; j < cols; ++j) {
			result[i + rowsA][j] = B[i][j];
		}
	}

	return result;
}

inline const ZZ nearestInteger(const ZZ& x, const ZZ& w) {
	return (x + (w+1)/2) / w;
}

const vec_ZZ decrypt(const mat_ZZ& S, const vec_ZZ& c) {
	vec_ZZ Sc = S*c;
	vec_ZZ output;
	output.SetLength(Sc.length());
	for (int i=0; i<Sc.length(); i++) {
		output[i] = nearestInteger(Sc[i], w);
	}
	return output;
}

const mat_ZZ keySwitchMatrix(const mat_ZZ& S, const mat_ZZ& T) {
	mat_ZZ Sstar = getBitMatrix(S);
	mat_ZZ A = getRandomMatrix(T.NumCols(),Sstar.NumCols(),aBound);
	mat_ZZ E = getRandomMatrix(Sstar.NumRows(),Sstar.NumCols(),eBound);
	return vCat(Sstar + E - T*A, A);
}

const vec_ZZ encrypt(const mat_ZZ& T, const vec_ZZ& x) {
	mat_ZZ I;
	ident(I, x.length());
	mat_ZZ M=keySwitchMatrix(I,T);
	vec_ZZ result=keySwitch(M, w * x);
	return result;
}




const vec_ZZ addVectors(const vec_ZZ& c1, const vec_ZZ& c2){
	return c1 + c2;
}

const vec_ZZ linearTransform(const mat_ZZ& M, const vec_ZZ& c){
	return M * getBitVector(c);
}

const mat_ZZ linearTransformClient(const mat_ZZ& G, const mat_ZZ& S, const mat_ZZ& T){
	return keySwitchMatrix(G * S, T);
}


const vec_ZZ innerProd(const vec_ZZ& c1, const vec_ZZ& c2, const mat_ZZ& M){
	mat_ZZ cc1;
	mat_ZZ cc2;
	mat_ZZ cc;

	cc1.SetDims(c1.length(), 1);
	for (int i=0; i<c1.length(); ++i){
		cc1[i][0] = c1[i];
	}
	cc2.SetDims(1, c2.length());
	for (int i=0; i<c2.length(); ++i){
		cc2[0][i] = c2[i];
	}
	cc = vectorize(cc1 * cc2);

	vec_ZZ output;
	output.SetLength(cc.NumRows());
	for (int i=0; i<cc.NumRows(); i++) {
		output[i] = nearestInteger(cc[i][0], w);
	}
	return M * getBitVector(output);
}

const mat_ZZ innerProdClient(const mat_ZZ& T){
	mat_ZZ S = getSecretKey(T);
	mat_ZZ tvsts = transpose(vectorize(transpose(S) * S));
	mat_ZZ mvsts = copyRows(tvsts, T.NumRows());
	return keySwitchMatrix(mvsts, T);
}




const mat_ZZ copyRows(const mat_ZZ& row, long numrows){
	mat_ZZ ans;
	ans.SetDims(numrows, row.NumCols());
	for (int i=0; i<ans.NumRows(); ++i){
		for (int j=0; j<ans.NumCols(); ++j){
			ans[i][j] = row[0][j];
		}
	}
	return ans;
}

const mat_ZZ vectorize(const mat_ZZ& M){
	mat_ZZ ans;
	ans.SetDims(M.NumRows() * M.NumCols(), 1);
	for (int i=0; i<M.NumRows(); ++i){
		for (int j=0; j<M.NumCols(); ++j){
			ans[i*M.NumCols() + j][0] = M[i][j];
		}
	}
	return ans;
}

int main() {
	// Testing for the 3 fundamental operations:
	clock_t start;
	int time;
	int N = 10;
	vec_ZZ x1;
	vec_ZZ x2;
	fprintf(stdout, "size\tns\ts\n",N,time,time/CLOCKS_PER_SEC);
	for(int i=0;i<4;i++,N+=10){
		x1.SetLength(N);
		x2.SetLength(N);
		for(int j = 0; j < N; j++) {
			x1[j] = RandomBnd(100);
			x2[j] = RandomBnd(100);
		}
		mat_ZZ T = getRandomMatrix(N, N, tBound);
		mat_ZZ S = getSecretKey(T);
		vec_ZZ c1 = encrypt(T, x1);
		vec_ZZ c2 = encrypt(T, x2);

		mat_ZZ M;
		vec_ZZ cc;
		vec_ZZ dxx;

		M = innerProdClient(T);
		start=clock();
		cc = innerProd(c1, c2, M);
		dxx = decrypt(getSecretKey(T), cc);
		time=(double)(clock() - start);
		fprintf(stdout, "%d\t%d\t%d\n",N,time,time/CLOCKS_PER_SEC);

		//	ZZ xx;
		//	InnerProduct(xx, x1, x2);
		//	cout << xx << endl;
		//	cout << dxx[0] << endl;
		//	cout << xx - dxx[0] << endl;
	}
}
