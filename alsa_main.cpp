/*This program makes a CEG chord by generating values
from sine waves.

The program uses the ALSA library.

Use option -lasound on compile line.*/

#include "sin_wave.h"
#include "synth_note.h"
#include "frequencies.h"

#include </usr/include/alsa/asoundlib.h>
#include <math.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

static char *device = "default";	/*default playback device */
snd_output_t *output = NULL;
unsigned short buffer[44100*8];	/*sound data*/
#define PI 3.14159

int main(void)
{

	int err, i;
	snd_pcm_t *handle;
	snd_pcm_sframes_t frames;

	// f = 440;
	// sin_wave wav;
	// init(&wav, 0.3, 0.0, (float) f);
	
	synth_note A;
	create_note(&A, 9, -1, 0.0);

	for (i = 0; i < 88200; i++) {
		float t = i / 44100.0;
		//buffer[t + 44100 * i] = sin(2*PI*f/100)*t*(t^t+(t>>15|1)^(t-1280^t)>>10);
		// buffer[t + 44100] = floor(1+sin(2*PI*1000*t/44100))*floor(sin(2*PI*f*t/44100));
		//buffer[t + 44100] *= t*exp(-1*t/44100)/441000;
		buffer[i + 44100] = floor(sample_note(&A, t)) * 10000;
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