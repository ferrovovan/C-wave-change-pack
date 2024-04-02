CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -o2

SRCDIR = source
BINDIR = bin

boosted_wav: $(SRCDIR)/boosted_wav.c
	$(CC) $(CFLAGS) -o boosted_wav $(SRCDIR)/boosted_wav.c

concat_wav: $(SRCDIR)/concat_wav.c
	$(CC) $(CFLAGS) -o concat_wav $(SRCDIR)/concat_wav.c

mult_wav: $(SRCDIR)/mult_wav.c
	$(CC) $(CFLAGS) -o mult_wav $(SRCDIR)/mult_wav.c

slowed_wav: $(SRCDIR)/slowed_wav.c
	$(CC) $(CFLAGS) -o slowed_wav $(SRCDIR)/slowed_wav.c

wav_info: $(SRCDIR)/wav_info.c
	$(CC) $(CFLAGS) -o wav_info $(SRCDIR)/wav_info.c

cut_wav: $(SRCDIR)/cut.c
	$(CC) $(CFLAGS) -o cut_wav.out $(SRCDIR)/cut.c

1_hour_wav: $(SRCDIR)/1_hour_wav/1_hour_wav.c
	$(CC) $(CFLAGS) -o 1_hour_extender.out $(SRCDIR)/1_hour_wav/1_hour_wav.c

fan_1_hour_wav: $(SRCDIR)/1_hour_wav/1_hour_wav-fan.c
	$(CC) $(CFLAGS) -o fan_1_hour_extender.out $(SRCDIR)/1_hour_wav/1_hour_wav-fan.c

all: boosted_wav concat_wav mult_wav slowed_wav wav_info cut_wav 1_hour_wav fan_1_hour_wav

