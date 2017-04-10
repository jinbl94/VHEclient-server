/*
   VHE head file
   function and class declaration
 */
#ifndef VHEHEAD
#define VHEHEAD
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

#include <NTL/mat_ZZ.h>
#include <NTL/ZZ.h>
#include <NTL/vec_ZZ.h>

#include <cmath>
#include <stack>

using namespace std;
using namespace NTL;

/*
  varible declaration
  */
const ZZ w(1ll<<40ll),aBound(1000),tBound(aBound),eBound(1000);

const mat_ZZ hcat(const mat_ZZ& matrix1,const mat_ZZ& matrix2);
const mat_ZZ vcat(const mat_ZZ& matrix1,const mat_ZZ& matrix2);
const mat_ZZ getrandommatrix(long row,long col,const ZZ& bound);
const mat_ZZ vectorize(const mat_ZZ& matrix);
const void nearestinteger(mat_ZZ& matrix,const ZZ& w);

class VHE{
	private:
		//secret key and p_s, p_m
		mat_ZZ secretkey,publickey,newsecretkey,newpublickey;
	public:
		VHE();
		~VHE();

		//generate ps and pm with the size of plain text
		void init(int size_n);

		//w*I as secret key, x as ciphertext
		mat_ZZ encrypt(const mat_ZZ& plaintext);
		mat_ZZ newencrypt(const mat_ZZ& plaintext);
		mat_ZZ decrypt(const mat_ZZ& ciphertext);
		mat_ZZ newdecrypt(const mat_ZZ& ciphertext);

		//linear transform
		void setltmatrix(const mat_ZZ& matrix);

		//get/set secret key pair
		mat_ZZ getsecretkey();
		void setsecretkey(const mat_ZZ& matrix);
		mat_ZZ getpublickey();
		void setpublickey(const mat_ZZ& matrix);

		//get linear transform key pair
		mat_ZZ getnewsecretkey();
		mat_ZZ getnewpublickey();
};
#endif
