

#ifndef _ENVELOPE_H_
#define _ENVELOPE_H_

struct envelope
{
	float attack;	//time from start to max
	float decay;	//half-life
};

void create_envelope(envelope * e, float attack, float decay);

float sample_envelope(envelope * e, float t);

#endif
