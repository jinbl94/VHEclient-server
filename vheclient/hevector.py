#! /usr/bin/python
from subprocess import Popen,PIPE
from sys import stderr
def tupleToMat(t):
	if type(t) is int:
		return str(t).strip('L')
	if t and type(t[0]) is tuple:
		return 'matrix [%s]'%' '.join('[%s]'%' '.join(map(str,i)) for i in t)
	if t and type(t)is tuple:
		return 'matrix [%s]'%('[%s]'%' '.join(map(str,t)))
	else:
		stderr.write(error + 'Unrecognized input type\n')
		stderr.flush()

def vecToTuple(vec):
	return tuple(map(int,vec[1:-1].split()))

def send(ops):
	return '\n'.join(op if type(op) is str else tupleToMat(op) for op in ops)

def recv(output):
	out=list(map(str.strip,output.splitlines()))[::-1]
	result=[]
	while out:
		vec=out.pop()
		if vec[:2]=='[[':
			vec=vec[1:]
			matrix=[]
			while vec!=']':
				matrix.append(tuple(map(int,vec[1:-1].split())))
				vec=out.pop()
			result.append(tuple(matrix))
		else:
			stderr.write(error+'Unrecognized output\n')
			stderr.flush()
	return tuple(result)

def evaluate(operations,DEBUG=False):
	if DEBUG:
		print(operations)
	inp = send(operations)
	if DEBUG:
		print(inp)
	with open('vheinput','w') as f:
		f.write(inp)
		f.close()
	output,error=Popen(['./vhe'],stdin=PIPE,stdout=PIPE,shell=True,universal_newlines=True).communicate('')
	if DEBUG:
		print(output)
	if error:
		stderr.write(error+'\n')
		stderr.flush()
	return recv(output)
