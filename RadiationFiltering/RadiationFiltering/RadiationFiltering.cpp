// RadiationFiltering.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <stdint.h>
#include <stdio.h>

#define SAMPLES   65536

uint16_t input[SAMPLES];
double floatInput[SAMPLES];

uint16_t output[SAMPLES];
double floatOutput[SAMPLES];



void intToFloat(uint16_t *input, double *output, int length)
{
	int i;

	for (i = 0; i < length; i++) {
		output[i] = (double)input[i];
	}
}

void floatToInt(double *input, uint16_t *output, int length)
{
	int i;

	for (i = 0; i < length; i++) {
		//This stuff is making sure the data is not out of range.
//		if (input[i] > 32767.0) {
//			input[i] = 32767.0;
//		}
//		else if (input[i] < -32768.0) {
//			input[i] = -32768.0;
//		}
		// convert
		output[i] = (int16_t)input[i];
	}

}





int _tmain(int argc, _TCHAR* argv[])
{
	int size;

	FILE * sampleFid;
	FILE * sampleOutFid;
	FILE * sampleOutTFid;
	std::cout << "Hello World" << std::endl;
	errno_t err;


	// open the input waveform file
	err = fopen_s(&sampleFid, "C:\\Users\\tdouglas\\Documents\\GitHub\\Radiation\\RadiationFiltering\\sample.bin", "rb");
	if (err != 0)
	{
		perror("File not opened");
		return 1;
	}
	
	// open the output waveform file
	err = fopen_s(&sampleOutFid, "C:\\Users\\tdouglas\\Documents\\GitHub\\Radiation\\RadiationFiltering\\Outsample.bin", "wb");
	if (err != 0)
	{
		perror("File not opened");
		return 1;
	}

	err = fopen_s(&sampleOutTFid, "C:\\Users\\tdouglas\\Documents\\GitHub\\Radiation\\RadiationFiltering\\OutsampleText.csv", "wt");
	if (err != 0)
	{
		perror("File not opened");
		return 1;
	}
	
//
//	sampleFid = fopen("sample.bin", "rb");
	//if (sampleFid == 0) {
//		std::cout << "couldn't open input.pcm" << std::endl;
		//return 1;
	//}


	uint16_t test = 65000;

	// process all of the samples
	//do {
	    // read samples from file
		size = fread(input, sizeof(uint16_t), SAMPLES, sampleFid);
		// convert to doubles
		intToFloat(input, floatInput, size);
		// perform the filtering
//		firFloat(coeffs, floatInput, floatOutput, size,	FILTER_LEN);
		// convert to ints



		// Use FloatInput until I put in the FIR code.
		floatToInt(floatInput, output, size);
		// write samples to file
		fwrite(output, sizeof(int16_t), size, sampleOutFid); 
		//
		for (int i = 0; i < 1000; ++i)
		{ 
			fprintf(sampleOutTFid, "%hu\n", output[i]);
		}
//	} while (size != 0);










	return 0;
}

