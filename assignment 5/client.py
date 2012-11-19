import sys,signal,socket
import Tkinter as tk
from threading import Thread,Lock
class Client():
	def __init__(self,addr,port):
		self.peer_list = []
		self.server_ip = addr
		self.server_port = port
		try:
			self.sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
			self.sock.connect((addr,port))
			print "[INFO]Connected with %s:%s"%(addr,str(port))			
			self.server_sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
			self.server_sock.bind(('',0))
			
		except socket.error as msg:
			if self.sock:
				self.sock.close()
			if self.server_sock:
				self.server_sock.close()		
			print "[ERR] Cannot bind address / Connect to server"
			sys.exit(1)
		self.server_sock.listen(5)
		print "[INFO]Listening at port %s"%(self.server_sock.getsockname()[1],)
		Thread(target=self.incoming_accept).start()
		
		#~ self.refresh_peerlist(self.sock)
		
	def incoming_accept(self):
		
		while(1):
			try:
				conn, addr = self.server_sock.accept()				
				Thread(target=self.incoming_individual_chatboxes,args=(conn,addr)).start()
			except socket.error:
				print "LOL"
				return
				
	def incoming_individual_chatboxes(self,conn,addr):
		c = ChatWindow(conn,addr)
		c.run()
				
	def refresh_peerlist(self,conn):
		print "[INFO]Fetching peer list..."
		
		conn.send('SEND_PEER_LIST')
		
		peer_string =""
		length = 0
		is_end = False
		while not is_end:
			data = conn.recv(1024)
			if not data:
				break;
			peer_string += data
			if ':' in data:
				number,peer_string = peer_string.split(';')
				length = int(number)
				peer_string = peer_string.lstrip('\n')
				
				if len(peer_string) >= length:
					break;
				data = conn.recv(length - len(peer_string))
				if not data:
					print "Server Disconnected"
					sys.exit(1)
				peer_string += data
				is_end = True
				break
		
		host_list = peer_string.split('\n')
		
		self.peer_list = []
		for h in host_list:
			if ':' in h:
				ip,port = h.rstrip().lstrip().split(':')
				self.peer_list += [(ip,int(port))]
		
		i=1
		horiz_line = "-".join("" for i in xrange(22))
		print horiz_line
		for addr in self.peer_list:
			print "%d -> %s:%d"%(i,addr[0],addr[1])
			i += 1
		print horiz_line
			
	def command_line(self):
		
		s_port = self.server_sock.getsockname()[1]
		self.sock.send('PORT:%s;'%(s_port,))
		while(True):
			command = raw_input('>')
			
			command = command.rstrip().lstrip()
			
			if command == 'LIST':
				self.refresh_peerlist(self.sock)
			elif command.startswith('CONNECT'):
				self.connect_to_peer(command)
			elif command == 'QUIT':
				self.close_all_sockets()
				break;
		return
				
	def close_all_sockets(self):
		self.sock.send('CLOSE')
		self.sock.close()
		self.server_sock.close()
	
	def connect_to_peer(self,command):
		num = -1
		for x in command.strip(' ')[1:]:
			if x != ' ':
				try:
					num = int(x)
				except ValueError:
					continue
		if num < 0:
			return
		print num
		if len(self.peer_list) < num:
			print "[ERROR] Peer does not exists"
			return
		
		try:
			s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
			s.connect(self.peer_list[num-1])
			print "- Connected with peer -> %s:%d"%(self.peer_list[num-1][0],self.peer_list[num-1][1])
		except socket.error as msg:
			print "[ERROR] Cannot connect to peer -> %s:%d"%(peer_list[0],peer_list[1])
			return
		
		#~ while(True):
			#~ send_data = raw_input('-')
			#~ if(send_data.strip() == r'\quit'):
				#~ s.close()
				#~ break
			#~ try:
				#~ s.send(send_data)
			#~ except socket.error as msg:
				#~ print msg[1]
				#~ return
		c = ChatWindow(s,self.peer_list[num-1])
		c.run()
				
		
class ChatWindow:
	def __init__(self,conn,addr):
		self.root = tk.Tk()
		self.conn = conn
		self.fr = tk.Frame(self.root)
		self.fr.pack()
		self.chatlog = tk.Text(self.fr,state='disabled',width=50,height=10);
		self.inputbox = tk.Text(self.root,state='normal',width=50,height=10);
		self.scrollbar = tk.Scrollbar(self.fr,width=6)
		self.inputbox.bind("<Return>",self.send_data)
		
		self.chatlog.pack(side=tk.LEFT)
		self.scrollbar.pack(side=tk.RIGHT)
		self.inputbox.pack()		
		
		self.chatlog.configure(yscrollcommand = self.scrollbar.set)
		self.scrollbar.configure(command = self.chatlog.yview)
		
		self.chatlog.tag_config('username_sender',background='yellow',foreground="blue")
		self.chatlog.tag_config('username_me',foreground="black")
		self.chatlog.tag_config('sender_text',background='white',foreground="blue")
		self.chatlog.tag_config('me_text',background='white',foreground="black")
		
		Thread(target=self.update_chatlog,args=(conn,addr)).start()
		self.root.protocol("WM_DELETE_WINDOW", self.close_connection)
		
	def run(self):
		self.root.mainloop()
		
	def close_connection(self):
		print "here"
		self.root.destroy()	
		self.conn.close()
		
	def update_chatlog(self,conn,addr):
		while(True):
			buf = conn.recv(1024)
			if not buf:
				#Promt to close the window
				break
			self.chatlog['state'] = 'normal'
			self.chatlog.insert('end',addr[0]+":"+str(addr[1])+"- ",'username_sender')
			self.chatlog.insert('end',buf,'sender_text')
			self.chatlog['state'] = 'disabled'
			self.chatlog.yview(tk.END)
		conn.close()
		
	def send_data(self,event):
		#~ print "HEY"
		text = self.inputbox.get(1.0,'end')
		self.conn.send(text)
		
		self.inputbox.delete(1.0,'end')
		
		self.chatlog['state'] = 'normal'
		self.chatlog.insert('end',"Me- ","username_me")
		self.chatlog.insert('end',text,"me_text")
		self.chatlog['state'] = 'disabled'
		self.chatlog.yview(tk.END)
		return "break"

if __name__== "__main__":
	
	if(len(sys.argv) < 3):
		print "Usage: python client.py <ip> <port>"
		sys.exit(2)
	
	IP = sys.argv[1]
	PORT = int(sys.argv[2])
	
	c = Client(IP,PORT)
	c.command_line()
	#~ c = ChatWindow()
