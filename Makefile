CC = gcc
CFLAGS = -Wall

all: encoder decoder

encoder: encoder.c
	$(CC) $(CFLAGS) -o $@ $^

decoder: decoder.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f encoder decoder

demo0: all
	./encoder 0 KimberlyNCat.bmp R.txt G.txt B.txt dim.txt
	./decoder 0 ResKimberlyNCat.bmp R.txt G.txt B.txt dim.txt
	cmp KimberlyNCat.bmp ResKimberlyNCat.bmp
