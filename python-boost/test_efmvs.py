import sys
sys.path.append('.')
from efmvs import FMV

# We'll create a Foo object with a value of 5...
if __name__== "__main__":
	print(sys.argv[1])
	print(len(sys.argv))
	fmv=FMV(len(sys.argv), sys.argv[1]);
	fmv.connect(True);
    
# 	i = fmv.get_current_frame_number();
# 	while(i<10):
# 		fmv.extract_motion_vectors(False); # Give true to dump_av_format
# 		i=fmv.get_current_frame_number();
# 	#fmv.flush_cached_frames();
# 	fmv.freeStuff();
# f = FMV(5)
#
# # Calling f.bar() will print a message including the value...
# print('f=Foo(5)')
# print('f.bar() = {}'.format(f.bar()))
#
# # Now we'll use foobar to add a value to that stored in our Foo object, f
# print("f.foobar(7) = {}".format(f.foobar(7)))
#
# # Now we'll do the same thing - but this time demonstrate that it's a normal
# # Python integer...
#
# x = f.foobar(2)
# print("Type of x; where x = f.foobar(2) is {}".format(type(x)))
