### Voice Activity Detection (VAD)
Given an input audio file, detect noise time-frames and set corresponding bytes to 0 to remove noise.

---

*Andrea Valentinuzzi*

**INSTRUCTIONS**
1. Set up the compiling environment for C++.
2. Compile the code with the command: \
        `g++ -o vad main.cpp VAD.cpp`
3. Run the code with the command: \
        `./vad`
4. Once the code is running, you are presented with the choice of which audio file to use.
5. The code will then read in the audio file and perform the voice activity detection.
6. You can test the results of the VAD by opening the output data files with Audacity (or similar software) with the appropriate parameters.

**NOTE** \
The input files are assumed to be in the inputdata folder.
The output files are stored in the outputdata and outputtxt folders (they must already exist!).
If you want to change VAD parameters such as the amount of packets to look ahead/back and the frequency threshold, you can change the values in the initializer list of the vad class in VAD.cpp.
