CFG ?= debug

CC = gcc
CXX = g++
LD = $(CC)
STRIP = strip


SRC = $(wildcard src/*.c src/**/*.c)
	
CFLAGS = -MMD -MP

LDFLAGS = 

TARGET_NAME = ft_ping

ARGS_0 = -c 20 google.com
ARGS_1 = -c 40 -f google.com
ARGS_2 = -i 0.003 -c 300 -l 2 -n  google.com
ARGS_3 = -h

ifeq ($(CFG), debug)
CFLAGS += -DDEBUG
else
CFLAGS += -O3 -DNDEBUG
endif

BUILD_DIR = build/$(CFG)
BIN_DIR = bin/$(CFG)
LOG_DIR = logs/$(CFG)

OBJS_DIR = $(BUILD_DIR)/objs
TARGET = ${BIN_DIR}/${TARGET_NAME}
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
	mkdir -p ${dir $@}
	$(LD) $(LDFLAGS) -o $@.debug $(OBJS)
	$(STRIP) $@.debug -o $@
	@ln -fs ${TARGET} ${TARGET_NAME} 

$(OBJS_DIR)/%.o: %.c | inform
	@mkdir -p ${dir $@}
	@$(CC) -c $(CFLAGS) -o $@ $<

.PHONY: clean
clean:
	@rm -rf $(OBJS) $(DEPS) ${TARGET} ${TARGET_NAME} ${LOG_DIR}/* 
	@echo "deleted [Objects|Dependency files] & ${TARGET}"

.PHONY: fclean
fclean: clean
	@rm -rf  build bin logs
	@echo "deleted build directory"

.PHONY: re
re : fclean  ${TARGET}

.Phony: install
install: ${TARGET}
	@cp ${TARGET} /usr/local/bin/${TARGET_NAME}
	@cp man/${TARGET_NAME}.8 /usr/share/man/man8/
	@echo "Installed ${TARGET_NAME} to /usr/local/bin"

.PHONY: spinup
spinup:
	@echo "Staring Docker Container" 
	@docker build -t ft_ping .
	@docker run -v $${PWD}:/app -it ft_ping zsh

.PHONY: check
check:  ${TARGET}
	@mkdir -p ${LOG_DIR}
	@echo "Checking for memory leaks"
	@sudo valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=${LOG_DIR}/valgrind-out-0.txt ${TARGET}.debug ${ARGS_0}
	@sudo valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=${LOG_DIR}/valgrind-out-1.txt ${TARGET}.debug ${ARGS_1}
	@sudo valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=${LOG_DIR}/valgrind-out-2.txt ${TARGET}.debug ${ARGS_2}
	@sudo valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=${LOG_DIR}/valgrind-out-3.txt ${TARGET}.debug ${ARGS_3}
	@echo "Memory leaks check done"
	@echo "open logs folder to view the results"


.PHONY: help
help:
	@echo "Usage: make [target] [CFG=debug|release]"
	@echo "Targets:"
	@echo "   all      Build the project (default target)"
	@echo "   clean    Remove object files and intermediate build files"
	@echo "   fclean   Remove the build directory"
	@echo "   re       Clean and rebuild"
	@echo "   install  Installs the ${TARGET_NAME} binary"
	@echo "   spinup   create a container and launch it for testing"
	@echo "   help     Show this help message"
	
-include $(DEPS)