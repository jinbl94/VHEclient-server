/*
  VHE class implementation, without homomorphic calculations.
  */

#include "vhe.h"

/*
  global functions
  */
const mat_ZZ getrandommatrix(long row,long col,const ZZ& bound){
	mat_ZZ matrix;
	matrix.SetDims(row,col);
	for(int i=0;i<row;++i){
		for(int j=0;j<col;++j){
			matrix[i][j]=RandomBnd(bound);
		}
	}
	return matrix;
}

const mat_ZZ hcat(const mat_ZZ& matrix1,const mat_ZZ& matrix2){
	assert(matrix1.NumRows()==matrix2.NumRows());

	int i=0,j=0;
	mat_ZZ matrix1trans=transpose(matrix1),matrix2trans=transpose(matrix2);
	mat_ZZ result;
	result.SetDims(matrix1trans.NumRows()+matrix2trans.NumRows(),matrix1trans.NumCols());

	for(i=0;i<matrix1trans.NumRows();i++){
		result[i]=matrix1trans[i];
	}
	for(j=0;j<matrix2trans.NumRows();j++){
		result[i]=matrix2trans[j];
	}

	return transpose(result);
}

const mat_ZZ vcat(const mat_ZZ& matrix1,const mat_ZZ& matrix2){
	assert(matrix1.NumCols()==matrix2.NumCols());

	int i=0,j=0;
	mat_ZZ result;
	result.SetDims(matrix1.NumRows()+matrix2.NumRows(),matrix1.NumCols());

	for(i=0;i<matrix1.NumRows();i++){
		result[i]=matrix1[i];
	}
	for(j=0;j<matrix2.NumRows();j++){
		result[i]=matrix2[j];
	}

	return result;
}

const void getinvertiblematrix(int dimension,mat_ZZ& l,mat_ZZ& r){
	int op[3];
	vec_ZZ templ,tempr;
	ident(l,dimension);
	ident(r,dimension);
	templ.SetLength(dimension);
	tempr.SetLength(dimension);
	srand((int)time(NULL));
	for(int i=0;i<dimension*5;i++){
		op[0]=rand()%dimension;
		do{
			op[1]=rand()%dimension;
		}while(op[0]==op[1]);
		op[2]=rand()%3-1;
		tempr=r[op[0]];
		for(int j=0;j<dimension;j++){
			templ[j]=l[j][op[1]];
		}
		if(op[2]==0){
			r[op[0]]=r[op[1]];
			r[op[1]]=tempr;
			for(int j=0;j<dimension;j++){
				l[j][op[1]]=l[j][op[0]];
				l[j][op[0]]=templ[j];
			}
		}else{
			r[op[1]]+=tempr*op[2];
			for(int j=0;j<dimension;j++){
				l[j][op[0]]+=templ[j]*(-op[2]);
			}
		}
	}
}

const mat_ZZ vectorize(const mat_ZZ& matrix){
	mat_ZZ result;
	result.SetDims(matrix.NumRows()*matrix.NumCols(),1);
	for(int i=0;i<matrix.NumRows();i++){
		for(int j=0;j<matrix.NumCols();j++){
			result[matrix.NumCols()*i+j][0]=matrix[i][j];
		}
	}
	return result;
}

const void nearestinteger(mat_ZZ& matrix,const ZZ& w){
	for(int i=0;i<matrix.NumRows();i++){
		for(int j=0;j<matrix.NumCols();j++){
			matrix[i][j]=(matrix[i][j]+(w+1)/2)/w;
		}
	}
	return;
}

/*
   VHE functions
   */
VHE::VHE(){}
VHE::~VHE(){}

void VHE::init(int size_n){
	mat_ZZ lmatrix,rmatrix;
	getinvertiblematrix(size_n+1,lmatrix,rmatrix);
	mat_ZZ temp,I,T,A;
	ident(I,size_n);
	T=getrandommatrix(size_n,1,tBound);
	A=getrandommatrix(1,size_n,aBound);
	secretkey=hcat(I,T)*lmatrix;
	publickey=rmatrix*vcat(w*I-T*A,A);
}

mat_ZZ VHE::encrypt(const mat_ZZ& plaintext){
	assert(publickey.NumCols()==plaintext.NumRows());
	mat_ZZ e;
	e.SetDims(publickey.NumRows(),plaintext.NumCols());
	for(int i=0;i<e.NumRows();i++){
		for(int j=0;j<e.NumCols();j++){
			e[i][j]=RandomBnd(eBound);
		}
	}
	return publickey*plaintext+e;
}

mat_ZZ VHE::newencrypt(const mat_ZZ& plaintext){
	assert(newpublickey.NumCols()==plaintext.NumRows());
	mat_ZZ e;
	e.SetDims(newpublickey.NumRows(),plaintext.NumCols());
	for(int i=0;i<e.NumRows();i++){
		for(int j=0;j<e.NumCols();j++){
			e[i][j]=RandomBnd(eBound);
		}
	}
	return newpublickey*plaintext+e;
}

mat_ZZ VHE::decrypt(const mat_ZZ& ciphertext){
	assert(secretkey.NumCols()==ciphertext.NumRows());
	mat_ZZ result=secretkey*ciphertext;
	nearestinteger(result,w);
	return result;
}

mat_ZZ VHE::newdecrypt(const mat_ZZ& ciphertext){
	assert(newsecretkey.NumCols()==ciphertext.NumRows());
	mat_ZZ result=newsecretkey*ciphertext;
	nearestinteger(result,w);
	return result;
}

void VHE::setltmatrix(const mat_ZZ& matrix){
	mat_ZZ lmatrix,rmatrix;
	getinvertiblematrix(matrix.NumRows()+1,lmatrix,rmatrix);
	mat_ZZ I,T,A;
	ident(I,matrix.NumRows());
	T=getrandommatrix(matrix.NumRows(),1,tBound);
	A=getrandommatrix(1,secretkey.NumCols(),aBound);
	newsecretkey=hcat(I,T)*lmatrix;
	newpublickey=rmatrix*vcat(matrix*secretkey-T*A,A);
}

mat_ZZ VHE::getsecretkey(){return secretkey;}
mat_ZZ VHE::getpublickey(){return publickey;}
void VHE::setsecretkey(const mat_ZZ& matrix){secretkey=matrix;}
void VHE::setpublickey(const mat_ZZ& matrix){publickey=matrix;}
mat_ZZ VHE::getnewsecretkey(){return newsecretkey;}
mat_ZZ VHE::getnewpublickey(){return newpublickey;}
