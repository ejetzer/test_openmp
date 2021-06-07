%.o: %.cpp
	g++-11 -fopenmp -o $@ $^ 2>&1 | tee $*.log

%.txt: %.o %.lint
	./$^ 2>&1 | tee $@

%.lint: %.cpp
	python3.9 -m cpplint $*.cpp 2>&1 | tee $@

nettoyer:
	rm -rf *.o *.txt *.lint

.SECONDARY: %.o %.lint
