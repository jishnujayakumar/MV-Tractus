import unittest
import numpy as np

try:
    import examples
except ImportError:
    raise ImportError('Could not import the examples module. Did you run make?')


class TestExamples(unittest.TestCase):
    '''
    Tests for the examples module.

    '''
    def test_vector_multiplication(self):
        a = np.array([[1., 2., 3.]])
        b = a.reshape(3, 1)
        self.assertEqual(np.equal(examples.mul(a, b), a.dot(b)).all(), True)

    def test_matrix_squaring(self):
        a = np.array([[1., 2., 3.],
                      [4., 5., 6.],
                      [7., 8., 9.]])
        self.assertEqual(np.equal(examples.mul(a, a), a.dot(a)).all(), True)

    def test_matrix_multiplication(self):
        a = np.array([[1., 2., 3., 4.],
                      [4., 3., 2., 1.]])
        b = np.array([[ 1., 0.],
                      [ 2., 3.],
                      [ 4., 4.],
                      [-1., 2.]])
        self.assertEqual(np.equal(examples.mul(a, b), a.dot(b)).all(), True)

    def test_identity_multiplication(self):
        eye = np.eye(3)
        a = np.random.random((3, 3)) * 10
        self.assertEqual(np.equal(examples.mul(eye, a),
                                  examples.mul(a, eye)).all(), True)
        self.assertEqual(np.equal(examples.mul(eye, a), eye.dot(a)).all(), True)

    def test_binarization(self):
        image = np.ones((8, 8), dtype=np.uint8) * 128 # Grayscale
        image[4:6, 4:6] = 126
        expected = np.ones((8, 8), dtype=np.uint8) * 255
        expected[4:6, 4:6] = 0
        binarized = examples.binarize(image, 127)
        self.assertTrue(np.equal(expected, binarized).all())


if __name__ == '__main__':
    unittest.main()
