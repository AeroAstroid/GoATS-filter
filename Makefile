ifeq (run,$(firstword $(MAKECMDGOALS)))
  ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(ARGS):;@:)
endif

compile:
	gcc main.c ./module/*.c ./filters/*.c ./logic/*.c -I/include -L./libs -lgcrypt -lgpg-error -lcubiomes -lm -pthread -Ofast -g -march=native -o seed

py:
	python config.py

pyn:
	python config.py noname

run:
	./seed $(ARGS)

lavachecker:
	java -jar jar/lava_checker.jar