CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -o2

SRCDIR = source
BINDIR = build

# Правила компиляции для каждой цели
#boosted_wav:
#	$(CC) $(CFLAGS) -o $(BINDIR)/boosted_wav.out $(SRCDIR)/boosted_wav.c

concat_wav:
	$(CC) $(CFLAGS) -o $(BINDIR)/concat_wav.out $(SRCDIR)/concat_wav.c

mult_wav:
	$(CC) $(CFLAGS) -o $(BINDIR)/mult_wav.out $(SRCDIR)/mult_wav.c

#slowed_wav:
#	$(CC) $(CFLAGS) -o $(BINDIR)/slowed_wav.out $(SRCDIR)/slowed_wav.c

wav_info:
	$(CC) $(CFLAGS) -o $(BINDIR)/wav_info.out $(SRCDIR)/wav_info.c

cut_wav:
	$(CC) $(CFLAGS) -o $(BINDIR)/cut_wav.out $(SRCDIR)/cut_wav.c

fan_1_hour_wav:
	$(CC) $(CFLAGS) -o $(BINDIR)/fan_1_hour_wav.out $(SRCDIR)/fan_1_hour_wav.c

two_channels_wav:
	$(CC) $(CFLAGS) -o $(BINDIR)/two_channels_wav.out $(SRCDIR)/two_channels_wav.c


SINGLE_TARGETS := mult_wav concat_wav wav_info cut_wav  fan_1_hour_wav two_channels_wav zero_wav null_wav
# boosted_wav slowed_wav


1_hour_wav:
	$(CC) $(CFLAGS) -o $(BINDIR)/1_hour_extender.out $(SRCDIR)/1_hour_wav/1_hour_wav.c

zero_wav:
	$(CC) $(CFLAGS) -o $(BINDIR)/zero_wav.out $(SRCDIR)/null_wav.c

null_wav:
	$(CC) $(CFLAGS) -o $(BINDIR)/zero_wav.out $(SRCDIR)/null_wav.c

all: $(SINGLE_TARGETS) 1_hour_wav

clear:
	rm -rf $(BINDIR)/*
