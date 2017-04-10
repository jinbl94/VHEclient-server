/*
   top level vhe functions
 */
#ifndef VHEMAINHEAD
#define VHEMAINHEAD
#include "vhe.h"
#include <string>
#define OPNUM 4

const char* operations[]={"matrix","encrypt","encrypt_m","decrypt","getltkeypair"};

VHE vhe;

int parseop(string op){
	int i=0;
	for(i=0;i<OPNUM;i++){
		if(op.compare(operations[i])==0)
			break;
	}
	return i;
}

//encryp plaintext with given publickey
const void encrypt_m(const mat_ZZ& plaintext, const mat_ZZ& publickey){
	vhe.setpublickey(publickey);
	cout<<transpose(vhe.encrypt(transpose(plaintext)))<<endl;
}

//generate keypair and encrypt plaintext
const void encrypt(const mat_ZZ& plaintext){
	vhe.init(transpose(plaintext).NumRows());
	cout<<vhe.getsecretkey()<<endl<<vhe.getpublickey()<<endl<<transpose(vhe.encrypt(transpose(plaintext)))<<endl;
}

//decrypt ciphertext with given secretkey
const void decrypt(const mat_ZZ& ciphertext,const mat_ZZ& secretkey){
	vhe.setsecretkey(secretkey);
	cout<<transpose(vhe.decrypt(transpose(ciphertext)))<<endl;
}

//return linear transform key pair which is costructed with G and old secretkey
const void getltkeypair(const mat_ZZ& matrix,const mat_ZZ& secretkey){
	vhe.setsecretkey(secretkey);
	vhe.setltmatrix(matrix);
	cout<<vhe.getnewsecretkey()<<endl;
	cout<<vhe.getnewpublickey()<<endl;
}

//return inner product key pair which is constructed with H and two secretkeys
const void getipkeypair(const mat_ZZ& secretkey1,const mat_ZZ& secretkey2,const mat_ZZ& hmatrix){
	mat_ZZ matrix;
	mat_ZZ secretkey=transpose(vectorize(transpose(secretkey1)*hmatrix*secretkey2));
	ident(matrix,secretkey.NumRows());
	vhe.setsecretkey(secretkey);
	vhe.setltmatrix(matrix);
	cout<<vhe.getnewsecretkey()<<endl;
	cout<<vhe.getnewpublickey()<<endl;
}

//return inner product result in ciphertext
const void getipciphertext(const mat_ZZ& ciphertext1,const mat_ZZ& ciphertext2,const mat_ZZ& publickey){
	mat_ZZ result;
	vhe.setpublickey(publickey);
	result=vectorize(ciphertext1*transpose(ciphertext2));
	nearestinteger(result,w);
	cout<<transpose(vhe.encrypt(result))<<endl;
}
#endif
