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

using namespace std;

#define BIG_NUMBER 100000000
#define SMALLER_NUMBER 10000000

#define NUM_NOTES 100
#define SAMPLE_FREQUENCY 44100

static char *device = const_cast<char *>("default");	/*default playback device */
snd_output_t *output = NULL;
unsigned short buffer[44100*8];	/*sound data*/
snd_pcm_channel_area_t *areas;


static int write_and_poll_loop(snd_pcm_t *handle,
								play_context * context,
								snd_pcm_channel_area_t * areas)
{
	struct pollfd *ufds;
	double phase = 0;
	signed short *ptr;
	int err, count, cptr, init;
	count = snd_pcm_poll_descriptors_count (handle);
	if (count <= 0)
	{
		printf("Invalid poll descriptors count\n");
		return count;
	}
	ufds = malloc(sizeof(struct pollfd) * count);
	if (ufds == NULL)
	{
		printf("No enough memory\n");
		return -ENOMEM;
	}
	if ((err = snd_pcm_poll_descriptors(handle, ufds, count)) < 0) 
	{
		printf("Unable to obtain poll descriptors for playback: %s\n", snd_strerror(err));
		return err;
	}
	init = 1;
	while (1)
	{
		if (!init)
		{
			err = wait_for_poll(handle, ufds, count);
			if (err < 0)
			{
				if (snd_pcm_state(handle) == SND_PCM_STATE_XRUN ||
					snd_pcm_state(handle) == SND_PCM_STATE_SUSPENDED)
				{
					err = snd_pcm_state(handle) == SND_PCM_STATE_XRUN ? -EPIPE : -ESTRPIPE;
					if (xrun_recovery(handle, err) < 0) {
						printf("Write error: %s\n", snd_strerror(err));
						exit(EXIT_FAILURE);
					}
					init = 1;
				} else
				{
				printf("Wait for poll failed\n");
				return err;
				}
			}
		}
		generate_sine(areas, 0, period_size, &phase);
		ptr = samples;
		cptr = period_size;
		while (cptr > 0)
		{
			err = snd_pcm_writei(handle, ptr, cptr);
			if (err < 0)
			{
				if (xrun_recovery(handle, err) < 0)
				{
					printf("Write error: %s\n", snd_strerror(err));
					exit(EXIT_FAILURE);
				}
				init = 1;
				break; /* skip one period */
			}
			if (snd_pcm_state(handle) == SND_PCM_STATE_RUNNING)
				init = 0;
			ptr += err * channels;
			cptr -= err;
			if (cptr == 0)
				break;
			/* it is possible, that the initial buffer cannot store */
			/* all data from the last period, so wait awhile */
			err = wait_for_poll(handle, ufds, count);
			if (err < 0)
			{
				if (snd_pcm_state(handle) == SND_PCM_STATE_XRUN ||
					snd_pcm_state(handle) == SND_PCM_STATE_SUSPENDED)
				{
					err = snd_pcm_state(handle) == SND_PCM_STATE_XRUN ? -EPIPE : -ESTRPIPE;
					if (xrun_recovery(handle, err) < 0)
					{
						printf("Write error: %s\n", snd_strerror(err));
						exit(EXIT_FAILURE);
					}
					init = 1;
				} else
				{
					printf("Wait for poll failed\n");
					return err;
				}
			}
		}
	}
}

int main(void)
{

	int err;
	snd_pcm_t *handle;
	snd_pcm_sframes_t frames;
	snd_pcm_sframes_t frames_written;

	// f = 440;
	// sin_wave wav;
	// init(&wav, 0.3, 0.0, (float) f);
	
	synth_note sNote;

	markov_chart music;
	read_file(&music, const_cast<char *>("basic_markov_chart.mc1"));
	int current_note = 0;
	float current_time = 0.0;


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

	areas = calloc(channels, sizeof(snd_pcm_channel_area_t));
	if (areas == NULL) {
		printf("No enough memory\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}