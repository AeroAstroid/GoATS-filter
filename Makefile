ifeq (run,$(firstword $(MAKECMDGOALS)))
  ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(ARGS):;@:)
endif

PYTHON := python3

L_CUBIOMES := $(filter-out ./submodules/cubiomes/tests.c,$(wildcard ./submodules/cubiomes/*.c))
L_SFMT := ./submodules/sfmt/SFMT.c

compile:
	gcc main.c ./filters/*.c ./logic/*.c ./util/*.c $(L_CUBIOMES) $(L_SFMT) -lm -pthread -Ofast -DSFMT_MEXP=19937 -g -mavx -o seed

py:
	$(PYTHON) config.py

pyn:
	$(PYTHON) config.py noname

run:
	./seed $(ARGS)

lavachecker:
	java -jar jar/lava_checker.jar