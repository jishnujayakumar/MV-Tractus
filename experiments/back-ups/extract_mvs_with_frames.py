from ctypes import *
import sys
emv = CDLL("./extract_mvs_with_frames.so")
  
if __name__== "__main__":
	print(sys.argv[1])
	emv.extract_motion_vectors(sys.argv[1])
