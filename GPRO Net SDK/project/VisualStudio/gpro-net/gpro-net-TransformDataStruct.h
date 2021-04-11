#pragma once
#include "RakNet/BitStream.h"

struct sSpatialPose
{
    float scale[3];     // non-uniform scale
    float rotate[3];    // orientation as Euler angles
    float translate[3]; // translation

    // read from stream
    RakNet::BitStream& Read(RakNet::BitStream& bitstream)
    {
        int compression = 0;

        //scale
        compression = SpacialCompressor(scale, 0);
        bitstream.Read(compression);

        //rotation
        compression = SpacialCompressor(rotate, 1);
        bitstream.Read(compression);
        
        //translatiom
        compression = SpacialCompressor(translate, 2);
        bitstream.Read(compression);
        
        return bitstream;
    }

    // write to stream
    RakNet::BitStream& Write(RakNet::BitStream& bitstream) const
    {
        int decompression = 0;
        
        //scale
        bitstream.Write(decompression);
        //SpacialDecompressor(decompression);

        //rotation
        bitstream.Write(decompression);
        //SpacialDecompressor(decompression);
        
        //translation
        bitstream.Write(decompression);
        //SpacialDecompressor(decompression);

        return bitstream;
    }

    int SpacialCompressor(float value[3], int srt)
    {
        //32 bits (10 bits per float + 2 extra)
        int compressed = 0;

        /* Compression Algorith Pseudo Code

        //take srt, convert to bit value, and mask to the first 2 bits of compressed to tag the int

        int bitIndex = 20;

        for (int i = 0; i < 3; ++i)
        {
            
            //Convert float value into a bit equivilent
            //temp = value[i] / (const convertVal)

            //mask area of bitshift, and match the 10 bits with compressed float value
            //(compressed & bitmaskValue(1111111111)) : (b << compressed value)

            //Move index right 10 for next operation
            bitIndex -= 10;
        }
        
        */

        return compressed;
    }

    void SpacialDecompressor(int compressed)
    {
        float temp[] = {0.0f, 0.0f, 0.0f};
        int srt = 0;

        /* Decompression Algorith Pseudo Code
        
        //Take the index of the first 2 bits, to find which value set they belong to

        int bitIndex = 20;

        for (int i = 0; i < 3; ++i)
        {
            
            //read the first masked area of 10 bits via bitshifted value and get the value of those bits
            //store the bits

            //covnert the bits to a float value using the conversion constant
            //temp[i] = bits / (const convertVal)

            //Move index right 10 for next operation
            bitIndex -= 10;
        }

        */

        switch (srt)
        {
            case 0:
                scale[0] = temp[0];
                scale[1] = temp[1];
                scale[2] = temp[2];
                break;
            case 1:
                rotate[0] = temp[0];
                rotate[1] = temp[1];
                rotate[2] = temp[2];
                break;
            case 2:
                translate[0] = temp[0];
                translate[1] = temp[1];
                translate[2] = temp[2];
                break;
        }
        
    }
};