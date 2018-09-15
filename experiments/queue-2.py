from ctypes import *
import sys
emv = CDLL("./queue-2.so")
  
if __name__== "__main__":
	emv.main()
