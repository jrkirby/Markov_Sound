
#include "note_list.h"
#include "synth_note.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

void create_list(note_list * nl)
{
	nl->notes = g_array_new(0, 0, sizeof(synth_note));
}

void destroy_list(note_list * nl)
{
	g_array_free(nl->notes, 1);
	free(nl);
}

void add_note(note_list * nl, synth_note note)
{
	g_array_append_val(nl->notes, note);
}

void move_started(note_list * playing, note_list * to_play, float t)
{
	GArray * temp_array = g_array_new(0, 0, sizeof(synth_note));
	synth_note temp;
	for(uint i = 0; i < to_play->notes->len; i++)
	{
		temp = g_array_index(to_play->notes, synth_note, i);
		if( temp.start_time < t)
		{
			g_array_append_val(temp_array, temp);
			g_array_remove_index(to_play->notes, i);
			i--;
		}
	}

	g_array_append_vals(playing->notes, temp_array->data, temp_array->len);

}

void move_finished(note_list * done_playing, note_list * playing, float t)
{
	GArray * temp_array = g_array_new(0, 0, sizeof(synth_note));
	synth_note temp;
	for(uint i = 0; i < playing->notes->len; i++)
	{
		temp = g_array_index(playing->notes, synth_note, i);
		if( t > temp.end_time)
		{
			printf("start: %f, current: %f, and end: %f\n", temp.start_time, t, temp.end_time);
			g_array_append_val(temp_array, temp);
			g_array_remove_index(playing->notes, i);
			i--;
		}
	}

	g_array_append_vals(done_playing->notes, temp_array->data, temp_array->len);

}

float sample_list(note_list * playing, float t)
{

	float value = 0.0;
	for(uint i = 0; i < playing->notes->len; i++)
	{
		value += sample_note( &g_array_index(playing->notes, synth_note, i), t);
	}
	//printf("value: %f\n", value);
	return value;
}

