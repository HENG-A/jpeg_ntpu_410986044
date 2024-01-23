CC = gcc
CFLAGS = -Wall

all: encoder decoder

encoder: encoder.c
	$(CC) $(CFLAGS) -o $@ $^

decoder: decoder.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f encoder decoder

demo1a: all
	./encoder 1 KimberlyNCat.bmp Qt_Y.txt Qt_Cb.txt Qt_Cr.txt dim.txt qF_Y.raw qF_Cb.raw qF_Cr.raw eF_Y.raw eF_Cb.raw eF_Cr.raw
	./decoder 1 QResKimberlyNCat.bmp KimberlyNCat.bmp Qt_Y.txt Qt_Cb.txt Qt_Cr.txt dim.txt qF_Y.raw qF_Cb.raw qF_Cr.raw

demo1b: all
	./encoder 1 KimberlyNCat.bmp Qt_Y.txt Qt_Cb.txt Qt_Cr.txt dim.txt qF_Y.raw qF_Cb.raw qF_Cr.raw eF_Y.raw eF_Cb.raw eF_Cr.raw
	./decoder 1 ResKimberlyNCat.bmp Qt_Y.txt Qt_Cb.txt Qt_Cr.txt dim.txt qF_Y.raw qF_Cb.raw qF_Cr.raw eF_Y.raw eF_Cb.raw eF_Cr.raw
	cmp KimberlyNCat.bmp ResKimberlyNCat.bmp

demo0: all
	./encoder 0 KimberlyNCat.bmp R.txt G.txt B.txt dim.txt
	./decoder 0 ResKimberlyNCat.bmp R.txt G.txt B.txt dim.txt
	cmp KimberlyNCat.bmp ResKimberlyNCat.bmp


