
/*The program uses the ALSA library.

Use option -lasound on compile line.*/

#include "sin_wave.h"
#include "synth_note.h"
#include "frequencies.h"
#include "markov.h"
#include "alsa_utils.h"
#include "play_context.h"

#include </usr/include/alsa/asoundlib.h>
#include <math.h>
#include <stdlib.h>

using namespace std;

#define SAMPLE_FREQUENCY 44100
#define CHANNELS 2
#define BUFFER_TIME 500000
#define PERIOD_TIME 100000


static char *device = const_cast<char *>("default");	/*default playback device */

int poll_loop(snd_pcm_t *handle, signed short *buffer, snd_pcm_channel_area_t * areas, alsa_params_t * alsa_params)
{
	struct pollfd *ufds;	//File discriptor for polling the sound
	signed short *ptr;		//pointer into our buffer
	play_context * pc = (play_context *) malloc(sizeof(play_context));
	int err;	//temporary error holder/number of samples written to alsa during a snd_pcm_writei
	int count;	//number of poll discriptors
	int cptr;	//number of samples we have left to write
	int init;	//1 if we need to wait for poll, 0 otherwise

	markov_chart * music = (markov_chart *) malloc(sizeof(markov_chart));
	read_file(music, const_cast<char *>("basic_markov_chart.mc1"));


	create_context(pc);
	printf("create_context\n");

	count = snd_pcm_poll_descriptors_count (handle);
	if (count <= 0) {
		printf("Invalid poll descriptors count\n");
		return count;
	}
	ufds = (pollfd *) malloc(sizeof(pollfd) * count);

	err = snd_pcm_poll_descriptors(handle, ufds, count);
	if (err < 0) {
		printf("Unable to obtain poll descriptors for playback: %s\n", snd_strerror(err));
		return err;
	}


	init = 1;
	while (1) {
		if (!init) {
			err = wait_for_poll(handle, ufds, count);	//checks 
			if (err < 0) {
				if (snd_pcm_state(handle) == SND_PCM_STATE_XRUN || snd_pcm_state(handle) == SND_PCM_STATE_SUSPENDED) {
					err = snd_pcm_state(handle) == SND_PCM_STATE_XRUN ? -EPIPE : -ESTRPIPE;
					if (xrun_recovery(handle, err) < 0) {
						printf("Write error: %s\n", snd_strerror(err));
						exit(EXIT_FAILURE);
					}
					init = 1;
				} else {
					printf("Wait for poll failed\n");
					return err;
				}
			}
		}

		play_audio(areas, pc, alsa_params, music);	//generate our buffer
//		generate_sine(areas, 0, alsa_params->period_size, &phase, alsa_params);

		ptr = buffer;
		cptr = alsa_params->period_size;

		while (cptr > 0) {	//while we haven't written all of our buffer
			err = snd_pcm_writei(handle, ptr, cptr);
			//regular error checking with recovery
			if (err < 0) {
				if (xrun_recovery(handle, err) < 0) {
					printf("Write error: %s\n", snd_strerror(err));
					exit(EXIT_FAILURE);
				}
				init = 1;
				break; /* skip one period */	//why? I don't know
			}

			//if things are going properly
			//then snd_pcm_state(handle) == SND_PCM_STATE_RUNNING
			//and after we finish writing all of our buffer to alsa
			//we will want to wait for the poll again
			//so set init = 0

			if (snd_pcm_state(handle) == SND_PCM_STATE_RUNNING)
				init = 0;

			//move p our pointer into our buffer the number of samples * number of channels forwards
			ptr += err * alsa_params->channels;

			//decrease the number of samples we have left to write by the number of samples actually written
			cptr -= err;
			if (cptr == 0)
				break;

					/* it is possible, that the initial buffer cannot store */
					/* all data from the last period, so wait awhile */
			err = wait_for_poll(handle, ufds, count);
			if (err < 0) {
				if (snd_pcm_state(handle) == SND_PCM_STATE_XRUN || snd_pcm_state(handle) == SND_PCM_STATE_SUSPENDED) {
					err = snd_pcm_state(handle) == SND_PCM_STATE_XRUN ? -EPIPE : -ESTRPIPE;
					if (xrun_recovery(handle, err) < 0) {
						printf("Write error: %s\n", snd_strerror(err));
						exit(EXIT_FAILURE);
					}
					init = 1;
				} else {
					printf("Wait for poll failed\n");
					return err;
				}
			}
		}
	}


}


int main()
{
	int err;	//temporary variable to hold any errors that we encounter
	snd_pcm_t * handle;	//the handle of our alsa device
	snd_pcm_hw_params_t * hwparams;	//stores all the hardware parameters
	snd_pcm_sw_params_t * swparams;	//stores all the software parameters
	alsa_params_t * alsa_params;	//stores all the other parameters

	signed short * buffer;

	snd_pcm_channel_area_t *areas;	//what is this for again?

	//allocate space for parameters
	alsa_params = (alsa_params_t *) malloc(sizeof(alsa_params_t));
	snd_pcm_hw_params_alloca(&hwparams);
	snd_pcm_sw_params_alloca(&swparams);

	//setup alsa parameters
	alsa_params->rate = SAMPLE_FREQUENCY;
	alsa_params->channels = CHANNELS;
	alsa_params->buffer_time = BUFFER_TIME;
	alsa_params->period_time = PERIOD_TIME;
	alsa_params->format = SND_PCM_FORMAT_S16_LE;
	alsa_params->period_event = 1;	//we do want to be polling


	//open a device and make sure it worked
	err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0);
	if (err < 0) {
		printf("Playback open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	//set hardware params
	err = set_hwparams(handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED, alsa_params);
	if (err < 0) {
		printf("Setting of hwparams failed: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	//set software params
	err = set_swparams(handle, swparams, alsa_params);
	if (err < 0) {
		printf("Setting of swparams failed: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	buffer = (signed short *) malloc((alsa_params->period_size * alsa_params->channels * snd_pcm_format_physical_width(alsa_params->format)) / 8);
	areas = (snd_pcm_channel_area_t *) calloc(alsa_params->channels, sizeof(snd_pcm_channel_area_t));

	for (unsigned int chn = 0; chn < alsa_params->channels; chn++) {
		areas[chn].addr = buffer;
		areas[chn].first = chn * snd_pcm_format_physical_width(alsa_params->format);
		areas[chn].step = alsa_params->channels * snd_pcm_format_physical_width(alsa_params->format);
	}

	poll_loop(handle, buffer, areas, alsa_params);
}