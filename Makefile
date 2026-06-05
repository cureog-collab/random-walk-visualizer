build:
	clang main.c agent.c -o random_walk -lSDL2 -lSDL2_ttf -lm -Werror -Wall