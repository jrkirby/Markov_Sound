
#include "alsa_utils.h"
#include <math.h>

int set_hwparams(snd_pcm_t *handle, snd_pcm_hw_params_t *hwparams, snd_pcm_access_t access, alsa_params_t *alsaparams)
{
	unsigned int rrate;
	snd_pcm_uframes_t size;
	int err, dir;
		/* choose all parameters */
	err = snd_pcm_hw_params_any(handle, hwparams);
	if (err < 0) {
		printf("Broken configuration for playback: no configurations available: %s\n", snd_strerror(err));
		return err;
	}
		/* set hardware resampling */
	err = snd_pcm_hw_params_set_rate_resample(handle, hwparams, alsaparams->resample);
	if (err < 0) {
		printf("Resampling setup failed for playback: %s\n", snd_strerror(err));
		return err;
	}
		/* set the interleaved read/write format */
	err = snd_pcm_hw_params_set_access(handle, hwparams, access);
	if (err < 0) {
		printf("Access type not available for playback: %s\n", snd_strerror(err));
		return err;
	}
		/* set the sample format */
	err = snd_pcm_hw_params_set_format(handle, hwparams, alsaparams->format);
	if (err < 0) {
		printf("Sample format not available for playback: %s\n", snd_strerror(err));
		return err;
	}
		/* set the count of channels */
	err = snd_pcm_hw_params_set_channels(handle, hwparams, alsaparams->channels);
	if (err < 0) {
		printf("Channels count (%i) not available for playbacks: %s\n", alsaparams->channels, snd_strerror(err));
		return err;
	}
		/* set the stream rate */
	rrate = alsaparams->rate;
	err = snd_pcm_hw_params_set_rate_near(handle, hwparams, &rrate, 0);
	if (err < 0) {
		printf("Rate %iHz not available for playback: %s\n", alsaparams->rate, snd_strerror(err));
		return err;
	}
	if (rrate != alsaparams->rate) {
		printf("Rate doesn't match (requested %iHz, get %iHz)\n", alsaparams->rate, err);
		return -EINVAL;
	}
		/* set the buffer time */
	err = snd_pcm_hw_params_set_buffer_time_near(handle, hwparams, &(alsaparams->buffer_time), &dir);
	if (err < 0) {
		printf("Unable to set buffer time %i for playback: %s\n", alsaparams->buffer_time, snd_strerror(err));
		return err;
	}
	err = snd_pcm_hw_params_get_buffer_size(hwparams, &size);
	if (err < 0) {
		printf("Unable to get buffer size for playback: %s\n", snd_strerror(err));
		return err;
	}
	alsaparams->buffer_size = size;
		/* set the period time */
	err = snd_pcm_hw_params_set_period_time_near(handle, hwparams, &(alsaparams->period_time), &dir);
	if (err < 0) {
		printf("Unable to set period time %i for playback: %s\n", alsaparams->period_time, snd_strerror(err));
		return err;
	}
	err = snd_pcm_hw_params_get_period_size(hwparams, &size, &dir);
	if (err < 0) {
		printf("Unable to get period size for playback: %s\n", snd_strerror(err));
		return err;
	}
	alsaparams->period_size = size;
		/* write the parameters to device */
	err = snd_pcm_hw_params(handle, hwparams);
	if (err < 0) {
		printf("Unable to set hw params for playback: %s\n", snd_strerror(err));
		return err;
	}
	return 0;
}
int set_swparams(snd_pcm_t *handle, snd_pcm_sw_params_t *swparams, alsa_params_t *alsaparams)
{
	int err;
		/* get the current swparams */
	err = snd_pcm_sw_params_current(handle, swparams);
	if (err < 0) {
		printf("Unable to determine current swparams for playback: %s\n", snd_strerror(err));
		return err;
	}
		/* start the transfer when the buffer is almost full: */
		/* (buffer_size / avail_min) * avail_min */
	err = snd_pcm_sw_params_set_start_threshold(handle, swparams, (alsaparams->buffer_size / alsaparams->period_size) * alsaparams->period_size);
	if (err < 0) {
		printf("Unable to set start threshold mode for playback: %s\n", snd_strerror(err));
		return err;
	}
		/* allow the transfer when at least period_size samples can be processed */
		/* or disable this mechanism when period event is enabled (aka interrupt like style processing) */
	err = snd_pcm_sw_params_set_avail_min(handle, swparams, alsaparams->period_event ? alsaparams->buffer_size : alsaparams->period_size);
	if (err < 0) {
		printf("Unable to set avail min for playback: %s\n", snd_strerror(err));
		return err;
	}
		/* enable period events when requested */
	if (alsaparams->period_event) {
		err = snd_pcm_sw_params_set_period_event(handle, swparams, 1);
		if (err < 0) {
			printf("Unable to set period event: %s\n", snd_strerror(err));
			return err;
		}
	}
		/* write the parameters to the playback device */
	err = snd_pcm_sw_params(handle, swparams);
	if (err < 0) {
		printf("Unable to set sw params for playback: %s\n", snd_strerror(err));
		return err;
	}
	return 0;
}

