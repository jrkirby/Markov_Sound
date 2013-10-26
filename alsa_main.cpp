/*This program makes a CEG chord by generating values
from sine waves.

The program uses the ALSA library.

Use option -lasound on compile line.*/

#include "sin_wave.h"
#include "synth_note.h"
#include "frequencies.h"
#include "markov.h"

#include </usr/include/alsa/asoundlib.h>
#include <math.h>
#include <iostream>
#include <stdlib.h>

#define NUM_NOTES 100

using namespace std;

static char *device = "default";	/*default playback device */
snd_output_t *output = NULL;
unsigned short buffer[44100*8 * NUM_NOTES];	/*sound data*/
#define PI 3.14159

int main(void)
{

	int err;
	snd_pcm_t *handle;
	snd_pcm_sframes_t frames;

	// f = 440;
	// sin_wave wav;
	// init(&wav, 0.3, 0.0, (float) f);


	
	synth_note sNote;

	markov_chart music;
	read_file(&music, "basic_markov_chart.mc1");
	int current_note = 0;
	float current_time = 0.0;

	for(int note = 0; note < NUM_NOTES; note++)
	{
		create_note(&sNote, current_note, -1, current_time, 1.0);
		for (int i = 0; i < 88200; i++) {
			buffer[i + (44100 * note)] = floor(sample_note(&sNote, current_time)) * 10000;
			current_time += i / 44100.0;
		}
		current_note = evaluate_chart(&music, current_note);
		destroy_note(&sNote);
	}
	err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0);
	if (err < 0) {
		printf("Playback open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}
	err = snd_pcm_set_params(handle,
							 SND_PCM_FORMAT_S16_LE,
							 SND_PCM_ACCESS_RW_INTERLEAVED,
							 1,
							 44100,
							 1,
							 100000);
	if (err < 0) {	/* 0.5sec */
		printf("Playback open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}	
	frames = snd_pcm_writei(handle, buffer, sizeof(buffer)/8);
	if (frames < 0)
		frames = snd_pcm_recover(handle, frames, 0);
	if (frames < 0) {
			printf("snd_pcm_writei failed: %s\n", snd_strerror(err));
	}
	cout << "DONE!" << endl;
	snd_pcm_close(handle);
	return 0;
}