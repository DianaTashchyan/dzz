G = g++
FLAGS = -Wall -g

init: init.cpp
	$(G) $(FLAGS) init.cpp -o init
set: set.cpp
	$(G) $(FLAGS) set.cpp -o set
check: check.cpp
	$(G) $(FLAGS) check.cpp -o check
destroy: destroy.cpp
	$(G) $(FLAGS) destroy.cpp -o destroy
myprog: init, set, check, destroy
	$(G) $(FLAGS) init set check -o SOE
clean:
	rm -f init, set, check, destroy SOE

