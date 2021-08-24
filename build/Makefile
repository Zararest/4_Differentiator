
obj_main = main.o Knots.o Tree.o
obj_test = test.o Knots.o Tree.o

Knots.o: ../src/Knots.cpp
	g++ -c ../src/Knots.cpp

Tree.o: ../src/Tree.cpp
	g++ -c ../src/Tree.cpp

main.o: ../main.cpp
	g++ -c ../main.cpp

test.o: ../test.cpp
	g++ -c ../test.cpp


main: $(obj_main)
	g++ -o main $(obj_main)

test: $(obj_test)
	g++ -o test $(obj_test)

only_test: test.o
	g++ -o only_test test.o


.PHONY : clean
clean:
	rm -rf *.o main
	rm -rf *.o test
