
install:
	clib install --dev

test:
	@$(CC) test.c $(CFLAGS) -I src -I deps -I deps/greatest $(LDFLAGS) -o $@
	@./$@

.PHONY: install test
