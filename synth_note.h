

#ifndef _SYNTH_NOTE_H_
#define _SYNTH_NOTE_H_

#include "sin_wave.h"

struct synth_note
{
	int num_waves;
	sin_wave * waves;
	int note;
	int octave;
	float start_time;
};

void create_note(synth_note * sn, int note, int octave, float start_time);

float sample_note(synth_note * sn, float t);

#endif