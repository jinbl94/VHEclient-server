#! /usr/bin/python
## encrypt files listed in filenames.
## filenames contains 200 file names.
## it may take more than an hour.
## that's normal, don't panic

from hevector import evaluate
import pickle

features=pickle.load(open('features','rb'))
filenames=pickle.load(open('filenames','rb'))
featuremat=[]
bannedcharacters=set('!"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~0123456789')

def simplify(text):
	return ' '.join(w for w in ''.join(c for c in text.lower() if c not in bannedcharacters).split() if w in features).strip()

def getfeature(text):
	feature=[0]*len(features)
	wordlist=simplify(text).split()
	for w in wordlist:
		feature[features[w]]+=1
	return tuple(feature,)

for i in range(len(filenames)):
	doc=open(filenames[i],'rb').read().decode('utf-8','ignore')
	if doc!="":
		featuremat.append(getfeature(doc))

print("features extracted")

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
