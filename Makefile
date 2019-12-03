all: kc

kc: $(wildcard koberi-c/*)
	g++ -o kc koberi-c/*.cpp

clean:
	rm -f kc