int xrun_recovery(snd_pcm_t *handle, int err)
{
	if (err == -EPIPE) { /* under-run */
		err = snd_pcm_prepare(handle);
		if (err < 0)
			printf("Can't recovery from underrun, prepare failed: %s\n", snd_strerror(err));
		return 0;
	} else if (err == -ESTRPIPE) {
		while ((err = snd_pcm_resume(handle)) == -EAGAIN)
			sleep(1);	/* wait until the suspend flag is released */
						/* really bad way of doing this? */
		if (err < 0) {
			err = snd_pcm_prepare(handle);
			if (err < 0)
				printf("Can't recovery from suspend, prepare failed: %s\n", snd_strerror(err));
		}
		return 0;
	}
	return err;
}

int wait_for_poll(snd_pcm_t *handle, struct pollfd *ufds, unsigned int count)
{
	unsigned short revents;
	while (1) {
		poll(ufds, count, -1);
		snd_pcm_poll_descriptors_revents(handle, ufds, count, &revents);
		if (revents & POLLERR)
			return -EIO;
		if (revents & POLLOUT)
			return 0;
	}
}

void generate_sine(const snd_pcm_channel_area_t *areas, snd_pcm_uframes_t offset, int count, double *_phase, alsa_params_t * alsaparams)
{
	static double max_phase = 2. * M_PI;
	double phase = *_phase;
	double step = max_phase*440.0/(double)alsaparams->rate;
	unsigned char *samples[alsaparams->channels];
	int steps[alsaparams->channels];
	unsigned int chn;
	int format_bits = snd_pcm_format_width(alsaparams->format);
	unsigned int maxval = (1 << (format_bits - 1)) - 1;
	int bps = format_bits / 8; /* bytes per sample */
	int phys_bps = snd_pcm_format_physical_width(alsaparams->format) / 8;
	int big_endian = snd_pcm_format_big_endian(alsaparams->format) == 1;
	int to_unsigned = snd_pcm_format_unsigned(alsaparams->format) == 1;
	int is_float = (alsaparams->format == SND_PCM_FORMAT_FLOAT_LE ||
		alsaparams->format == SND_PCM_FORMAT_FLOAT_BE);
	/* verify and prepare the contents of areas */
	for (chn = 0; chn < alsaparams->channels; chn++) {
		if ((areas[chn].first % 8) != 0) {
			printf("areas[%i].first == %i, aborting...\n", chn, areas[chn].first);
			exit(EXIT_FAILURE);
		}
		samples[chn] = /*(signed short *)*/(((unsigned char *)areas[chn].addr) + (areas[chn].first / 8));
		if ((areas[chn].step % 16) != 0) {
			printf("areas[%i].step == %i, aborting...\n", chn, areas[chn].step);
			exit(EXIT_FAILURE);
		}
		steps[chn] = areas[chn].step / 8;
		samples[chn] += offset * steps[chn];
	}
	/* fill the channel areas */
	while (count-- > 0) {
		union {
			float f;
			int i;
		} fval;
		int res, i;
		if (is_float) {
			fval.f = sin(phase) * maxval;
			res = fval.i;
		} else
		res = sin(phase) * maxval;
		if (to_unsigned)
			res ^= 1U << (format_bits - 1);
		for (chn = 0; chn < alsaparams->channels; chn++) {
			/* Generate data in native endian format */
			if (big_endian) {
				for (i = 0; i < bps; i++)
					*(samples[chn] + phys_bps - 1 - i) = (res >> i * 8) & 0xff;
			} else {
				for (i = 0; i < bps; i++)
					*(samples[chn] + i) = (res >> i * 8) & 0xff;
			}
			samples[chn] += steps[chn];
		}
		phase += step;
		if (phase >= max_phase)
			phase -= max_phase;
	}
	*_phase = phase;
}