import threading

def crawl():
    import urllib2
    data = urllib2.urlopen("http://www.google.com/").read()

    print "Read google.com"

threads = []

for n in range(10):
    thread = threading.Thread(target=crawl)
    thread.start()

    threads.append(thread)

# to wait until all three functions are finished

print "Waiting..."

for thread in threads:
    thread.join()

print "Complete."