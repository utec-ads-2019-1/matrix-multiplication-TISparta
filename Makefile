run: FORCE
	g++ generate-matrix.cpp -o generate-matrix
	g++ -pthread multiplication.cpp -o multiplication -O2
	g++ multiplication-simple.cpp -o simple -O2

FORCE: ;

clean:
	rm generate-matrix
	rm multiplication
	rm simple
