

struct synth_note
{
	int num_waves;
	sin_wave * waves;
	int note;
	int octave;
	float start_time;
};

create_note(synth_note * sn, int note, int octave, float start_time);

sample_note(synth_note * sn, float t);

