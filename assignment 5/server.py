import sys,socket,signal
from threading import Thread,Lock

class Server():
	
	def __init__(self,addr='',port=8080):
		self.connected_hosts = dict()
		self.accepted_connections = []
		self.sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
		try:
			self.sock.bind((addr,port))
			signal.signal(signal.SIGINT,self.ctrlcHandler)
			self.addr = addr
			self.port = port
		except socket.error as msg:
			self.sock.close()
			print "[ERR] Cannot bind address %s:%d; Error No. %s; Error message- %s"%(addr,port,msg[0],msg[1])
			sys.exit(1)
		self.mutex = Lock()
		
		
	def run(self):
		self.sock.listen(10)
		print "[INFO] Starting to listen at %s"%(str(self.port),)
		while(1):
			try:
				conn, addr = self.sock.accept()
				self.accepted_connections.append(conn)
				print "[INFO]Connected with %s:%s"%(addr[0],str(addr[1]))
				Thread(target=self.clientHandler,args=(conn,addr)).start()
			except socket.error:
				return
			
	def clientHandler(self,conn,addr):
		
		while(True):
			data = conn.recv(1024)
			port,data = data.split(';')			
			com,port = port.split(':')
			if com.strip() == 'PORT':		
				self.mutex.acquire()
				self.connected_hosts[addr[0]+":"+port] = 1
				self.mutex.release()
				break;
		
		
		while(True):
			print "again",
			data += conn.recv(1024);
			print "OK1"
			if not data:
				break;
			print "OK2"
			if data.strip() == 'SEND_PEER_LIST':
				self.send_peer_list(conn)
			elif data.strip() == 'CLOSE':
				break
			data = ""
		
		#Clean up code
		self.mutex.acquire()
		del self.connected_hosts[addr[0]+":"+port]
		self.mutex.release()
		print "Done"
		
	def send_peer_list(self,conn):
		
		buff = ""
		print "Here1"
		self.mutex.acquire()
		print "Here2"
		for i in self.connected_hosts:
			buff += i +"\n"
		self.mutex.release()
		print buff
		conn.send(str(len(buff)) +";\n"+ buff)
		
		#~ buff = '126.0.0.0:12345\n'
		#~ conn.send(str(len(buff)) +";\n"+ buff)
		
	def ctrlcHandler(self,signal, frame):
		print "Closing server"
		for c in self.accepted_connections:
			c.close()
		self.sock.close()
		
		
if __name__ == "__main__":
	s = Server('',12348);
	s.run()
