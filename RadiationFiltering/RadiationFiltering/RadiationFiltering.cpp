// RadiationFiltering.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <string>

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
* Parse the input array and try to get the samples that we need to filter out.  Make
* sure to not disturbe the original array as we need it to write our final output.
*/
bool parseInputSamplesArray(uint16_t * input, uint16_t size, uint16_t numberOfChannels, 
							uint16_t inputChannel, uint16_t * output)
{





	return true;

}


/*
 * The arguments will be coefficientsFile, inputFileName, numberOfChannels, inputChannel, outputChannel

*/

int main(int argc, char * argv[])
{

	FILE * sampleFid;
	FILE * sampleOutFid;
	FILE * sampleOutTextFid;

	errno_t err;
	int fileSize;
	


	if (argc != 6)
	{
		perror("Wrong number of arguments for this program");
	}
	//Get the arguments
	printf("%s\n", argv[2]);
	std::string inputFileName = argv[2];



	// open the input waveform file
	//This file has to reside within the project directory.
	err = fopen_s(&sampleFid, "inputFileName", "rb");
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


	//Read in the sample file 16 bits at a time
    //size_t fread(void * ptr, size_t size, size_t count, FILE * stream);
	//Reads an array of count elements, each one with a size of size bytes, from the 
	//stream and stores them in the block of memory specified by ptr.
	fileSize = fread(input, sizeof(uint16_t), SAMPLES, sampleFid);
	
	//if(!(parseInputSamplesArray(uint16_t * input, uint16_t size, uint16_t numberOfChannels,
//		uint16_t inputChannel, uint16_t * output)



//
//	sampleFid = fopen("sample.bin", "rb");
	//if (sampleFid == 0) {
//		std::cout << "couldn't open input.pcm" << std::endl;
		//return 1;
	//}




	// process all of the samples
	//do {
	    // read samples from file

		// convert to doubles
		intToFloat(input, floatInput, fileSize);
		// perform the filtering
//		firFloat(coeffs, floatInput, floatOutput, size,	FILTER_LEN);
		// convert to ints



		// Use FloatInput until I put in the FIR code.
		floatToInt(floatInput, output, fileSize);
		// write samples to file
		fwrite(output, sizeof(int16_t), fileSize, sampleOutFid); 
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

