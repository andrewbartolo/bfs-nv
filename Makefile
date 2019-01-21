CCFLAGS=-Wall -Werror
CSTD=c11
LDFLAGS=-lpthread
SRCFILES=$(wildcard *.c)
BUILDDIR=build
# Every file in OBJFILES has the directory prepended to it
OBJFILES=$(SRCFILES:%.cpp=$(BUILDDIR)/%.o)
BINNAME=bfs

.PHONY: clean run

all: $(BINNAME)
$(BINNAME): $(BUILDDIR) $(OBJFILES)
	$(CC) $(CCFLAGS) -std=$(CSTD) -o $(BUILDDIR)/$(BINNAME) $(LDFLAGS) $(OBJFILES)
	ln -sf $(BUILDDIR)/$(BINNAME) $(BINNAME)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# TODO unify concept of below pattern rule and $(OBJFILES)
# TODO do .h check too (this currently fails for main.cpp, since it has no .h)
$(BUILDDIR)/%.o: %.c
	$(CC) $(CCFLAGS) -std=$(CSTD) -c -o $@ $<


run:
	$(BUILDDIR)/$(BINNAME)

clean:
	rm -rf $(BUILDDIR) $(BINNAME)
