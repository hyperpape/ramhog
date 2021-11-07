#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

uint64_t nonce = 14192476102350395398ULL; // chosen randomly

struct options {
  size_t size;
  long touch_interval;
  _Bool should_touch;
  _Bool touch_randomly;
};

uint64_t next_random() {
  nonce = 1664525 * nonce + 1013904223; // YOLO from Stack Exchange
  return nonce;
}

int parse_options(struct options *opt, int argc, char **argv) {
  size_t size;
  long touch_interval = 0;
  _Bool should_touch = 0;
  _Bool touch_randomly = 0;
  if (argc < 1) {
    return -1;
  }

  for (int i = 0;; i++) {
    char c = argv[1][i];
    if (c < '0' || c > '9') {
      if (c == 'K' || c == 'M' || c == 'G') {
	errno = 0;
	uint64_t passed_size = strtoull(argv[1], NULL, 10);
	if (errno) {
	  fprintf(stderr, "Parse error for memory size %s. Aborting.\n", argv[1]);
	  return errno;
	}
	if (c == 'K') {
	  passed_size *= 1024;
	}
	else if (c == 'M') {
	  passed_size *= 1024ULL * 1024ULL;
	}
	else if (c == 'G') {
	  passed_size *= 1024ULL * 1024ULL * 1024ULL;
	}
	if (passed_size > SIZE_MAX) {
	  fprintf(stderr, "Size %lu too large", passed_size);
	  return -1;
	}
	size = passed_size;
	break;
      }
      else {
	fprintf(stderr, "Illegal first argument: %s. Aborting.\n", argv[1]);
	return -1;
      }
    }
  }
  for (int i = 2; i < argc; i++) {
    if (!strncmp(argv[i], "--touch", 7)) {
      should_touch = 1;
    }
    else if (!strncmp(argv[i], "--random", 8)) {
      touch_randomly = 1;
    }
    else if (!strncmp(argv[i], "--sleep=", 8)) {
      char *tail = &argv[i][8];
      errno = 0;
      touch_interval = strtoull(tail, NULL, 10);
      if (errno) {
	fprintf(stderr, "Invalid sleep option %s. Aborting.\n", argv[i]);
	return errno;
      }
    }
    else {
      fprintf(stderr, "Unrecognized option %s. Aborting", argv[i]);
    }
  }

  opt->size = size;
  opt->touch_interval = touch_interval;
  opt->should_touch = should_touch;
  opt->touch_randomly = touch_randomly;

  return 0;
}

uint64_t* alloc_and_fill_memory(uint64_t size) {
  void* ptr = malloc(size);
  if (ptr == NULL) {
    exit(1);
  }
  uint64_t* memory = (uint64_t*) ptr;
  for (size_t i = 0; i < size / 8; i++) {
    memory[i] = next_random();
  }
  return memory;
}

uint64_t run(struct options *program_options) {
  uint64_t counter = 0;
  uint64_t *memory = alloc_and_fill_memory(program_options -> size);
  if (program_options -> should_touch) {
    for (;;) {
      if (program_options -> touch_interval > 0) {
	long tv_sec = program_options->touch_interval / 1000000000;
	long tv_nsec = program_options->touch_interval % 1000000000;
	const struct timespec ts = {
	  .tv_sec = tv_sec,
	  .tv_nsec = tv_nsec };
	struct timespec ts2;
	int ret;
	do {
	  ret = nanosleep(&ts, &ts2);
	} while (ret && errno == EINTR);
      }
      for (size_t i = 0; i < program_options -> size / 8; i++) {
	uint64_t value;
	if (program_options -> touch_randomly) {
	  value = memory[next_random() % (program_options -> size / 8)];
	}
	else {
	  value = memory[i];
	}
	// avoid dead code elimination
	if (value == nonce) {
	  counter++;
	  if (counter == nonce) {
	    return counter;
	  }
	}
      }
    }
  }
  return 0;
}

int main(int argc, char **argv) {
  struct options program_options;
  int parsed = parse_options(&program_options, argc, argv);

  if (parsed == 0) {
    uint64_t result = run(&program_options);
    if (result == 5) {
      printf("Surprisingly a five!\n"); // TODO: fix
    }
  }
  return parsed;
}
