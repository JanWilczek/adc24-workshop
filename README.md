# DSP in Practice Workshop Repository

Workshop hosted at the Audio Developer Conference 2024 in Bristol.

[Slides](docs/ADC24_Workshop_Slides_DSP_In_Practice.pdf)

## Getting started

Follow these instructions **before** attending the workshop. There is high risk that because of low WiFi bandwidth at the conference venue, it will take too long or be impossible to set it up on the spot.

### General prerequisites

You need to have `git` installed and in your PATH. Here are the commands that we will use in this workshop:

```bash
git checkout <branch-name> # check out the branch with the given name
git checkout -b <new-branch-name> # create a new branch and check it out
git status # show the status of the working tree
git diff # show the difference between the working tree and the last commit
git add <path-to-file> # stage a file (especially important for untracked files)
git commit -a -m "<commit-message>" # commit all changes with a specific message
```

WAV files and PNG files require `git-lfs`. It should be bundled with your `git` installation but you may need to manually initialize it inside the repository. After cloning this repository, run

```bash
git lfs install
git lfs fetch
```

You can try playing back files in the _data_ folder to check if it worked correctly.

The complete solution to all the tasks are present on the `main` branch. If at any time during the workshop you need to check the reference solution, compare the difference between your working tree and the `main` branch.

```bash
git diff main
```

In Visual Studio Code, there's a wonderful GitLens plugin that allows you to visually compare against the `main` branch via "GitLens: Compare Working Tree with..." command.

### Python environment setup

For this workshop, you need to have Python 3 installed and in your PATH. Code was tested with Python 3.11 and 3.12.

It's most comfortable to work in Python using `venv` (virtual environment).

```bash
# create virtual environment
python3 -m venv venv

# activate it
source venv/bin/activate # macOS, Linux
.\venv\bin\Scripts\Acitvate.ps1 # Windows

# install the dependencies within the environment
pip install .
```

Now, you should be able to run Python tests:

```bash
python -m unittest py/test/test_flanger.py
```

You also should be able to run the audio processing script:


```bash
python py/main.py data/saw200.0Hz5.0s.wav
```

### C++ setup

This workshop requires the following tools installed on your machine and available in your PATH:

- a **working C++ compiler** that supports C++ 20,
- CMake 3.22 or newer,
- a build system like ninja (recommended), MSBuild (bundled with Visual Studio), xcode-build (bundled with Xcode), or make (UNIX Makefiles)

Once this is in place, you can run classic CMake commands:

```bash
# Downloads C++ dependencies and generates the project for ninja (check CMakePresets.json for other generators)
cmake --preset default

# Builds the project
cmake --build --preset default
```

