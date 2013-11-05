
//Copyright Jonathan Kirby 2013

#ifndef _ALSA_UTILS_H_
#define _ALSA_UTILS_H_

#include </usr/include/alsa/asoundlib.h>

struct alsa_params_t
{
	int resample;
	snd_pcm_format_t format;
	unsigned int channels;
	unsigned int rate;
	unsigned int buffer_time;
	snd_pcm_sframes_t buffer_size;
	unsigned int period_time;
	snd_pcm_sframes_t period_size;
	int period_event;

};

int set_hwparams(snd_pcm_t * handle, snd_pcm_hw_params_t * hwparams, snd_pcm_access_t access, alsa_params_t * alsaparams);
int set_swparams(snd_pcm_t * handle, snd_pcm_sw_params_t * swparams, alsa_params_t * alsaparams);
int xrun_recovery(snd_pcm_t * handle, int err);
int wait_for_poll(snd_pcm_t * handle, struct pollfd * ufds, unsigned int count);
//void generate_sine(const snd_pcm_channel_area_t *areas, snd_pcm_uframes_t offset, int count, double *_phase, alsa_params_t * alsaparams);

#endif
