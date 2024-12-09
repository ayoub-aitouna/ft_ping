CFG ?= debug

CC = gcc
CXX = g++
LD = $(CC)
STRIP = strip

TARGET = ft_ping

SRC = $(wildcard src/*.c src/**/*.c)
	
CFLAGS = -MMD -MP

LDFLAGS = 


ifeq ($(CFG), debug)
CFLAGS += -DDEBUG
else
CFLAGS += -O3 -DNDEBUG
endif

BUILD_DIR = build/$(CFG)
OBJS_DIR = $(BUILD_DIR)/objs

OBJS=$(patsubst %.c, $(OBJS_DIR)/%.o, ${SRC})
DEPS=$(patsubst %.c, $(OBJS_DIR)/%.d, ${SRC})

.PHONY: all
all: ${BUILD_DIR}/${TARGET}

.PHONY: inform
inform:
ifneq ($(CFG),release)
ifneq ($(CFG),debug)
	@echo "┌---------------------------------------------------------------------┐"
	@echo "|                                                                     │" 
	@echo "| invalide configuration "$(CFG)" specified.                            │"
	@echo "| Possible choised for configuration are 'CFG=release' and 'CFG=debug'│"
	@echo "|                                                                     │"
	@echo "└---------------------------------------------------------------------┘"
	@exit 1
endif
endif
	@echo "┌---------------------------------------------┐"
	@echo "|configuration "$(CFG)"                          │"
	@echo "└---------------------------------------------┘"

${BUILD_DIR}/${TARGET}: ${OBJS} | inform
	@mkdir -p ${dir $@}
	@$(LD) $(LDFLAGS) -o $@.debug $(OBJS)
	@$(STRIP) $@.debug -o $@
	@echo
	@echo "To start the program, run:"
	@echo "   $@"

$(OBJS_DIR)/%.o: %.c | inform
	@mkdir -p ${dir $@}
	@$(CC) -c $(CFLAGS) -o $@ $<
	


.PHONY: clean
clean:
	@rm $(OBJS) $(DEPS) ${BUILD_DIR}/${TARGET} ${BUILD_DIR}/${TARGET}.debug

.PHONY: fclean
fclean :
	rm -rf  build

.PHONY: re
re : fclean  ${BUILD_DIR}/${TARGET}


.PHONY: help
help:
	@echo "Usage: make [target] [CFG=debug|release]"
	@echo "Targets:"
	@echo "   all      Build the project (default target)"
	@echo "   clean    Remove object files and intermediate build files"
	@echo "   fclean   Remove the build directory"
	@echo "   re       Clean and rebuild"
	@echo "   help     Show this help message"
	
-include $(DEPS)