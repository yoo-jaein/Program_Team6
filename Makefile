level1game : level1game.o
	cc level1game.c set_ticker.c -lrt -lcurses -o level1game
level2game : level2game.o
	cc level2game.c set_ticker.c -lrt -lcurses -o level2game
level3game : level3game.o
	cc level3game.c set_ticker.c -lrt -lcurses -o level3game

project : project.o
	cc project.c played_game1.c played_game2.c played_game3.c coin_input.c -o project
