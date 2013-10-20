
#include "frequencies.h"
#include <math.h>


//0 = C
//1 = C#
//2 = D
//3 = D#
//4 = E
//5 = F
//6 = F#
//7 = G
//8 = G#
//9 = A
//10 = A#
//11 = B
float get_frequency(int note, int octave)
{
	float freq = 0.0;
	if(note == 0)
		freq = 261.626;
	if(note == 1)
		freq = 277.183;
	if(note == 2)
		freq = 293.665;
	if(note == 3)
		freq = 311.127;
	if(note == 4)
		freq = 329.628;
	if(note == 5)
		freq = 349.228;
	if(note == 6)
		freq = 369.994;
	if(note == 7)
		freq = 391.995;
	if(note == 8)
		freq = 415.305;
	if(note == 9)
		freq = 440.000;
	if(note == 10)
		freq = 466.164;
	if(note == 11)
		freq = 493.883;

	float octave_multipy = pow(2.0, (float) octave);

	return freq * octave_multipy;
}

