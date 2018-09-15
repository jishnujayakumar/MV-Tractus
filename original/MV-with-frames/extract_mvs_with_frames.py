import ctypes
extract_mvs = ctypes.cdll.LoadLibrary("extract_mvs_with_frames.so")
extract_mvs.extract_motion_vectors("/home/jishnu/Documents/RBCCPS/video-analytics/motion-vector-based-approach/github_repos/MV-Tractus/input/3-seconds.mp4");	
