import argparse
from pathlib import Path
import soundfile as sf
from flanger.flanger import apply_flanger


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


if __name__ == "__main__":
    main()
