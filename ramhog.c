#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
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
  program_options.size = 65556ULL * 65556;
  program_options.touch_interval = 1000000;
  program_options.should_touch = 1;
  program_options.touch_randomly = 1;
  uint64_t result = run(&program_options);
  // avoid dead code elimination
  if (result == UINT64_MAX) {
    printf("This should probably never happen.");
  }
  return 0;
}

