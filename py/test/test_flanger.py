import unittest
from ..flanger.flanger import DelayLine, FractionalDelayLine, clp2


class DelayLineTest(unittest.TestCase):
    def test_clp2(self):
        self.assertEqual(1 << 32, clp2((1 << 32) - 1))
        self.assertEqual(1 << 33, clp2((1 << 33) - 1))
        self.assertEqual(1 << 64, clp2((1 << 64) - 1))

    def test_simple_delay(self):
        delay_line = DelayLine(8)
        delay_line.push_sample(1)
        delay_line.push_sample(2)
        delay_line.push_sample(3)
        delay_line.push_sample(4)
        delay_line.push_sample(5)

        self.assertEqual(5, delay_line.pop_sample(0))
        self.assertEqual(4, delay_line.pop_sample(1))
        self.assertEqual(3, delay_line.pop_sample(2))
        self.assertEqual(2, delay_line.pop_sample(3))
        self.assertEqual(1, delay_line.pop_sample(4))
        self.assertEqual(0, delay_line.pop_sample(5))

        with self.assertRaises(IndexError):
            delay_line.pop_sample(-1)
        with self.assertRaises(IndexError):
            delay_line.pop_sample(8)
        with self.assertRaises(IndexError):
            delay_line.pop_sample(9)

    def test_wrap(self):
        delay_line = DelayLine(4)
        delay_line.push_sample(1)
        delay_line.push_sample(2)
        delay_line.push_sample(3)
        delay_line.push_sample(4)
        delay_line.push_sample(5)

        self.assertEqual(5, delay_line.pop_sample(0))
        self.assertEqual(4, delay_line.pop_sample(1))
        self.assertEqual(3, delay_line.pop_sample(2))
        self.assertEqual(2, delay_line.pop_sample(3))
        with self.assertRaises(IndexError):
            delay_line.pop_sample(4)
        with self.assertRaises(IndexError):
            delay_line.pop_sample(5)


class FractionalDelayLineTest(unittest.TestCase):
    def test_interpolation(self):
        delay_line = FractionalDelayLine(4)
        delay_line.push_sample(1)
        delay_line.push_sample(2)
        self.assertEqual(1.5, delay_line.pop_sample(0.5))
        self.assertEqual(1.1, delay_line.pop_sample(0.9))
        self.assertAlmostEqual(1.9, delay_line.pop_sample(0.1))


if __name__ == "__main__":
    unittest.main()
