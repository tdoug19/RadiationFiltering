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



/*
 * The arguments will be coefficientsFile, inputFileName, numberOfChannels, inputChannel, outputChannel

*/

int _tmain(int argc, _TCHAR* argv[])
{
	int size;

	FILE * sampleFid;
	FILE * sampleOutFid;
	FILE * sampleOutTextFid;
	std::cout << "Hello World" << std::endl;
	errno_t err;


	// open the input waveform file
	//This file has to reside within the project directory.
	err = fopen_s(&sampleFid, "sample9MHz4CH.bin", "rb");
	if (err != 0)
	{
		perror("File not opened");
		return 1;
	}
	
	// open the output waveform file
	err = fopen_s(&sampleOutFid, "Outsample.bin", "wb");
	if (err != 0)
	{
		perror("File not opened");
		return 1;
	}

	err = fopen_s(&sampleOutTextFid, "OutsampleText.csv", "wt");
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

		//This portion is test code to output our data into a readable format.
		//We are printing the first 1000 entries just to make it easier to read in excel.
		//The input file is currently in 4 Channels (ie: 4 columns of data).  Only the first 
		//channel is valid.  I am simply putting it back into the 4 columns.  This is all
		//hardcoded for now but the number of channels will ultimately be passed in.
		for (int i = 0; i < 250; ++i)
		{ 
			for (int y = 0; y < 4; ++y)
			{
				fprintf(sampleOutTextFid, "%hu,", output[(i*4)+y]);
			
			}
			fprintf(sampleOutTextFid, "\n");
		}
//	} while (size != 0);

		fclose(sampleFid);
		fclose(sampleOutFid);
		fclose(sampleOutTextFid);







	return 0;
}

