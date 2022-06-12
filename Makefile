
# vim: ft=make noexpandtab

BINDIR := bin
OBJDIR := obj

OBJECTS := $(OBJDIR)/src/mbeep.o

MD := mkdir -v -p

CFLAGS := -Wall -Werror -fpic -DMBEEP_ALSA

LDFLAGS := -lasound

all: $(BINDIR)/static/libmbeep.a $(BINDIR)/shared/libmbeep.so mbeep

mbeep: $(OBJDIR)/src/main.o | $(BINDIR)/static/libmbeep.a
	$(CC) -o $@ $^ -L$(BINDIR)/static -lmbeep $(LDFLAGS)

$(BINDIR)/static/libmbeep.a: $(OBJECTS)
	$(MD) $(dir $@)
	$(AR) rcs $@ $^

$(BINDIR)/shared/libmbeep.so: $(OBJECTS)
	$(MD) $(dir $@)
	$(CC) -shared -o $@ $^

$(OBJDIR)/%.o: %.c
	$(MD) $(dir $@)
	$(CC) -c -o $@ $(CFLAGS) $<

.PHONY: clean

clean:
	rm -rf $(OBJDIR); \
	rm -rf $(BINDIR); \
	rm -f mbeep_alsa

