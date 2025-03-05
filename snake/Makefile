ifeq ($(strip $(GBA_LLVM)),)
	GBA_LLVM = ../gba-llvm-devkit-1-Linux-AArch64
endif

NAME = snake

BIN = $(GBA_LLVM)/bin
CC = $(BIN)/clang++
GBAFIX = $(BIN)/gbafix
CFLAGS = -O0 -mthumb --config armv4t-gba.cfg -Wl,-T,gba_cart.ld -I.

C_FILES = $(NAME).cc

$(NAME).gba: $(NAME).elf
	$(BIN)/llvm-objcopy -O binary $^ $@
	$(GBAFIX) $@

$(NAME).elf: $(C_FILES)
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

.PHONY: clean

clean:
	rm $(NAME).elf $(NAME).gba
