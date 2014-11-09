// RadiationFiltering.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <string>

#define SAMPLES   65536

//This is the max array we will need to read in for now.  We have 4 channels with 65536 
//samples.  Change this if we are passing in more channels.
uint16_t allChannelInput[SAMPLES * 4];
uint16_t sampleInput[SAMPLES];
double doubleInput[SAMPLES];

uint16_t output[SAMPLES];
double doubleOutput[SAMPLES];
uint16_t allChannelOutput[SAMPLES * 4];

FILE * sampleFid;
FILE * sampleOutFid;
FILE * sampleOutTextFid;
FILE * coefficientFid;



#define FILTER_LEN  9
double coeffs[FILTER_LEN] =
{
	-0.75,
	1.25,
	0.75,
	-2.25,
	0,
	2.25,
	-0.75,
	-1.25,
	0.75
};

/**
* To parse this file, i need to take the coefficients and then flip them and pad with
* the number of zeroes ..
/

/*bool parseCoefficientsFile(std::string coeffFileName, double * coeffs)
{
	errno_t err;

	// open the input waveform file
	//This file has to reside within the project directory.
	err = fopen_s(&coefficientFid, coeffFileName.c_str(), "r");
	if (err != 0)
	{
		perror("Coefficients File not opened");
		return 1;
	}
}
*/
/** 
* Convert an integer array to a double array
*/
void intToDouble(uint16_t *input, double *output, int length)
{
	int i;

	for (i = 0; i < length; i++) {
		output[i] = (double)input[i];
	}
}


/*
* Parse the input array and try to get the samples that we need to filter out.  Make
* sure to not disturbe the original array as we need it to write our final output. A
* secondary feature will to unmask the upper two bits that are always set to high in 
* the original samples
*/
void parseInputSamplesArray(uint16_t * input, long size, long numberOfChannels, 
							long inputChannel, uint16_t * output)
{
	//Remember we pull out every numberOfChannel'th sample
	for (int i = 0; i < (size/numberOfChannels); ++i)
	{
		output[i] = input[i * numberOfChannels];
		output[i] = output[i] & 0x3FFF;
	}
}


/*
*   Our Samples need to be shifted around the zero point. The input is 14 bit samples.  The 
*   sample value is 16 bits and the samples have the upper two bits set.  Need to unmask 
*   those
*/
void shiftSamplesAroundZero(double * input, long size, double shiftValue)
{

	for (int i = 0; i < SAMPLES; ++i)
	{
		//input[i] &= 0x3F;
		input[i] = input[i] - shiftValue;


	}

}



//First is the definition of the working array to hold the input samples.I have defined the array 
//length to handle up to 80 input samples per function call, and a filter length up to 
//63 coefficients.  In other words, the filter function that uses this working array will 
//support between 1 to SAMPLES input samples, and a filter length between 1 and 63.

// maximum number of inputs that can be handled
// in one function call
#define MAX_INPUT_LEN   SAMPLES
// maximum length of filter than can be handled
#define MAX_FLT_LEN     63
// buffer to hold all of the input samples
#define BUFFER_LEN      (MAX_FLT_LEN - 1 + MAX_INPUT_LEN)

// array to hold input samples
double insamp[BUFFER_LEN];

// FIR init
void firFilterInit(void)
{
	memset(insamp, 0, sizeof(insamp));
}


/**
* Implement the FIR filter 
*/
void firFilter(double *coeffs, double *input, double *output,
	int length, int filterLength)
{
	double acc;     // accumulator for MACs
	double *coeffp; // pointer to coefficients
	double *inputp; // pointer to input samples
	int n;
	int k;

	// put the new samples at the high end of the buffer
	memcpy(&insamp[filterLength - 1], input,
		length * sizeof(double));

	// apply the filter to each input sample
	for (n = 0; n < length; n++) {
		// calculate output n
		coeffp = coeffs;
		inputp = &insamp[filterLength - 1 + n];
		acc = 0;
		for (k = 0; k < filterLength; k++) {
			acc += (*coeffp++) * (*inputp--);
		}
		output[n] = acc;
	}
	// shift input samples back in time for next time
	memmove(&insamp[0], &insamp[length],
		(filterLength - 1) * sizeof(double));

}


/**
* Convert a double array back to an integer array
*/
void convertAndShiftOutput(double *input, uint16_t *output, long length, double shiftValue)
{
	int i;

	//The largest value I can support is 8191
	//The smallest value I can support is 8191
	for (i = 0; i < length; i++) {

		if (input[i] > shiftValue)
		{
			input[i] = shiftValue;
		}
		else if (input[i] < (shiftValue * -1))
		{
			input[i] = shiftValue * -1;
		}
		//shift back up now;
		input[i] = input[i] + 8191.0;

		// convert back to int.
		output[i] = (int16_t)input[i];

		//Also put back the upper two bits
		output[i] = output[i] | 0xC000;
	}

}

