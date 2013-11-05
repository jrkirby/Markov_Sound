

#ifndef _NOTE_LIST_H_
#define _NOTE_LIST_H_

#include <glib.h>
#include "synth_note.h"

using namespace std;

struct note_list
{
	GArray * notes;
};

//allocates and makes a blank note list
void create_list(note_list * nl);

//frees note list and destroys all the notes in it
void destroy_list(note_list * nl);

//add a single note to the note list
void add_note(note_list * nl, synth_note note);

//moves all the notes that have started playing as of time t
//from to_play into playing
void move_started(note_list * playing, note_list * to_play, float t);

//moves all the notes that have finished playing as of time t
//from playing into done_playing
void move_finished(note_list * done_playing, note_list * playing, float t);

float sample_list(note_list * playing, float t);

#endif
