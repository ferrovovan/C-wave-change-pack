CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -o2

SRCDIR = source
BINDIR = build

SINGLE_TARGETS := boosted_wav concat_wav mult_wav slowed_wav wav_info cut_wav  fan_1_hour_wav two_channels_wav

define compile_target
$1:
	$(CC) $(CFLAGS) -o $(BINDIR)/$1.out $(SRCDIR)/$1.c
endef

# задание целей для одиночных файлов
$(foreach target,$(SINGLE_TARGETS),$(eval $(call compile_target,$(target))))

1_hour_wav:
	$(CC) $(CFLAGS) -o $(BINDIR)/1_hour_extender.out $(SRCDIR)/1_hour_wav/1_hour_wav.c
zero_wav:
	$(CC) $(CFLAGS) -o $(BINDIR)/zero_wav.out $(SRCDIR)/null_wav.c

all: $(SINGLE_TARGETS), 1_hour_wav

clear:
	rm -rf $(BINDIR)/*
