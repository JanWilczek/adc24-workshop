import numpy as np


def clp2(x):
    x = x - 1
    x = x | (x >> 1)
    x = x | (x >> 2)
    x = x | (x >> 4)
    x = x | (x >> 16)
    return x + 1


class DelayLine:
    """
    Implements a delay line with integer sample delay, used for storing and
    reading delayed audio samples.

    Attributes:
        wrap_mask (int): Mask used for wrapping buffer indices, set to size - 1.
        buffer (numpy.ndarray): A buffer array to hold the delayed samples.
        write_pointer (int): The current index for writing in the buffer.
    """

    def __init__(self, size: int):
        """
        Initializes the delay line buffer with a specified size. The size is
        adjusted to the next power of 2 for efficient wrapping.

        Parameters:
            size (int): The size of the delay line buffer.
        """
        size = clp2(size)
        self.wrap_mask = size - 1
        self.buffer = np.zeros((size,))
        self.write_pointer = 0

    def push_sample(self, sample: float):
        """
        Writes a sample to the delay line buffer at the current write pointer
        location and advances the write pointer with wrapping.

        Parameters:
            sample (float): The sample value to write into the buffer.
        """
        self.buffer[self.write_pointer] = sample
        self.write_pointer = (self.write_pointer + 1) & self.wrap_mask

    def pop_sample(self, delay: int):
        """
        Reads a delayed sample from the buffer based on a specified integer
        delay.

        Parameters:
            delay (int): The delay in samples to read from the buffer.

        Returns:
            float: The delayed sample value.

        Raises:
            IndexError: If the delay is negative or exceeds the buffer size.
        """
        if delay < 0:
            raise IndexError("delay smaller than 0")

        if self.buffer.size <= delay:
            raise IndexError("delay larger than buffer indices")

        # The write pointer always advances after write.
        # Thus, we must use -1 to have delay 0 mark the last written sample.
        read_pointer = (self.write_pointer - 1 - delay) & self.wrap_mask
        return self.buffer[read_pointer]

    def print_buffer(self):
        """
        Prints the current state of the delay line buffer.
        """
        print(self.buffer)


class FractionalDelayLine(DelayLine):
    """
    Extends DelayLine to support fractional sample delay by interpolating between
    two samples, providing non-integer delays.
    """

    def __init__(self, size: int):
        super().__init__(size)

    def pop_sample(self, delay: float):
        """
        Reads a sample from the delay line buffer using fractional delay. Uses
        linear interpolation between two samples for smoother delay response.

        Parameters:
            delay (float): The fractional delay in samples.

        Returns:
            float: The delayed sample value using fractional delay.

        Raises:
            IndexError: If the integer part of delay is out of bounds.
        """
        int_delay = int(delay)

        if int_delay == delay:
            return super().pop_sample(int_delay)

        frac_delay = delay - int_delay

        sample_1 = super().pop_sample(int_delay)
        sample_2 = super().pop_sample(int_delay + 1)

        return frac_delay * sample_2 + (1 - frac_delay) * sample_1


class LFO:
    def __init__(self, sample_rate) -> None:
        self.phase = 0
        self.phase_increment = 0
        self.sample_rate = sample_rate

    def set_frequency(self, frequency_hz):
        self.phase_increment = 2 * np.pi * frequency_hz / self.sample_rate

    def get_next_value(self):
        """
        Generates next LFO output sample. This advances internal phase
        so 2 subsequent calls to this method will result in different values.

        Returns:
            float: the generated sine sample
        """
        value = np.sin(self.phase)
        self.phase += self.phase_increment
        self.phase = np.fmod(self.phase, 2 * np.pi)
        return value


class Flanger:
    def __init__(self, sample_rate):
        max_delay_ms = 2
        self.max_delay = int(np.ceil(sample_rate * max_delay_ms / 1000))
        self.delay_line = FractionalDelayLine(self.max_delay)
        self.middle_delay = self.max_delay // 2
        self.feedback = 0.7
        self.blend = 0.7
        self.feedforward = 0.7

        self.lfo = LFO(sample_rate)
        lfo_frequency_hz = 0.1
        self.lfo.set_frequency(lfo_frequency_hz)

    def process_sample(self, input_sample):
        # Calculate the delay of the modulated tap
        # m = s_{LFO,unipolar}[n] * D
        lfo_unipolar_value = (self.lfo.get_next_value() + 1) / 2
        current_delay = self.max_delay * lfo_unipolar_value
        m = current_delay

        x = input_sample

        # Calculate the helper signal
        # x_h[n] = x[n] + feedback * x_h[n - D/2]
        x_h = x + self.feedback * self.delay_line.pop_sample(self.middle_delay)

        # Calculate the output
        # y[n] = blend * x_h[n] + feedforward * x_h[n - m]
        y = self.blend * x_h + self.feedforward * self.delay_line.pop_sample(m)

        # Update the buffers
        self.delay_line.push_sample(x_h)

        return y


def apply_flanger(samples, sample_rate):
    output = np.zeros_like(samples)
    flanger = Flanger(sample_rate)

    for i in range(samples.shape[0]):
        output[i] = flanger.process_sample(samples[i])

    return output
