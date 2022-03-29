/**
 * @file VAD.hpp
 * @author Andrea Valentinuzzi 1219617
 * @brief VAD namespace header file
 */
#ifndef VAD_HPP
#define VAD_HPP
#include <complex>
#include <fstream>
#include <string>
#include <vector>

/**
 * @namespace VAD
 * @brief Group of functions to detect voice activity. \n
 * Contains the vad class and the helper functions to detect voice activity.
 */
namespace VAD
{
    /**
     * @class vad
     * @brief Voice Activity Detection from a sample of audio file. \n
     * The program reads a sample of audio file and detects if the audio is active or not, then smartly removes the background noise when not speaking.
     *
     * LOGIC:
     * -# Read the audio file
     * -# Detect if there is voice or not
     * -# If there is voice, write the audio file in the output file
     * -# If there is no voice, write the silence in the output file
     * -# Repeat the process until the end of the file
     * -# Last iterations limit case
     * -# End
     *
     * DECISIONS:
     * Every time the program reads a new packet, it is put in the vector packet[packet.size() - 1] and the result of the voice detection for that packet is put in the vector has_voice[has_voice.size() - 1].
     * Then if any of the values of the vector has_voice is true, the current packet is written in the output file, since it means that there was voice detected nearby or in the current packet.
     * If instead all the values of the vector has_voice are false, the current packet is written in the output file as silence, since it means that there was no voice detected nearby or in the current packet.
     * After that the values of the packet and has_voice vectors are shifted to the left to make room for the new packet.
     *
     * LIMITCASES:
     * The first iterations are not a limit case, since the writing is based on the size of the packet[0] vector, which is 0 for the first iterations.
     * The last iterations decide how to output the last packets, as voice or silence. They have to be done outside the main loop since the input file stream returns false for ifs.good() at that point.
     *
     * NOTE:
     * The best results are obtained when the program looks ahead and back one packet.
     * This allows for a more natural detection of voice, otherwise the words get cut off.
     * Looking ahead one packet means that the original audio will start playing 20ms (one packet) before the next voice section.
     * Looking back one packet means that the original audio will stop playing 20ms (one packet) after the last voice section.
     * This takes in account the fact that usually when speaking the voice takes some time to reach peak intensity (the first letter) and then fades out (the last letter).
     */
    class vad
    {
    public:
        /**
         * @brief Construct a new VAD object, then calls read_file() to process the audio file.
         * @param filename The path/filename of the audio file.
         */
        vad(std::string filename);

    private:
        /** Number of packets to look ahead */
        const int look_ahead;
        /** Number of packets to look behind */
        const int look_back;
        /** Number of samples per packet */
        const int packet_size;
        /** Max frequency that is considered to be a voice */
        const int max_freq;
        /** Min frequency that is considered to be a voice */
        const int min_freq;
        /** Vector that holds the current packet and all the ones to look ahead, each packet is a vector of signed char (8 bits) */
        std::vector<std::vector<signed char>> packet;
        /** Vector of voice flags that keeps track of which packets have been detected as voice, previous, current and next ones */
        std::vector<bool> has_voice;

        /**
         * @brief Starts the voice activity detection algorithm, reading packets from the file, and writing the output file.
         * @param filename The path/filename of the audio file.
         */
        void process(std::string filename);
        /**
         * @brief Reads a packet from the audio file and puts it in the packet vector.
         * @param ifs The input file stream.
         */
        void read_file(std::ifstream &ifs);
        /**
         * @brief Given a vector of audio samples, decides if there is voice activity or not. \n
         * Uses the fourier transform to get the frequency spectrum of the audio and makes a decision based on the human voice frequency spectrum. \n
         * Updates the has_voice[3] flag.
         */
        void voice_detection();
        /**
         * @brief Write the current packet to the output file after deciding if it is voice or silence.
         * @param ofs The output file stream for the data file.
         * @param ofstxt The output file stream for the text file.
         */
        void write_file(std::ofstream &ofs, std::ofstream &ofstxt) const;
        /**
         * @brief Shift the packet vector to the left and clear the last element.
         */
        void shift_packets();
        /**
         * @brief Shift the has_voice vector to the left and reset the last flag.
         */
        void shift_flags();
    };

    /** PI constant */
    const double PI = 3.14159265358979323846;

    /**
     * @brief Given a vector of audio samples, calculates the frequency spectrum of the audio. \n
     * Based on the Cooley–Tukey FFT algorithm.
     * @param f The vector of audio samples.
     * @return A vector of complex numbers representing the frequency spectrum of the audio.
     */
    std::vector<std::complex<double>> fourier(std::vector<std::complex<double>> f);
    /**
     * @brief Finds the highest magnitude frequency in the vector that will then be used to decide if there is voice activity or not.
     * @param m The vector of complex numbers representing the frequency spectrum of the audio.
     * @return The highest magnitude frequency in the vector.
     */
    double max_magnitude(std::vector<std::complex<double>> m);
} // namespace VAD
#endif