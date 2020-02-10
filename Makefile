CC=gcc 
VPATH = applications:user:drivers:Tools
SUBDIR=$(shell ls -d */)
ELF = NanoPiMain
ROOTSRC=$(wildcard *.c)
ROOTOBJ=$(ROOTSRC:%.c=%.o)
SUBSRC=$(shell find $(SUBDIR) -name '*.c')
SUBOBJ=$(SUBSRC:%.c=%.o)

LDFLAGS = -lwiringPi -lpthread -lm


$(ELF):$(ROOTOBJ) $(SUBOBJ)
	$(CC) -g -o $(ELF) $(ROOTOBJ) $(SUBOBJ) $(LDFLAGS)
.c.o:
	echo $(@D) $(<F)  $(@F)
	$(CC) -c $(@D)/$(<F) -g -o $(@D)/$(@F)
clean:
	rm $(ELF) $(ROOTOBJ) $(SUBOBJ)

