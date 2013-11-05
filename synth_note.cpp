

#include "synth_note.h"
#include "sin_wave.h"
#include "frequencies.h"
#include "envelope.h"
#include <stdio.h>
#include <stdlib.h>

void create_note(synth_note * sn, int note, int octave, float start_time, float len)
{
	sn->note = note;
	sn->octave = octave;
	sn->start_time = start_time;
	sn->end_time = start_time + len;

	printf("Start Time: %f\n", sn->start_time);
	printf("End Time: %f\n", sn->end_time);

	//create sin waves
	sin_wave base;
	init(&base, 0.15, 0.0, get_frequency(note, octave));


	create_envelope(&(sn->env), 0.1, len);

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

void destroy_note(synth_note * sn)
{
	free(sn->waves);
}

float sample_note(synth_note * sn, float t)
{
	float sample = 0.0;
	float relative_time = t - sn->start_time;
//	printf("New Sample\n");
	for(int i = 0; i < sn->num_waves; i++)
	{
		sample += sample_wave( &(sn->waves[i]), relative_time);
	}
//	printf("Sample before envelope: %f\n", sample);
	sample = sample * sample_envelope(&(sn->env), relative_time);
//	printf("Sample after  envelope: %f\n", sample);
	return sample;
}
