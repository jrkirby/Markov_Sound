OFILES= sin_wave.o synth_note.o frequencies.o markov.o envelope.o alsa_utils.o note_list.o play_context.o
COMPILER_OPTS= -Wall -lasound `pkg-config --cflags glib-2.0` -lglib-2.0

all: realtime_main

realtime_main: realtime_main.cpp $(OFILES)
	g++ realtime_main.cpp $(OFILES) $(COMPILER_OPTS) -o realtime_main

play_context.o: play_context.cpp play_context.h
	g++ -c play_context.cpp -o play_context.o $(COMPILER_OPTS)

note_list.o: note_list.cpp note_list.h
	g++ -c note_list.cpp -o note_list.o $(COMPILER_OPTS)

alsa_utils.o: alsa_utils.cpp alsa_utils.h
	g++ -c alsa_utils.cpp -o alsa_utils.o $(COMPILER_OPTS)

sin_wave.o: sin_wave.cpp sin_wave.h
	g++ -c sin_wave.cpp -o sin_wave.o $(COMPILER_OPTS)

synth_note.o: synth_note.cpp synth_note.h
	g++ -c synth_note.cpp -o synth_note.o $(COMPILER_OPTS)

frequencies.o: frequencies.cpp frequencies.h
	g++ -c frequencies.cpp -o frequencies.o $(COMPILER_OPTS)

envelope.o: envelope.cpp envelope.h
	g++ -c envelope.cpp -o envelope.o $(COMPILER_OPTS)

markov.o: markov.cpp markov.h
	g++ -c markov.cpp -o markov.o $(COMPILER_OPTS)

clean:
	rm -rf *o *gch main