

#include "sin_wave.h"
#include <stdio.h>
#include <math.h>
#define PI 3.14159265


void init(sin_wave * wave, float amp, float ph, float freq)
{
	wave->amplitude = amp;
	wave->phase = ph;
	wave->frequency = freq;
}


//sets overtone to the harmonic of the fundamental wave
//Check it: http://en.wikipedia.org/wiki/Harmonic
//any harmonic >= 2 is accepted
//overtone must have space allocated for it
//fundamental must be 
void harmonic(sin_wave * overtone, sin_wave * fundamental, int harmonic)
{
	overtone->amplitude = fundamental->amplitude / harmonic * 2.0;
	overtone->phase = fundamental->phase;
	overtone->frequency = fundamental->frequency * (float) harmonic;
}


float sample_wave(sin_wave * wave, float t)
{
//	printf("time: %f\n", time);
	return wave->amplitude * sin(2*PI*wave->frequency*t + wave->phase);
}