/**
* Create the final output to write to the ouput file.
*/
void createFinalOutput(uint16_t * originalInput, uint16_t * input, uint16_t * output, 
						long outputChannel, long  length, long numberOfChannels)
{
	for (int i = 0; i < (length/numberOfChannels); ++i)
		{ 
			for (int y = 0; y < numberOfChannels; ++y)
			{
				if (outputChannel == y)
				{
					output[(i*numberOfChannels) + y] = input[i];
					
					fprintf(sampleOutTextFid, "%hu,", output[(i*numberOfChannels) + y]);
				}
				else
				{
					output[(i*numberOfChannels) + y] = originalInput[(i*numberOfChannels) + y];
					fprintf(sampleOutTextFid, "%hu,", output[(i*numberOfChannels) + y]);
				}
			}
			fprintf(sampleOutTextFid, "\n");
		}
}


/*
 * The arguments will be coefficientsFile, inputFileName, numberOfChannels, inputChannel, outputChannel

*/

int main(int argc, char * argv[])
{

	

	errno_t err;
	int fileSize;
	long numberOfChannels = 0;
	long inputChannel = 0;
	long outputChannel = 0;
	double * coefficients;
	
	if (argc != 6)
	{
		perror("Wrong number of arguments for this program");
		return 1;
	}
	//Get the arguments
	printf("%s\n", argv[2]);
	std::string inputFileName = argv[2];
	char *p;
	numberOfChannels = strtol(argv[3], &p, 10);
	inputChannel = strtol(argv[4], &p, 10);
	outputChannel = strtol(argv[5], &p, 10);
	std::string coefficientsFileName = argv[1];


	//parseCoefficientsFile(coefficientsFileName, coefficients);
	//Under configuration properties for the project you will find the place to put command line options
	// open the input waveform file
	//This file has to reside within the project directory.
	err = fopen_s(&sampleFid, inputFileName.c_str(), "rb");
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

	// initialize the filter
	//firFilterInit();

	//Read in the sample file 16 bits at a time
    //size_t fread(void * ptr, size_t size, size_t count, FILE * stream);
	//Reads an array of count elements, each one with a size of size bytes, from the 
	//stream and stores them in the block of memory specified by ptr.
	fileSize = fread(allChannelInput, sizeof(uint16_t), (SAMPLES * numberOfChannels), sampleFid);

	//Step 1 - pull out the original sample data.
	parseInputSamplesArray(allChannelInput, (SAMPLES * numberOfChannels), numberOfChannels, inputChannel, sampleInput);

	
	//Step 2 - convert to doubles
	intToDouble(sampleInput, doubleInput, SAMPLES);

	//Step 3 - Move the data around the 0 point.  8191 is 0x3FFF/2  (14 bit adc vals)
	shiftSamplesAroundZero(doubleInput, SAMPLES, 8191);


	//Step 4 - Perform the filtering
    //	firFloat(coeffs, floatInput, floatOutput, size,	FILTER_LEN);
	//firFilter(doubleInput, doubleOutput, SAMPLES);

	firFilter(coeffs, doubleInput, doubleOutput, SAMPLES, FILTER_LEN);


	
	//Step 5 - Massage the sampled data
	convertAndShiftOutput(doubleOutput, output, SAMPLES, 8191);

	//Step 6 - Write the final ouput
	createFinalOutput(allChannelInput, output, allChannelOutput, outputChannel, 
						(SAMPLES * numberOfChannels), numberOfChannels);

	// write samples to file
		//fwrite(output, sizeof(int16_t), fileSize, sampleOutFid); 
		//

		//This portion is test code to output our data into a readable format.
		//We are printing the first 1000 entries just to make it easier to read in excel.
		//The input file is currently in 4 Channels (ie: 4 columns of data).  Only the first 
		//channel is valid.  I am simply putting it back into the 4 columns.  This is all
		//hardcoded for now but the number of channels will ultimately be passed in.
		//for (int i = 0; i < 250; ++i)
		//{ 
//			for (int y = 0; y < 4; ++y)
			//{
//				fprintf(sampleOutTextFid, "%hu,", sampleInput[(i * 4) + y]);
			
//			}
			//fprintf(sampleOutTextFid, "\n");
//		}

		//for (int i = 0; i < 250; ++i)
//		{
//			fprintf(sampleOutTextFid, "%hu,%hu\n", sampleInput[i], output[i]);
//		}


		fclose(sampleFid);
		fclose(sampleOutFid);
		fclose(sampleOutTextFid);

	return 0;
}

