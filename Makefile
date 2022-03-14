.PHONY: all

IFLAGS = -Itrezor-crypto/ 
OBJS = bitcoin/transaction.o utils.o wallet.o slice.o bitcoin/signature.o ethereum/rlp.o ethereum/transaction.o

main: main.c trezor-crypto/libtrezor.a $(OBJS)
	$(CC) -o main main.c $(OBJS) $(IFLAGS) trezor-crypto/libtrezor.a

%.o: %.c
	$(CC) -c -o $@ $< $(IFLAGS)

tests/libtrezor-crypto.so:
	cd trezor-crypto
	make libtrezor.a

clean:
	rm -f main *.o bitcoin/*.o ethereum/*.o
