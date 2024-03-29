# You can use clang if you prefer
CC = gcc

# Feel free to add other C flags
CFLAGS += -c -std=c99 -Wall -Werror -Wextra -O2 -D_GNU_SOURCE
# By default, we colorize the output, but this might be ugly in log files, so feel free to remove the following line.
CFLAGS += -D_COLOR

# You may want to add something here
LDFLAGS += -lz -std=c99

#supp CFILE
CFILE += src/create_socket.c src/selective_repeat.c src/handle_message.c src/packet_implem.c

# Adapt these as you want to fit with your project
SENDER_SOURCES = $(wildcard src/sender.c src/log.c)
RECEIVER_SOURCES = $(wildcard src/receiver.c src/log.c)

SENDER_OBJECTS = $(SENDER_SOURCES:.c=.o)
RECEIVER_OBJECTS = $(RECEIVER_SOURCES:.c=.o)

SENDER = sender
RECEIVER = receiver

all: $(SENDER) $(RECEIVER)

$(SENDER): $(SENDER_OBJECTS)
	$(CC) $(SENDER_OBJECTS) $(CFILE) $(LDFLAGS) -o $@

$(RECEIVER): $(RECEIVER_OBJECTS)
	$(CC) $(RECEIVER_OBJECTS) $(CFILE) $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< $(LDFLAGS) -o $@

.PHONY: clean mrproper

clean:
	@rm -f $(SENDER_OBJECTS) $(RECEIVER_OBJECTS)
	@rm -f $(SENDER).log $(RECEIVER).log

mrproper:
	@rm -f $(SENDER) $(RECEIVER)

# It is likely that you will need to update this
tests: all
	@cd tests && ./run_tests.sh

link :
	@cd link_simulator && make

# By default, logs are disabled. But you can enable them with the debug target.
debug: CFLAGS += -D_DEBUG
debug: clean all

csv:
	@cd tests && ./make_csv.sh
	@git add -A
	@git commit -m "CSV upload"
	@git push

# Place the zip in the parent repository of the project
ZIP_NAME="../projet1_Mounzer_Aigret.zip"

# A zip target, to help you have a proper zip file. You probably need to adapt this code.
zip: clean
	# Generate the log file stat now. Try to keep the repository clean.
	@rm $(ZIP_NAME)
	@git log --stat > gitlog.stat
	@zip -r $(ZIP_NAME) Makefile src tests rapport.pdf gitlog.stat
	# We remove it now, but you can leave it if you want.
	@rm gitlog.stat

comp:
	@gcc src/sender.c src/create_socket.c src/packet_implem.c src/selective_repeat.c src/handle_message.c -lz -o src/sender
	@gcc src/receiver.c src/create_socket.c src/packet_implem.c src/selective_repeat.c src/handle_message.c -lz -o src/receiver

run:
	@./src/receiver -s src/stats_receiver.csv :: 12345 1>src/receive_file 2>log_rec.txt & ./src/sender ::1 12345 -f src/test.txt -s src/stats_sender.csv 1>src/sender_file 2>log_sen.txt

glob:
	@ valgrind --leak-check=full ./receiver -s stats_receiver.csv :: 12345 1>receive_file 2>log_rec.txt & valgrind --leak-check=full  ./sender ::1 12345 -f src/test.txt -s stats_sender.csv 1>sender_file 2>log_sen.txt
