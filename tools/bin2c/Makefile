TARGET					:=	bin2c.exe


$(TARGET): $(wildcard *.c *.cpp)
	g++ $^ -O3 -o $@

clean:
	rm -fr $(TARGET)

.PHONY: clean
