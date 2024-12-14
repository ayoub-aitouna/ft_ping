CFG ?= debug

CC = gcc
CXX = g++
LD = $(CC)
STRIP = strip

TARGET_NAME = ft_ping

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
TARGET = ${BUILD_DIR}/${TARGET_NAME}
OBJS=$(patsubst %.c, $(OBJS_DIR)/%.o, ${SRC})
DEPS=$(patsubst %.c, $(OBJS_DIR)/%.d, ${SRC})

.PHONY: all
all: ${TARGET}

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
	@echo "---------------------------------------------------------------------┘"
	@echo ""
${TARGET}: ${OBJS} | inform
	@mkdir -p ${dir $@}
	@$(LD) $(LDFLAGS) -o $@.debug $(OBJS)
	@$(STRIP) $@.debug -o $@
	@ln -fs ${TARGET} ${TARGET_NAME} 

$(OBJS_DIR)/%.o: %.c | inform
	@mkdir -p ${dir $@}
	@$(CC) -c $(CFLAGS) -o $@ $<
	


.PHONY: clean
clean:
	@rm -rf $(OBJS) $(DEPS) ${TARGET} ${TARGET}.debug   ${TARGET_NAME}
	@echo "deleted [Objects|Dependency files] & ${TARGET}"

.PHONY: fclean
fclean: clean
	@rm -rf  build
	@echo "deleted build directory"

.PHONY: re
re : fclean  ${TARGET}



.PHONY: spinup
spinup:
	@echo "Staring Docker Container" 
	@docker build -t ft_ping .
	@docker run -v $${PWD}:/app -it ft_ping zsh

.PHONY: help
help:
	@echo "Usage: make [target] [CFG=debug|release]"
	@echo "Targets:"
	@echo "   all      Build the project (default target)"
	@echo "   clean    Remove object files and intermediate build files"
	@echo "   fclean   Remove the build directory"
	@echo "   re       Clean and rebuild"
	@echo "   spinup   create a container and launch it for testing"
	@echo "   help     Show this help message"
	
-include $(DEPS)