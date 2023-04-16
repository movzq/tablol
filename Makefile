compiler = gcc

main: lexer.o tablol.o ustring/ustring.o parser.o yard-algo/yard-algo.o
	$(compiler) -o tblol lexer.o tablol.o ustring/ustring.o parser.o yard-algo/yard-algo.o
	./tblol sheets/a* | column -t -s '|'

update_ustr:
	cp ../ustring/ust* ./ustring

update_yard:
	cp ../yard-algo/yar* ./yard-algo

rm:
	rm *.o tblol ustring/*.o yard-algo/*.o
