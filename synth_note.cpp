

#include "synth_note.h"
#include "sin_wave.h"
#include "frequencies.h"
#include <stdlib.h>

void create_note(synth_note * sn, int note, int octave, float start_time)
{
	sn->note = note;
	sn->octave = octave;
	sn->start_time = start_time;

	//create sin waves
	sin_wave base;
	init(&base, 0.2, 0.0, get_frequency(note, octave));


	sn->num_waves = 8;
	sn->waves = (sin_wave *) malloc(sizeof(sin_wave) * sn->num_waves);
	sn->waves[0] = base;
	harmonic(&(sn->waves[1]), &(sn->waves[0]), 2);
	harmonic(&(sn->waves[2]), &(sn->waves[0]), 3);
	harmonic(&(sn->waves[3]), &(sn->waves[0]), 4);
	harmonic(&(sn->waves[4]), &(sn->waves[0]), 5);
	harmonic(&(sn->waves[5]), &(sn->waves[0]), 6);
	harmonic(&(sn->waves[6]), &(sn->waves[0]), 7);
	harmonic(&(sn->waves[7]), &(sn->waves[0]), 8);
}

float sample_note(synth_note * sn, float t)
{
	float sample = 0.0;
	for(int i = 0; i < sn->num_waves; i++)
	{
		sample += sample_wave( &(sn->waves[i]), t);
	}
	return sample;
}
