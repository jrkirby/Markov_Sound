OFILES= sin_wave.o

all: alsa_main

alsa_main: alsa_main.cpp $(OFILES)
	g++ alsa_main.cpp $(OFILES) -o main -Wall -lasound

sin_wave: sin_wave.cpp sin_wave.h
	g++ -c sin_wave.cpp -o sin_wave.o -Wall

clean:
	rm -rf *o *gch main