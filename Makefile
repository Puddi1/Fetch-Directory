run:
	@mkdir -p bin && rm -f ./bin/main && clang -o ./bin/main main.c && ./bin/main

meml:
	@mkdir -p bin && rm -f ./bin/main && clang -o ./bin/main main.c && leaks --atExit -- ./bin/main
