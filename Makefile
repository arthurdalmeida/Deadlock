CC = cc
CFLAGS = -Wall

banker: banker.c
	$(CC) $(CFLAGS) -o banker banker.c

clean:
	rm -f banker

.PHONY: clean