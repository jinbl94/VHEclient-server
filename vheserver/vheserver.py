#! /usr/bin/python
## vhe server without GUI

from hevector import evaluate
from message import send_msg
from message import recv_msg

import pickle
import socket
import threading
import time

HOST=''
SEARCH_PORT=6666
MAIL_PORT=7777

filefeatures=pickle.load(open('filefeatures','rb'))
filenames=pickle.load(open('filenames','rb'))

class Mailsubserver(threading.Thread):
	def __init__(self,clientsocket):
		threading.Thread.__init__(self)
		self.c=clientsocket

	def run(self):
		self.datareceive=recv_msg(self.c)
		self.filenum=pickle.loads(self.datareceive)
		self.mailcontent=open(filenames[self.filenum],'rb').read().decode('utf-8','ignore')
		self.datasend=pickle.dumps(self.mailcontent)
		print("sending mail to client")
		send_msg(self.c,self.datasend)
		self.c.close()

class Mailserver(threading.Thread):
	def __init__(self):
		threading.Thread.__init__(self)
		self.thread_list=[]

	def run(self):
		with socket.socket(socket.AF_INET,socket.SOCK_STREAM) as self.s:
			self.s.bind((HOST,MAIL_PORT))
			self.s.listen(5)

			while True:
				(self.c,self.addr)=self.s.accept()
				self.mailsubserver=Mailsubserver(self.c)
				self.thread_list.append(self.mailsubserver)
				self.mailsubserver.start()
				for thread in self.thread_list:
					if not thread.isAlive():
						self.thread_list.remove(thread)
						thread.join()
			for thread in self.thread_list:
				thread.join(1.0)
			self.s.close()

class Searchsubserver(threading.Thread):
	def __init__(self,clientsocket):
		threading.Thread.__init__(self)
		self.c=clientsocket

	def run(self):
		self.datareceive=recv_msg(self.c)
		print("get publickey from client")
		self.publickey=pickle.loads(self.datareceive)
		print("calculating")
		self.results,=evaluate([filefeatures,self.publickey,'encrypt_m'])
		self.datasend=pickle.dumps(self.results)
		print("sending result to client")
		send_msg(self.c,self.datasend)
		self.c.close()

class Searchserver(threading.Thread):
	def __init__(self):
		threading.Thread.__init__(self)
		self.thread_list=[]

	def run(self):
		with socket.socket(socket.AF_INET,socket.SOCK_STREAM) as self.s:
			self.s.bind((HOST,SEARCH_PORT))
			self.s.listen(5)

			while True:
				(self.c,self.addr)=self.s.accept()
				self.searchsubserver=Searchsubserver(self.c)
				self.thread_list.append(self.searchsubserver)
				self.searchsubserver.start()
				for thread in self.thread_list:
					if not thread.isAlive():
						self.thread_list.remove(thread)
						thread.join()
			for thread in self.thread_list:
				thread.join(1.0)
			self.s.close()

def vheserver():
	thread_list=[]
	searchserver=Searchserver()
	searchserver.daemon=True
	searchserver.start()
	thread_list.append(searchserver)
	mailserver=Mailserver()
	mailserver.daemon=True
	mailserver.start()
	thread_list.append(mailserver)
	while True:
		time.sleep(10)

vheserver()
