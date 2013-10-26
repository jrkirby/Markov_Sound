

#ifndef _SYNTH_NOTE_H_
#define _SYNTH_NOTE_H_

#include "sin_wave.h"
#include "envelope.h"

struct synth_note
{
	int num_waves;
	sin_wave * waves;
	int note;
	int octave;
	float start_time;
	envelope env;
};

void create_note(synth_note * sn, int note, int octave, float start_time, float len);
void destroy_note(synth_note * sn);

float sample_note(synth_note * sn, float t);

#endif