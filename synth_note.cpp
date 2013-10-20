

create_note(synth_note * sn, int note, int octave, float start_time)
{
	sn->note = note;
	sn->octave = octave;
	sn->start_time = start_time;

	//create sin waves
}

sample_note(synth_note * sn, float t)
{
	float sample = 0.0;
	for(int i = 0; i < sn->num_waves; i++)
	{
		sample = sample_wave( &(sn->waves[i]), t);
	}
	return sample;
}
