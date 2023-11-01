ifeq (run,$(firstword $(MAKECMDGOALS)))
  ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(ARGS):;@:)
endif

L_CUBIOMES := $(filter-out ./libs/cubiomes/tests.c,$(wildcard ./libs/cubiomes/*.c))

compile:
	gcc main.c ./module/*.c ./filters/*.c ./logic/*.c $(L_CUBIOMES) -I/include -L./libs -lgcrypt -lgpg-error -lm -pthread -Ofast -g -march=native -o seed

py:
	python config.py

pyn:
	python config.py noname

run:
	./seed $(ARGS)

lavachecker:
	java -jar jar/lava_checker.jar