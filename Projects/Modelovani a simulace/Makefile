all: compile

run: compile spusti

compile : projekt-IMS.cc
	g++ -g -O2 -o projekt-IMS projekt-IMS.cc -lsimlib -lm

spusti:
##################################################
	./projekt-IMS 1 0 a
	./projekt-IMS 3 0 b
	./projekt-IMS 4 0 c
##################################################
	./projekt-IMS 1 1 2 6.5
	./projekt-IMS 3 1 2 6.5
	./projekt-IMS 4 1 2 6.5
##################################################
	./projekt-IMS 1 1 3 6.24
	./projekt-IMS 1 1 3 9.46
	./projekt-IMS 4 1 3 6.24

#all:
#	g++ -g -O2 -o projekt-IMS projekt-IMS.cc -lsimlib -lm


