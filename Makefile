#
# simple makefile!
#

TARGET := pdp8
CSRC := $(shell find . -iname '*.c')
LIBS := sdl
CFLAGS := -std=gnu99 -O2 -pipe -Wall -Wextra -Werror -g
LDFLAGS := -lm

include common.mk
