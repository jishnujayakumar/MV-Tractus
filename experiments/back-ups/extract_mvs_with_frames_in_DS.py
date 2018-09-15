from ctypes import *
import sys
import threading
import time

emv = CDLL("./extract_mvs_with_frames_in_DS.so")

if __name__== "__main__":
    # while True:
    print "1"
    argu = sys.argv[1]
    print argu
    emv.establishconnection(argu)
    print('connection done')
    emv.extract_motion_vectors(argu)
        # print "\n2"

	# while not emv.isEmpty():
	# 	print "inside while\n"
	# 	print(str(emv.dequeueFrameData()))
