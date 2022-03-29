/**
 * @file main.cpp
 * @author Andrea Valentinuzzi
 * @brief Main file
 */
#include "VAD.hpp"
#include <iostream>
using namespace std;

// Tests the VAD algorithm to detect if the audio is active or not, then smartly removes the silence
int main()
{
    // Select the audio file using the command line
    int N = -1;
    do
    {
        cout << "\"inputaudioN.data\", insert N (1-5) to choose the audio file: ";
        cin >> N;
    } while (N < 1 || N > 5);
    // Process the audio file
    VAD::vad file = VAD::vad("inputdata/inputaudio" + to_string(N) + ".data");
    // End
    return 0;
}