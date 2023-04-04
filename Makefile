compiler = gcc

main: lexer.o tablol.o ustring/ustring.o parser.o;
	$(compiler) -o tblol lexer.o tablol.o ustring/ustring.o parser.o
	./tblol sheets/*

update_ustr:
	cp ../ustring/ust* ./ustring

rm:
	rm *.o tblol ustring/*.o
