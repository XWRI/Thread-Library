# Target library
lib := libuthread.a
objs := queue.o context.o uthread.o preempt.o
#executable :=
CC := gcc
CFLAGS	:= -Wall -Werror
CFLAGS	+= -g

# Generate dependencies
DEPFLAGS = -MMD -MF $(@:.o=.d)

ifneq ($(V),1)
Q = @
endif

#all: $(lib)

## TODO: Phase 1.1

# Generic rule for compiling objects
%.o: %.c
		@echo "CC	$@"
		$(Q)$(CC) $(CFLAGS) -c -o $@ $< $(DEPFLAGS)

$(lib): $(objs)
		ar rcs $(lib) $(objs)

# Cleaning rule
clean:
		@echo "CLEAN"
		$(Q)rm -rf $(objs) $(lib) *.d
