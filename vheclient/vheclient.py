#! /usr/bin/python
## vhe client with GUI
from hevector import evaluate
from model import features
from message import send_msg
from message import recv_msg

import pickle
import socket
import sys
import os
import gi
import datetime
gi.require_version('Gtk','3.0')
from gi.repository import Gtk

## server ip and service port are needed
SERVER_ADDR="127.0.0.1"
SEARCH_PORT=6666
MAIL_PORT=7777

filenames=pickle.load(open('filenames','rb'))
bannedcharacters=set('!"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~0123456789')

def simplify(text):
	return ' '.join(w for w in ''.join(c for c in text.lower() if c not in bannedcharacters).split() if w in features).strip()

def getfeature(text):
	feature=[0]*len(features)
	feature[0]=1
	wordlist=simplify(text).split()
	if len(wordlist)==0:
		return
	for w in wordlist:
		feature[features[w]]+=1
	return tuple(feature,)

## send Key-Switching matrix to server
def serverresult(matrix):
	s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
	s.connect((SERVER_ADDR,SEARCH_PORT))
	datasend=pickle.dumps(matrix)
	print("sending data to server")
	send_msg(s,datasend)
	datareceive=recv_msg(s)
	print("received results")
	results=pickle.loads(datareceive)
	s.close()
	return results

def getsearchresults(text):
	featurevec=getfeature(text)
	if not featurevec:
		return
	with open('secretkey','rb') as f:
		secretkey=pickle.load(f)
		f.close()
		print("calculating new key pair")
		newsecretkey,newpublickey=evaluate([featurevec,secretkey,'getltkeypair'])
		newciphertext=serverresult(newpublickey)
		print("decrypt ciphertext results")
		resultvec,=evaluate([newciphertext,newsecretkey,'decrypt'])
		results=[]
		for i in range(len(resultvec)):
			results.append(resultvec[i][0])

		return tuple(i for i in sorted(enumerate(results),key=lambda j: -j[1]) if i[1]>1)

## get content of an email
def getmail(i):
	s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
	s.connect((SERVER_ADDR,MAIL_PORT))
	datasend=pickle.dumps(i)
	send_msg(s,datasend)
	datareceive=recv_msg(s)
	print("received mail")
	results=pickle.loads(datareceive)
	s.close()
	return results

class StackWindow(Gtk.Window):
	def __init__(self):
		Gtk.Window.__init__(self, title="Mail Search")
		self.set_border_width(10)
		
		grid=Gtk.Grid(orientation=Gtk.Orientation.VERTICAL)
		grid.props.column_spacing=10
		self.set_default_size(600,400)
		self.add(grid)
		
		stack=Gtk.Stack()
		stack.set_transition_type(Gtk.StackTransitionType.SLIDE_LEFT_RIGHT)
		stack.set_transition_duration(100)
		
		searchsw=Gtk.ScrolledWindow()
		searchsw.set_vexpand(True)
		searchsw.set_hexpand(True)
		searchtext=Gtk.TextView()
		searchtext.set_wrap_mode(Gtk.WrapMode.WORD)
		self.searchbuffer=searchtext.get_buffer()
		searchsw.add(searchtext)
		searchbutton=Gtk.Button("Search")
		searchbutton.connect("clicked",self.on_search)
		searchgrid=Gtk.Grid()
		searchgrid.attach(searchsw,0,0,3,1)
		searchgrid.attach(searchbutton,2,3,1,1)
		stack.add_titled(searchgrid,"search","Search")
		
		resultsw=Gtk.ScrolledWindow()
		resultsw.set_vexpand(True)
		resultsw.set_hexpand(True)
		resulttext=Gtk.TextView()
		resulttext.set_editable(False)
		self.resultbuffer=resulttext.get_buffer()
		resultsw.add(resulttext)
		resultlabel=Gtk.Label("No.\tFileNum\tValue\tFileName")
		resultlabel.set_alignment(0,0.5)
		resultgrid=Gtk.Grid()
		resultgrid.attach(resultsw,0,0,3,1)
		resultgrid.attach(resultlabel,0,1,3,1)
		stack.add_titled(resultgrid,"result","Results")
		
		logsw=Gtk.ScrolledWindow()
		logsw.set_vexpand(True)
		logsw.set_hexpand(True)
		logtext=Gtk.TextView()
		logtext.set_wrap_mode(Gtk.WrapMode.WORD)
		logtext.set_editable(False)
		self.logbuffer=logtext.get_buffer()
		logsw.add(logtext)
		stack.add_titled(logsw,"log","Log")

		mailsw=Gtk.ScrolledWindow()
		mailsw.set_vexpand(True)
		mailsw.set_hexpand(True)
		mailtext=Gtk.TextView()
		mailtext.set_wrap_mode(Gtk.WrapMode.WORD)
		mailtext.set_editable(False)
		self.mailbuffer=mailtext.get_buffer()
		mailsw.add(mailtext)
		self.numentry=Gtk.Entry()
		mailbutton=Gtk.Button("View")
		mailbutton.connect("clicked",self.on_view)
		mailgrid=Gtk.Grid()
		mailgrid.attach(mailsw,0,0,3,1)
		mailgrid.attach(self.numentry,0,1,1,1)
		mailgrid.attach(mailbutton,1,1,1,1)
		stack.add_titled(mailgrid,"mail","Mail")
		
		stack_switcher=Gtk.StackSwitcher(orientation=Gtk.Orientation.VERTICAL)
		stack_switcher.set_stack(stack)

		grid.attach(stack_switcher,0,0,1,3)
		grid.attach(stack,1,0,3,3)
	
	def on_search(self,widget):
		self.write_log("searching\n")
		self.resultbuffer.delete(self.resultbuffer.get_start_iter(),self.resultbuffer.get_end_iter())
		text=self.searchbuffer.get_text(self.searchbuffer.get_start_iter(),self.searchbuffer.get_end_iter(),True)
		if not text:
			self.write_log("Error: please input a string\n")
			return
		results=getsearchresults(text)
		if not results:
			self.write_log("Error: please change your string\n")
			return
		self.write_log("search compeleted\n")
		self.write_results(results)
		print("done")

	def on_view(self,widget):
		self.mailbuffer.delete(self.mailbuffer.get_start_iter(),self.mailbuffer.get_end_iter())
		mailnum=int(self.numentry.get_text())
		mailcontent=getmail(mailnum)
		if not mailcontent:
			self.write_log("Error: failed to view email "+str(mailnum)+"\n")
			return
		self.mailbuffer.insert(self.mailbuffer.get_end_iter(),mailcontent)
		self.write_log("View: "+str(mailnum)+"\n")
	
	def write_log(self,text):
		logend=self.logbuffer.get_end_iter()
		self.logbuffer.insert(logend,datetime.datetime.now().strftime("%H:%M:%S %d/%m/%Y\t")+text)

	def write_results(self,results):
		n=0
		for i,j in results:
			n+=1
			resultend=self.resultbuffer.get_end_iter()
			self.resultbuffer.insert(resultend,"{0:<3d}\t{1:>3d}\t\t{2:<5d}\t".format(n,i,j)+filenames[i]+"\n")

def vheclient():
	window=StackWindow()
	window.connect('delete-event',Gtk.main_quit)
	window.show_all()
	Gtk.main()

vheclient()
