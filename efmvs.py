# from ctypes import *
# import sys
# efmvs = CDLL("./efmvs.so")
#
# if __name__== "__main__":
# 	print(sys.argv[1])
# 	print(len(sys.argv))
# 	fmv=efmvs.fmv(len(sys.argv), sys.argv[1]);
# 	fmv.connect();
# 	i = fmv.get_current_frame_number();
# 	while(i<10):
# 		fmv.extract_motion_vectors(False); # Give true to dump_av_format
# 		i=fmv.get_current_frame_number();
# 	#fmv.flush_cached_frames();
# 	fmv.freeStuff();

import ctypes
lib = ctypes.cdll.LoadLibrary('./efmvs.so')
class FMV(object):
    def __init__(self, stream_len, stream):

		#Constructor
		lib.FMV_new.argtypes = [ctypes.c_int, ctypes.c_char_p]
        lib.FMV_new.restype = ctypes.c_void_p

		lib.FMV_getSrcFilename.argtypes = [ctypes.c_void_p]
        lib.FMV_getSrcFilename.restype = ctypes.c_char_p

		lib.FMV_setFrameFlag.argtypes = [ctypes.c_bool]
        lib.FMV_setFrameFlag.restype = ctypes.c_void_p

		lib.FMV_getFrameFlag.argtypes = [ctypes.c_void_p]
		lib.FMV_getFrameFlag.restype = ctypes.c_bool

		lib.FMV_connect.argtypes = [ctypes.c_bool]
		lib.FMV_connect.restype = ctypes.c_void_p

		lib.FMV_open_codec_context.argtypes = [ctypes.c_void_p]
		lib.FMV_open_codec_context.restype = ctypes.c_bool

		lib.FMV_decode_packet.argtypes = [ctypes.c_void_p]
		lib.FMV_decode_packet.restype = ctypes.c_int

		lib.FMV_extract_motion_vectors.argtypes = [ctypes.c_void_p]
		lib.FMV_extract_motion_vectors.restype = ctypes.c_void_p

		lib.FMV_flush_cached_frames.argtypes = [ctypes.c_void_p]
		lib.FMV_flush_cached_frames.restype = ctypes.c_int

		lib.FMV_freeStuff.argtypes = [ctypes.c_void_p]
		lib.FMV_freeStuff.restype = ctypes.c_void_p

		lib.FMV_get_current_frame_number.argtypes = [ctypes.c_void_p]
		lib.FMV_get_current_frame_number.restype = ctypes.c_int

		lib.FMV_increase_current_frame_number.argtypes = [ctypes.c_void_p]
		lib.FMV_increase_current_frame_number.restype = ctypes.c_void_p

        self.obj = lib.FMV_new(stream_len, stream)

	def getSrcFilename(self):
		lib.FMV_getSrcFilename(self.obj)

	def FMV_setFrameFlag(self, flag):
		lib.FMV_setFrameFlag(self.obj, flag)

	def FMV_getFrameFlag(self):
		lib.FMV_getFrameFlag(self.obj)

	def FMV_connect(self, dump_av_format):
		lib.FMV_connect(self.obj, dump_av_format)

	def FMV_open_codec_context(self):
		lib.FMV_open_codec_context(self.obj)

	def FMV_decode_packet(self):
		lib.FMV_decode_packet(self.obj)

	def FMV_extract_motion_vectors(self):
		lib.FMV_extract_motion_vectors(self.obj)

	def FMV_flush_cached_frames(self):
		lib.FMV_flush_cached_frames(self.obj)

	def FMV_freeStuff(self):
		lib.FMV_freeStuff(self.obj)

	def FMV_get_current_frame_number(self):
		lib.FMV_get_current_frame_number(self.obj)

	def FMV_increase_current_frame_number(self):
		lib.FMV_increase_current_frame_number(self.obj)
