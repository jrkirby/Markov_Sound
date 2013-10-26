OFILES= sin_wave.o synth_note.o frequencies.o markov.o envelope.o

all: alsa_main

alsa_main: alsa_main.cpp $(OFILES)
	g++ alsa_main.cpp $(OFILES) -o alsa_main -Wall -lasound

sin_wave.o: sin_wave.cpp sin_wave.h
	g++ -c sin_wave.cpp -o sin_wave.o -Wall

synth_note.o: synth_note.cpp synth_note.h
	g++ -c synth_note.cpp -o synth_note.o -Wall

frequencies.o: frequencies.cpp frequencies.h
	g++ -c frequencies.cpp -o frequencies.o -Wall

envelope.o: envelope.cpp envelope.h
	g++ -c envelope.cpp -o envelope.o -Wall

markov.o: markov.cpp markov.h
	g++ -c markov.cpp -o markov.o -Wall

clean:
	rm -rf *o *gch main