C++ code in this repository is based on my [`audio-plugin-template` repository](https://github.com/JanWilczek/audio-plugin-template).

#### Mac users

The repostitory works with Xcode 16. Older versions of Xcode may be problematic.

#### Windows users

On Windows, to have the compiler on your PATH, you need to run a batch script. Assuming, you have Visual Studio 2022 installed, you can run the following command to initialize C++ environment in your shell.

```batch
"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
```

#### Linux users

To create a Python virtual environment, I needed to install `python3.12-venv` package with

```bash
sudo apt install python3.12-venv
```

Before generating the project above, you need to install [JUCE dependencies](https://github.com/juce-framework/JUCE/blob/master/docs/Linux%20Dependencies.md) with the following command:

```bash
sudo apt update
sudo apt install libasound2-dev libjack-jackd2-dev \
    ladspa-sdk \
    libcurl4-openssl-dev  \
    libfreetype-dev libfontconfig1-dev \
    libx11-dev libxcomposite-dev libxcursor-dev libxext-dev libxinerama-dev libxrandr-dev libxrender-dev \
    libwebkit2gtk-4.1-dev \
    libglu1-mesa-dev mesa-common-dev
```

## Workshop tasks

This workshop consists of several tasks in a logical progression. The goal is to build a working flanger plugin in C++ using the JUCE C++ framework.

You should start each task by checking out the appropriate branch (`task1`, `task2`, etc.).

### Task 1 (`task1` branch): Design

```bash
git checkout task1
```

Given this DSP block diagram of a flanger plugin,

![Workshop Flanger Annotated](docs/img/WorkshopFlangerAnnotated.png)
_Figure 1. Flanger DSP diagram._

write down flanger update equations.

Legend:

- $x[n]$ is the input signal
- $y[n]$ is the output signal
- $x_h[n]$ is a helper signal (used for convenience)
- $D$ is the length of the delay line
- $\text{feedforward}$, $\text{feedback}$, and $\text{blend}$ are coefficients (all equal to 0.7 for a flanger)
- $s_\text{LFO,unipolar}[n]$ is the unipolar LFO signal (a sine in the [0, 1] range)
- $m$ is the modulated delay value
- $x_h[n-D/2]$ denotes the helper signal delayed by $D/2$ samples

> [!NOTE]
> $m$ depends on $n$ but I write $m$ instead of $m[n]$ for simplicity. If you want, you can make the dependence explicit 😉

- [ ] Write down the equation for $y[n]$ (the output).
- [ ] Write down the equation for $x_h[n]$ (the helper signal).
- [ ] Write down the equation for the value of the variable delay $m$ at the $n$-th processed sample.

These equations are the basis of the prototype.

### Task 2 (`task2` branch): Prototype

```bash
git checkout task2
```

- [ ] Run `python py/main.py data/saw200.0Hz5.0s.wav` and check that audibly the output signal is the same as the input signal
    - That's because the flanger returns the input sample in the `process_sample()` function.
- [ ] Inspect the spectrograms generated in the _output_ folder. Are they identical visually?
- [ ] Properly initialize the `Flanger` class instance
    - [ ] Set `max_delay` property of the `Flanger` to 2 milliseconds in samples
    - [ ] Instantiate the `delay_line` property as a `FractionalDelayLine` instance with `max_delay`
    - [ ] Compute $D/2$: half of `max_delay` and store in `middle_delay` property
    - [ ] Initialize `feedback`, `feedforward`, and `blend` properties to 0.7
- [ ] Implement flanger update equations without the LFO in `process_sample()` in _flanger.py_
    - [ ] Write down your update equations in the `process_sample()` function as comments
    - [ ] Calculate the value of the helper signal sample $x_h[n]$
    - [ ] Calculate the output sample $y[n]$ (use a fixed delay instead of a modulated delay for now)
    - [ ] "Update the buffers": write $x_h[n]$ into the delay line
    - [ ] Return the output sample from the function
    - [ ] Run `python py/main.py data/saw200.0Hz5.0s.wav` and check if you hear a spectral coloration of the input.
    - [ ] Inspect the spectrograms generated in the _output_ folder. How is the spectrogram of the output different from the spectrogram of the input? Was it expected?
- [ ] Add an LFO
    - [ ] Instantiate an `LFO` instance in the `Flanger` constructor
    - [ ] Immediately set its freuqency to 0.1 Hz
    - [ ] In `process_sample()`, `get_next_value()` of the LFO and transform it to unipolar [0, 1] range
    - [ ] Calculate the delay value of the modulated tap for the current sample
    - [ ] Replace the previously fixed delay value in output sample computation with the LFO-modulated one
    - [ ] Run `python py/main.py data/saw200.0Hz5.0s.wav` and check if you hear the influence of the LFO (the signal slowly changes its timbre)
    - [ ] Inspect the spectrograms generated in the _output_ folder. How is the spectrogram of the output different from the "fixed modulated delay" version? Was it expected?

### Task 3 (`task3` branch): Implementation part 1

```bash
git checkout task3
```

In this part, your goal is to create a basic working implementation of the flanger plugin. For this purpose, we'll reuse my [`audio-plugin-template` repository](https://github.com/JanWilczek/audio-plugin-template).

This is the class diagram depicting the connections between the classes in a completed project (except handling multiple channels).

![Plugin class diagram](docs/img/PluginClassDiagram.png)
_Figure 2. Plugin class diagram._

I intentionally set a high warning level and enabled "set warnings as errors" compilation option. As C++ developers we should utilize to the fullest what the compiler provides because compilers are highly specialized expert pieces of software. In plugin development we have the luxury of starting often with green-field projects (i.e., create a codebase from scratch). Starting out is the best time to enable said options. Every time you run into a compilation error, try to understand what compiler tells you and fix the warning in code.

In this part, you can put all your code into _Flanger.h_.

- [ ] Generate and compile the C++ project as indicated in the [getting started section](#getting-started). Does the project build successfully? Can you see the "installation" message?
- [ ] Run the end-2-end test with `ctest --preset <used-preset>`. Is the output file the same as the input file?
- [ ] Run the digital audio workstation (DAW) of your choice. Can you put the "Workshop Flanger" plugin on a track? Does it leave the input unaltered?
- [ ] Add feedforward, feedback, and blend coefficients as class members of `Flanger`
- [ ] Add `FractionalDelayLine` member to the `Flanger`
- [ ] Implement `Flanger::reset()` where you should reset relevant member(s).
- [ ] Add `maxDelay_` and `middleDelay_` members to the `Flanger` class (analogous to the Python class)
- [ ] Initialize them in `prepare()` with the same lengths in samples as in Python
- [ ] Implement update equations in `processSample()` as you did in Python. Use a fixed delay tap instead of a modulated delay tap (exactly as the initial version in Python); you'll add LFO later
- [ ] Build your project
- [ ] Run the end-2-end unit test with `ctest --preset <used-preset>` again. Can you hear a spectral coloration in the output file? Is it the same as in the initial (fixed-delay) Python implementation?

### Task 4 (`task4` branch): Implementation part 2

```bash
git checkout task4
```

In the final part of this workshop, you will add the LFO and expose its frequency as a plugin parameter using JUCE APIs. You can implement your own LFO class, however, for simplicity, we'll use JUCE's `juce::dsp::Oscillator`. We'll expose the parameter in the GUI using `juce::GenericAudioProcessorEditor`. Finally, we'll add stereo processing using `juce::dsp::ProcessorDuplicator`; that's why we used `prepare()`/`process()`/`reset()` member function trio.

Usually, we would implement parameter smoothing in this part but `juce::dsp::Oscillator` does that for us already.

> [!NOTE]
> `juce::dsp::Oscillator` adds the generated sample to its input. Thus, to obtain the generated sample, we must call `Oscillator`'s `processSample()` member function with `0`, i.e., `const auto generatedSample = myOscillatorInstance_.processSample(0)`.

- [ ] Add an LFO
    - [ ] Add a `juce::dsp::Oscillator` member and initialize it with a sine oscillator
    - [ ] `prepare()` it in `Flanger::prepare()`
    - [ ] `reset()` it in `Flanger::reset()`
    - [ ] Set its frequency to 0.1 Hz in `Flanger::prepare()`
    - [ ] Compute the unipolar LFO value in `Flanger::processSample()`
    - [ ] Calculate the modulated delay value and use it in the output sample calculation in `Flanger::processSample()`
    - [ ] Compile the project
    - [ ] Run the end-2-end unit test with `ctest --preset <used-preset>` again. Can you hear the influence of the LFO in the output file? Is it the same as in the final (modulated-delay) Python implementation?
- [ ] Make parameters (LFO frequency) adjustable
    - [ ] Create a `juce::AudioParameterValueTreeState apvts_` member in `PluginProcessor`
    - [ ] Create a `juce::AudioParameterFloat` instance upon `PluginProcessor` construction and add it to the `apvts_`; use the following parameters:
        * ID "flangerFrequencyHz"
        * version hint 1
        * name "LFO frequency"
        * range [0.1, 10] Hz with steps of 0.01
        * default value 0.1 Hz
        * label "Hz"
    - [ ] Retrieve the pointer to the parameter (or to its raw value) and store it in a `PluginProcessor`'s member
    - [ ] Create a `Flanger::Parameters` struct with a single `wolfsound::Frequency` member; this is to pass the LFO frequency to the flanger
    - [ ] Initialize the member with 0.1
    - [ ] Create a setter for the parameters, i.e., `Flanger::setParameters(const Parameters&)` where you update the frequency of the `juce::dsp::Oscillator` instance
    - [ ] Set flanger's parameters with this setter in `processBlock()` after retrieving the plugin parameter value
- [ ] Return a `juce::GenericAudioProcessorEditor` in `PluginProcessor::createEditor()` (we're just working on the DSP part after all)
- [ ] Compile the plugin
- [ ] Run the plugin in the DAW. Can you adjust the LFO frequency and the change is audible?
- [ ] Make the plugin stereo using JUCE’s `juce::dsp::ProcessorDuplicator`
    - [ ] Change `PluginProcessor::flanger_` member type to `juce::dsp::ProcessorDuplicator<Flanger<SampleType>, Flanger<SampleType>::Parameters>`
    - [ ] Add an alias in `Flanger::Parameters`: `using Ptr = std::shared_ptr<Parameters>`
    - [ ] Add a `Parameters::Ptr` member in `Flanger`
    - [ ] Add an `explicit` constructor of `Flanger` that takes `Parameters::Ptr` as its only argument and stores it in the just added member (you may need to default the default constructor implementation)
    - [ ] Call `setParameters(*parameters_)` in `process()` before the actual processing begins
    - [ ] Instead of calling `flanger_.setParameters(newParameters)` in `processBlock()`, set the shared state with `*flanger_.state = newParameters`
    - [ ] Compile the project
    - [ ] Run `AudioProcessor.StereoTest` with `ctest --preset <used-preset> -R "StereoTest"`. Does it pass?
    - [ ] In the DAW of your choice, load _data/saw200.0Hz5.0s_stereo.wav_ on a track and put the plugin on the same track. Are both channels processed in the same way? Are changes to LFO frequency reflected accordingly?

