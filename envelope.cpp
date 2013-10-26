
#include "envelope.h"
#include <math.h>
#include <stdio.h>
#define EULERS 2.71828

void create_envelope(envelope * e, float attack, float decay)
{
	e->attack = attack;
	e->decay = decay;
}

float sample_envelope(envelope * e, float t)
{
	float value;
	if(t < e->attack)
	{
		value = t / e->attack;
	}
	else
	{
		//value = 1.0 * pow(EULERS, -1.0 * pow(2, t - e->attack) / pow(2, e->decay));
		value = 1.0;
	}
	//printf("envelope value: %f,", value);
	return value;
}