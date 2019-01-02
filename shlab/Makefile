# Makefile for the CS:APP Shell Lab

TEAM = NOBODY
VERSION = 1
HANDINDIR = /afs/cs/academic/class/15213-f02/L5/handin
DRIVER = ./sdriver.pl
TSH = ./tsh
TSHREF = ./tshref
TSHARGS = "-p"
CC = gcc
CFLAGS = -Wall -O2
FILES = $(TSH) ./myspin ./mysplit ./mystop ./myint
TEST_CASES = 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16
TESTS = $(foreach t, $(TEST_CASES), test$(t))
RTESTS = $(foreach t, $(TEST_CASES), rtest$(t))
CMPS = $(foreach t, $(TEST_CASES), cmp$(t))

all: $(FILES)

##################
# Handin your work
##################
handin:
	cp tsh.c $(HANDINDIR)/$(TEAM)-$(VERSION)-tsh.c


##################
# Regression tests
##################

# Run tests using the student's shell program

$(TESTS): test%: trace%.txt $(TSH)
	$(DRIVER) -t $< -s $(TSH) -a $(TSHARGS)

# Run the tests using the reference shell program

$(RTESTS): rtest%: trace%.txt $(TSHREF)
	$(DRIVER) -t $< -s $(TSHREF) -a $(TSHARGS)

$(CMPS): cmp%: trace%.txt $(TSH) $(TSHREF)
	@echo $@
	@$(DRIVER) -t $< -s $(TSH) -a $(TSHARGS) | sed \
		-e 's/([0-9]\+)/()/g' \
	        -e 's/^ \+[0-9]\+//g' \
	       	-e 's/tshref/tsh/g' > .tsh.out
	@$(DRIVER) -t $< -s $(TSHREF) -a $(TSHARGS) | sed \
		-e 's/([0-9]\+)/()/g' \
	        -e 's/^ \+[0-9]\+//g' \
	       	-e 's/tshref/tsh/g' > .tshref.out
	@diff .tsh.out .tshref.out

cmps: $(foreach c, $(CMPS), $(c))

# clean up
clean:
	rm -f $(FILES) *.o *~
	rm -f .tsh.out .tshref.out

