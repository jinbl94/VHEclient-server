#! /usr/bin/python
## encrypt files listed in filenames.
## filenames contains 200 file names.
## it may take more than an hour.
## that's normal, don't panic

from hevector import evaluate
from model import features

import pickle

filenames=pickle.load(open('filenames','rb'))
featuremat=[]
bannedcharacters=set('!"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~0123456789')

def simplify(text):
	return ' '.join(w for w in ''.join(c for c in text.lower() if c not in bannedcharacters).split() if w in features).strip()

def getfeature(text):
	feature=[0]*len(features)
	feature[0]=1
	wordlist=simplify(text).split()
	for w in wordlist:
		feature[features[w]]+=1
	for i in feature:
		if i!=0:
			print(i,end=' ')
	print('')
	return tuple(feature,)

for i in range(10):
	doc=open(filenames[i],'rb').read().decode('utf-8','ignore')
	if doc!="":
		featuremat.append(getfeature(doc))

secretkey,publickey,ciphertext=evaluate([featuremat,"encrypt"])

with open('secretkey','wb') as f:
	pickle.dump(secretkey,f,pickle.HIGHEST_PROTOCOL)
	f.close()
with open('publickey','wb') as f:
	pickle.dump(publickey,f,pickle.HIGHEST_PROTOCOL)
	f.close()
with open('filefeatures','wb') as f:
	pickle.dump(ciphertext,f,pickle.HIGHEST_PROTOCOL)
	f.close()
