import efmvs as e
import np_opencv_module as npcv
# import numpy
import cv2
import sys

# sys.path.insert(0, '/home/praneet/pyboostcvconverter-master')

# import pbcvt

if __name__=="__main__":
	print(sys.argv[1])
	# ret=e.process(2,sys.argv[1])
	fmv=e.FMV(2,sys.argv[1])
	#e.process(2,sys.argv[1])
	fmv.connect(True)# Give true to dump_av_format
	# i=fmv.get_current_frame_number()
	frame_num=0
    # while(i<10){
	while(frame_num==fmv.get_current_frame_number()):
	 	frame_num=fmv.extract_motion_vectors()
	 	cvmat=fmv.getMAT()
	 	if frame_num == 0:
			continue
	 	else:
	 		cv2.imshow("RTMP",cvmat)
	 		cv2.waitKey(10)
		# print(vars(fmv))
		# print(len(cvmat))

	fmv.freeStuff()
