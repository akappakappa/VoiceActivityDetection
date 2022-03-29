/**
 * @file VAD.cpp
 * @author Andrea Valentinuzzi
 * @brief VAD namespace implementation file
 */
#include "VAD.hpp"
#include <algorithm>
#include <cmath>
#include <complex>
#include <iostream>
using namespace std;

namespace VAD
{
    vad::vad(string filename)
        : look_ahead(1), look_back(1), packet_size(160), max_freq(3400), min_freq(200) // Default values
    {
        packet.resize(1 + look_ahead);                               // Contains the current and next_check packets, each packet is a vector of signed char (8 bits)
        has_voice = vector<bool>(look_back + 1 + look_ahead, false); // Contains the voice activity detection flags for all the checked packets, previous, current and next ones
        process(filename);                                           // Start the algorithm
    }

    void vad::process(string filename)
    {
        // Open the input stream and two output streams, one for the data file and the other for a txt report
        ifstream ifs(filename, ifstream::binary);                                                  // ifstream::binary is needed to read the file in binary mode
        ofstream ofs("outputdata/outputVAD" + filename.substr(20, 1) + ".data", ofstream::binary); // ofstream::binary is needed to write the file in binary mode
        ofstream ofstxt("outputtxt/outputVAD" + filename.substr(20, 1) + ".txt", ofstream::out);   // Output file in text mode
        // Main loop
        while (ifs.good())
        {
            read_file(ifs);          // Read one packet from the file
            voice_detection();       // Update the latest next packet voice flag
            write_file(ofs, ofstxt); // Write the current packet to the output file after deciding if it is voice or silence
            shift_packets();         // Shift the packets
            shift_flags();           // Shift the flags
        }
        // Last iterations limit case
        for (int i = 0; i < look_ahead; i++)
        {
            write_file(ofs, ofstxt);
            shift_packets();
            shift_flags();
        }
        // End of the algorithm, close the input and output streams
        ifs.close();
        ofs.close();
        ofstxt.close();
    }

    void vad::read_file(ifstream &ifs)
    {
        for (int i = 0; i < packet_size && ifs.good(); i++) // && ifs.good() since the last packet could be smaller than the others
        {
            signed char temp;
            ifs.read((char *)&temp, sizeof(temp));
            packet[packet.size() - 1].push_back(temp);
        }
    }

    void vad::voice_detection()
    {
        vector<complex<double>> f = fourier(vector<complex<double>>(packet[packet.size() - 1].begin(), packet[packet.size() - 1].end())); // Compute the fourier transform of the latest next packet
        double magnitude = max_magnitude(f);                                                                                              // Compute the max magnitude of the fourier transform
        has_voice[has_voice.size() - 1] = magnitude > min_freq && magnitude < max_freq;                                                   // Return true if the max magnitude is in the range of the voice frequencies, false otherwise
    }

    void vad::write_file(ofstream &ofs, ofstream &ofstxt) const
    {
        if (!packet[0].size()) // If the packet[0] is empty, do nothing, since we're in the first iterations
            return;
        if (any_of(has_voice.begin(), has_voice.end(), [](bool voice)
                   { return voice; })) // Checks the surrounding packets to decide wether to write the current packet or silence
        {
            for (int i = 0; i < packet[0].size(); i++)
            {
                ofs.write((char *)&packet[0][i], sizeof(packet[0][i])); // Write the current packet to the output file
            }
            ofstxt << "1";
        }
        else // The surrounding packets have no voice activity
        {
            vector<signed char> temp(packet[0].size(), 0);
            for (int i = 0; i < temp.size(); i++)
            {
                ofs.write((char *)&temp[i], sizeof(temp[i])); // Write silence to the output file
            }
            ofstxt << "0";
        }
    }

    void vad::shift_packets()
    {
        for (int i = 0; i < packet.size() - 1; i++)
        {
            packet[i] = packet[i + 1]; // Shift the packet vector to the left
        }
        packet.back().clear(); // Clear the last packet
    }

    void vad::shift_flags()
    {
        for (int i = 0; i < has_voice.size() - 1; i++)
        {
            has_voice[i] = has_voice[i + 1]; // Shift the flags vector to the left
        }
        has_voice.back() = false; // Clear the last flag
    }

    vector<complex<double>> fourier(vector<complex<double>> f)
    {
        // Base case
        if (f.size() == 1)
        {
            return f;
        }
        // Even list of complex numbers
        vector<complex<double>> even_packets;
        for (int i = 0; i < f.size(); i += 2)
        {
            even_packets.push_back(f[i]);
        }
        even_packets = fourier(even_packets);
        // Odd list of complex numbers
        vector<complex<double>> odd_packets;
        for (int i = 1; i < f.size(); i += 2)
        {
            odd_packets.push_back(f[i]);
        }
        odd_packets = fourier(odd_packets);
        // Combine the two lists and compute the fourier transform
        vector<complex<double>> ret(f.size());
        for (int i = 0; i < f.size() / 2; i++)
        {
            complex<double> w(cos(2 * PI * i / f.size()), sin(2 * PI * i / f.size()));
            ret[i] = even_packets[i] + w * odd_packets[i];
            ret[i + f.size() / 2] = even_packets[i] - w * odd_packets[i];
        }
        return ret;
    }
    double max_magnitude(vector<complex<double>> m)
    {
        double max = -1;
        for (int i = 0; i < m.size(); i++)
        {
            if (sqrt(pow(m[i].real(), 2) + pow(m[i].imag(), 2)) > max)
            {
                max = sqrt(pow(m[i].real(), 2) + pow(m[i].imag(), 2)); // The magnitude of the complex number is the square root of the sum of the squares of the real and imaginary parts
            }
        }
        return max;
    }
} // namespace VAD