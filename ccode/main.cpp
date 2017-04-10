/*
  VHE test program
  */

#include "main.h"

int main() {
	int code=-1;
	ifstream cin("vheinput");

	cin.tie(NULL);
	ios_base::sync_with_stdio(false);

	stack<mat_ZZ*> matrices;

	mat_ZZ publickey,secretkey,plaintext,ciphertext,gmatrix;
	mat_ZZ* matrix;

	string operation;
	while(cin>>operation){
		switch(parseop(operation)){
			case 0:
				//matrix: input matrix
				matrix=new mat_ZZ();
				cin>>*matrix;
				matrices.push(matrix);
				code=0;
				break;
			case 1:
				//encrypt: generate keypair and encrypt plaintext
				plaintext=*matrices.top();
				matrices.pop();
				encrypt(plaintext);
				code+=1;
				break;
			case 2:
				//encrypt_m: encrypt a plaintext with given publickey
				publickey=*matrices.top();
				matrices.pop();
				plaintext=*matrices.top();
				matrices.pop();
				encrypt_m(plaintext,publickey);
				code+=2;
				break;
			case 3:
				//decrypt: decrypt a ciphertext with given secretkey
				secretkey=*matrices.top();
				matrices.pop();
				ciphertext=*matrices.top();
				matrices.pop();
				decrypt(ciphertext,secretkey);
				code+=3;
				break;
			case 4:
				//getltkeypair: return linear tranform key pair by given parameters
				secretkey=*matrices.top();
				matrices.pop();
				gmatrix=*matrices.top();
				matrices.pop();
				getltkeypair(gmatrix,secretkey);
				code+=4;
				break;
			default:
				//other operations
				cerr<<"Operation "<<operation<<" not defined"<<endl;
				code=-1;
		}
	}
//	int N=10;
//	mat_ZZ x,c,cc,g,ss,mm;
//	x.SetDims(N,N);
//	g.SetDims(1,N);
//	for(int i=0;i<N;i++){
//		for(int j=0;j<N;j++){
//			x[i][j]=RandomBnd(2);
//		}
//		g[0][i]=RandomBnd(2);
//	}
//	vhe.init(transpose(x).NumRows());
//	cout<<"x: "<<x<<endl;
//	cout<<"gx: "<<g*transpose(x)<<endl;
//	c=transpose(vhe.encrypt(transpose(x)));
//	vhe.setltmatrix(g);
//	ss=vhe.getnewsecretkey();
//	mm=vhe.getnewpublickey();
//	cc=encrypt_m(c,mm);
//	decrypt(cc,ss);
}
