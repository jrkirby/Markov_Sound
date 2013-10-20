
#ifndef _SIN_WAVE_H_
#define _SIN_WAVE_H_

struct sin_wave
{
	float amplitude;
	float phase;
	float frequency;
};

void init(sin_wave * wave, float amp, float ph, float freq);

//sets overtone to the harmonic of the fundamental wave
//Check it: http://en.wikipedia.org/wiki/Harmonic
//any harmonic >= 2 is accepted
//overtone must have space allocated for it
//fundamental must be an initialized wave
void harmonic(sin_wave * overtone, sin_wave * fundamental, int harmonic);


float sample_wave(sin_wave * wave, float time);



#endif