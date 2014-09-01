from threading import Thread
import time
import datetime

def print_systime1():
	f_one = open('thread1.txt','w')
	for i in xrange(10):
		t1 = datetime.datetime.now()
		t1_string=('%02d:%02d.%d'%(t1.minute,t1.second,t1.microsecond))
		f_one.write(t1_string+'\n')

def print_systime2():
	f_two = open('thread2.txt','w')
	for i in xrange(10):
		t2 = datetime.datetime.now()
		t2_string=('%02d:%02d.%d'%(t2.minute,t2.second,t2.microsecond))
		f_two.write(t2_string+'\n')
	
two_print = Thread(target=print_systime2)
one_print = Thread(target=print_systime1)


one_print.start()
two_print.start()

time.sleep(0.1)

print "Now join..."

one_print.join()
two_print.join()

