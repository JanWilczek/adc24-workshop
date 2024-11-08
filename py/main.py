import argparse
from pathlib import Path
import soundfile as sf
from flanger.flanger import apply_flanger
from dspyplot.stft import plot_spectrogram_and_save


def argument_parser():
    parser = argparse.ArgumentParser()
    parser.add_argument("filepath", help="path to the file to process with flanger")
    return parser


def main():
    # Parse command-line arguments
    args = argument_parser().parse_args()

    # Extract the path to the file to process
    input_file_path = Path(args.filepath)

    # Read the file
    samples, sample_rate = sf.read(input_file_path)

    # Apply flanger processing
    processed = apply_flanger(samples, sample_rate)

    # Create the output directory if doesn't exist
    OUTPUT_PATH = Path(__file__).parent.parent / "output"
    OUTPUT_PATH.mkdir(exist_ok=True, parents=False)
    output_file_path = OUTPUT_PATH / input_file_path.name

    # Write the processed samples
    sf.write(output_file_path, processed, sample_rate)

    # Create spectrograms of input and output
    fft_size = 2048
    hop_size = fft_size // 8
    window_size = fft_size // 2
    plot_spectrogram_and_save(
        samples,
        sample_rate,
        OUTPUT_PATH / f"{input_file_path.name}_input.png",
        fft_size=fft_size,
        hop_size=hop_size,
        window_size=window_size,
    )
    plot_spectrogram_and_save(
        processed,
        sample_rate,
        OUTPUT_PATH / f"{input_file_path.name}_output.png",
        fft_size=fft_size,
        hop_size=hop_size,
        window_size=window_size,
    )


if __name__ == "__main__":
    main()
