from ctypes import *
import sys
efmvs = CDLL("./efmvs_backup.so")
  
if __name__== "__main__":
	print(sys.argv[1])
	efmvs.process(len(sys.argv[1]),sys.argv[1])
