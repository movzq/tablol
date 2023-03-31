compiler = gcc

main: lexer.o tablol.o ustring/ustring.o;
	$(compiler) -o tblol lexer.o tablol.o ustring/ustring.o
	./tblol tables/simple
	rm *.o tblol ustring/ustring.o

update_ustr:
	cp ../ustring/ust* ./ustring
