import os
import sys
from ctypes import *
emv = CDLL("./extract_mvs.so")

class MVTractus():
    """docstring for MotionVectorExtractor."""
    def __init__(self, videopath):
        self.videopath = videopath
        self.create_output_dir('./output/mv')

    def get_motion_vectors(self):
        emv.extract_motion_vectors(self.videopath);

    def create_output_dir(self, directory):
        if not os.path.exists(directory):
            os.makedirs(directory)

if __name__=="__main__":
    mvt=MVTractus('../input/ds-easy1.mp4');
    mvt.get_motion_vectors();
