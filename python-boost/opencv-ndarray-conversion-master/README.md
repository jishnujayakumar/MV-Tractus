NumPy ndarray ⇋ OpenCV Mat conversion, that just works.
===========================================================

API
-----

- `class NDArrayConverter`: The converter class
        
- `NDArrayConverter::NDArrayConverter()`: Constructor

- `cv::Mat NDArrayConverter::toMat(const PyObject* o)`: Convert a NumPy ndarray 
  to a `cv::Mat`. 

    - o is the object representing the Python representation of the ndarray.

    - **Returns** a `cv::Mat` which is the OpenCV representation of o.

- `PyObject* NDArrayConverter::toNDArray(const cv::Mat& mat)`: Convert a `cv::Mat` to a NumPy ndarray.
    
    - mat is the cv::Mat to convert.

    - **Returns** a `PyObject*` that is the Python representation of an ndarray.


Examples
--------

`examples.cpp` contains

- An implementation of matrix multiplication `mul()` that takes two
  `ndarray` objects, converts them to `cv::Mat`, multiplies them and returns
  the result as an `ndarray`.

- An image binarization function `binarize()` that takes an `ndarray`
  containing a grayscale image and a threshold value, converts the image to
  a `cv::Mat` and thresholds(binarizes) it. It then returns the result as
  a `ndarray`.

- An image display function `display(ndarray)` that just takes any image
  as an `ndarray` object and displays it.

All of these functions are callable from Python.

The plumbing(passing of values to and from Python) is handled by
Boost::Python.

`test.py` contains some testcases that call the aforementioned C++ functions
from Python.


Building and testing
---------------------

After installing [Boost::Python][1] and [NumPy][2](maybe the devel package),
    
    $ make
    $ make test


This builds and tests the project. Specifically, it builds the core converter
in `conversion.o` and a set of examples in `examples.so`, which is a Python
module containing a few examples of using the converter. `test.py` contains
a test-suite to test (and demonstrate) the converter.

[1]: http://www.boost.org/doc/libs/1_53_0/libs/python/doc/index.html
[2]: http://www.numpy.org/

