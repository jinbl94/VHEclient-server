/*
  VHE test program
  */

#include "vhe.h"
#include <time.h>

const mat_ZZ ipkey(const mat_ZZ& secretkey1,const mat_ZZ& secretkey2){
	return transpose(vectorize(transpose(secretkey1)*secretkey2));
}

const mat_ZZ ipcipher(const mat_ZZ& c1,const mat_ZZ& c2){
	mat_ZZ cc;
	cc=vectorize(c1*transpose(c2));
	nearestinteger(cc,w);
	return cc;
}

int main() {
	VHE vhe1,vhe2;
	clock_t start;
	int time;
	int N=10;
	mat_ZZ x1,x2,dxx,c1,c2,cc,M1,M2,S1,S2,S,matrix;
	fprintf(stdout, "size\tns\ts\n",N,time,time/CLOCKS_PER_SEC);

	for(int i=0;i<4;i++,N+=10){
		x1.SetDims(N,1);
		x2.SetDims(N,1);
		vhe1.init(N);
		vhe2.init(N);
		M1=vhe1.getpublickey();
		M2=vhe2.getpublickey();
		S1=vhe1.getsecretkey();
		S2=vhe2.getsecretkey();
		for(int j=0;j<N;j++){
			x1[j][0]=RandomBnd(100);
			x2[j][0]=RandomBnd(100);
		}
		c1=vhe1.encrypt(x1);
		c2=vhe2.encrypt(x2);

		S=ipkey(S1,S2);
		ident(matrix,S.NumRows());
		vhe1.setsecretkey(S);
		vhe1.setltmatrix(matrix);
		start=clock();
		cc=vhe1.getnewpublickey()*ipcipher(c1,c2);
		dxx=vhe1.newdecrypt(cc);
		time=(double)(clock()-start);
		fprintf(stdout, "%d\t%d\t%d\n",N,time,time/CLOCKS_PER_SEC);

		//	ZZ xx;
		//	vec_ZZ xx1,xx2;
		//	xx1.SetLength(N);
		//	xx2.SetLength(N);
		//	for(int j=0;j<N;j++){
		//		xx1[j]=x1[j][0];
		//		xx2[j]=x2[j][0];
		//	}
		//	InnerProduct(xx,xx1,xx2);
		//	cout<<xx<<endl;
		//	cout<<dxx[0][0]<<endl;
	}
}
