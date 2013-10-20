

#include <stdlib.h>

int evaluate_chart(markov_chart * mc, int in_state)
{
	markov_row mr = mc->rows[in_state];
	return evaluate_row(&mr);
}


markov_row normalized(markov_row * mr)
{
	float total_probability = 0.0;
	for(int i = 0; i < mr->output_states; i++)
	{
		total_probability += mr->probabilities[i];
	}

	markov_row normalized;
	normalized.probabilities = malloc(sizeof(float) * mr->output_states);
	normalized.output_states = mr->output_states;

	for(int i = 0; i < mr->output_states; i++)
	{
		normalized.probabilities[i] = mr->probabilities[i] / total_probability;
	}
}

int evaluate_row(markov_row * mr)
{
	markov_row norm = normalized(mr);
	float random_sample = (rand() / (float)RAND_MAX);
	float probability_sum = 0.0;
	for(int i = 0; i < mr->output_states; i++)
	{
		probability_sum += norm.probabilities[i];
		if(random_sample <= probability_sum)
		{
			delete_row(norm);
			return i;
		}
	}
	delete_row(norm);
	return -1;
}

markov_row get_markov_row(markov_chart * mc, int in_state)
{
	return mc->rows[in_state];
}


void delete_row(markov_row * mr)
{
	free(mr->probabilities);
}

