

#include <stdlib.h>
#include <fstream>
#include "markov.h"

using namespace std;

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

	markov_row norm;
	norm.probabilities = (float *) malloc(sizeof(float) * mr->output_states);
	norm.output_states = mr->output_states;

	for(int i = 0; i < mr->output_states; i++)
	{
		norm.probabilities[i] = mr->probabilities[i] / total_probability;
	}
	return norm;
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
			delete_row(&norm);
			return i;
		}
	}
	delete_row(&norm);
	return -1;	//should not ever happen
}

markov_row get_markov_row(markov_chart * mc, int in_state)
{
	return mc->rows[in_state];
}


void delete_row(markov_row * mr)
{
	free(mr->probabilities);
}

void read_file(markov_chart *mc, char * file_name)
{
	ifstream file;
	string line;
	char *ptr;
	file.open(file_name);
	getline(file, line);
	if(line != "MARKOV")
	{
		printf("Error, not right format\n");
		return;
	}
	getline(file, line);
	int input_states = strtol(line.c_str(), &ptr, 10);
	int output_states = strtol(ptr+1, &ptr, 10);

	mc->rows = (markov_row *) malloc(sizeof(markov_row) * input_states);
	mc->input_states = input_states;
	mc->output_states = output_states;
	for(int i = 0; i < input_states; i++)
	{
		mc->rows[i].output_states = output_states;
		mc->rows[i].probabilities = (float *) malloc(sizeof(float) * output_states);
		getline(file, line);
		ptr = ((char *)line.c_str()) - 1;
		for(int j = 0; j < output_states; j++)
		{
			mc->rows[i].probabilities[j] = strtof(ptr + 1, &ptr);
		}
	}
}

