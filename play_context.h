

#ifndef _PLAY_CONTEXT_H_
#define _PLAY_CONTEXT_H_

#include "note_list.h"
#include "alsa_utils.h"
#include "markov.h"

#include </usr/include/alsa/asoundlib.h>

struct play_context
{
	float current_time;
	note_list playing_notes;
	note_list future_notes;
	note_list past_notes;

	int last_note;

	float gen_time;
	float beat_length;
};

void create_context(play_context * pc);

void gen_next_notes(play_context * pc);

//returns the amount of time that passed in playing it
void play_audio(const snd_pcm_channel_area_t *areas, play_context * pc, alsa_params_t * alsaparams, markov_chart * music);


#endif
