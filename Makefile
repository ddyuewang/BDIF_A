all: scrub normal

scrub:
	mpic++ -std=c++11 -I include main.cpp -o scrub
normal:
	mpic++ -std=c++11 -I include main.cpp -o normal

clean:
	rm -rf *o scrub normal
