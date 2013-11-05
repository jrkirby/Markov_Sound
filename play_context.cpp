
#include "play_context.h"
#include "note_list.h"
#include "markov.h"
#include "alsa_utils.h"

#include </usr/include/alsa/asoundlib.h>
#include <stdlib.h>


void create_context(play_context * pc)
{
	pc->current_time = 0.0;

	create_list(&(pc->playing_notes));
	create_list(&(pc->future_notes));
	create_list(&(pc->past_notes));

	pc->last_note = 0;

	pc->gen_time = 0.0;
	pc->beat_length = 0.5;
}

void gen_next_notes(play_context * pc, markov_chart * music)
{
	int current_note;
	synth_note temp_note;
	while(pc->current_time >= pc->gen_time - pc->beat_length)
	{
		current_note = evaluate_chart(music, pc->last_note);
		printf("Generating %d: \n", current_note);
		create_note(&temp_note, current_note, -1, pc->gen_time, 0.5);

		printf("Note octave: %d\n", temp_note.octave);

		add_note(&(pc->future_notes), temp_note);

		pc->gen_time += pc->beat_length;
		pc->last_note = current_note;
	}
	printf("Number of future notes:  %d\n", pc->future_notes.notes->len);
	printf("Number of current notes:  %d\n", pc->playing_notes.notes->len);
	printf("Number of past notes:  %d\n", pc->past_notes.notes->len);
}

void clean_context(play_context * pc, float t)
{
	move_started(&(pc->playing_notes), &(pc->future_notes), t);
	move_finished(&(pc->past_notes), &(pc->playing_notes), t);
}


void play_audio(const snd_pcm_channel_area_t *areas, play_context * pc, alsa_params_t * alsaparams, markov_chart * music)
{
	unsigned char *samples[alsaparams->channels];
	double step = 1.0 / (double)alsaparams->rate;
	int steps[alsaparams->channels];
	unsigned int chn;
	int format_bits = snd_pcm_format_width(alsaparams->format);
	unsigned int maxval = (1 << (format_bits - 1)) - 1;
	int bps = format_bits / 8;
	int phys_bps = snd_pcm_format_physical_width(alsaparams->format) / 8;
	int big_endian = snd_pcm_format_big_endian(alsaparams->format) == 1;
	int to_unsigned = snd_pcm_format_unsigned(alsaparams->format) == 1;
	int is_float = (alsaparams->format == SND_PCM_FORMAT_FLOAT_LE ||
		alsaparams->format == SND_PCM_FORMAT_FLOAT_BE);
	int count = alsaparams->period_size;
	float sample_time = pc->current_time;

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
//		samples[chn] += offset * steps[chn];
	}


	gen_next_notes(pc, music);
		/* fill the channel areas */
	while (count-- > 0) {
//		printf("Sample Time: %f\n", sample_time);
		clean_context(pc, sample_time);

		union {
			float f;
			int i;
		} fval;
		int res, i;
		if (is_float) {
			fval.f = sample_list(&(pc->playing_notes), sample_time) * maxval;
			res = fval.i;
		}
		else
		{
			res = sample_list(&(pc->playing_notes), sample_time) * maxval;
		}
		if (to_unsigned)
			res ^= 1U << (format_bits - 1);
//		printf("HOw many times?\n");
		for (chn = 0; chn < alsaparams->channels; chn++) {
			/* Generate data in native endian format */
			if (big_endian) {
				for (i = 0; i < bps; i++)
					*(samples[chn] + phys_bps - 1 - i) = (res >> i * 8) & 0xff;
			} else {
				for (i = 0; i < bps; i++)
				{
					*(samples[chn] + i) = (res >> i * 8) & 0xff;
				}
			}
			samples[chn] += steps[chn];
		}
//		printf("chn: %d\n", chn);
		sample_time += step;
	}
	pc->current_time = sample_time;

}