
#ifndef _MARKOV_H_
#define _MARKOV_H_

struct markov_row
{
	int output_states;
	float * probabilities;
};

//uses ints to index all the states
struct markov_chart
{
	int input_states;
	int output_states;
	markov_row * rows;
};


int evaluate_chart(markov_chart * mc, int in_state);

markov_row get_markov_row(markov_chart * mc, int in_state);

int evaluate_row(markov_row * mr);

markov_row normalized(markov_row * mr);

void delete_row(markov_row * mr);

void read_file(markov_chart *mc, char * file_name);

#endif

