// http://github.com/jeffwalter/defer
//
// Copyright <c) 2016 Jeff Walter <jeff@404ster.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.



#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include "../include/defer.h"

void readClose () {
	char failed;
	int fd;
	off_t fd_length;
	char *buffer;
	ssize_t buffer_length;

	failed = 0;

	//printf ("Opening close.c\n");
	fd = open ("close.c", O_RDONLY);
	if (fd < 0) {
		printf ("Failed to open close.c: %s\n", strerror (errno));
		failed = 1;
		return;
	}
	defer {
		if (failed) {
			printf ("Closing close.c\n");
		}
		if (close (fd) < 0) {
			printf ("Failed to close close.c: %s\n", strerror (errno));
		}
	};

	//printf ("Getting file length\n");
	fd_length = lseek (fd, 0, SEEK_END);
	if (fd_length < 0) {
		printf ("Failed to get length of close.c: %s\n", strerror (errno));
		failed = 1;
		return;
	}
	if (lseek (fd, 0 , SEEK_SET) != 0) {
		printf ("Failed to reposition to the beginning of close.c: %s\n", strerror (errno));
		failed = 1;
		return;
	}

	//printf ("Allocating buffer space\n");
	buffer = malloc ((size_t) (fd_length + 1));
	if (buffer == NULL) {
		printf ("Failed to allocate memory for buffer: %s\n", strerror (errno));
		failed = 1;
		return;
	}
	defer {
		if (failed) {
			printf ("Freeing buffer space\n");
		}
		free (buffer);
	};
	memset (buffer, 0, (size_t) (fd_length + 1));

	//printf ("Reading close.c\n");
	buffer_length = read (fd, buffer, (size_t) fd_length);
	if (buffer_length < 0) {
		printf ("Failed to read close.c: %s\n", strerror (errno));
		failed = 1;
		return;
	}
	//printf ("Read %i bytes\n", (int) buffer_length);
}

void printMemory () {
	struct rusage usage;

	memset (&usage, 0, sizeof (usage));

	if (getrusage (RUSAGE_SELF, &usage) < 0) {
		printf ("Failed to get memory usage: %s\n", strerror (errno));
		return;
	}

	printf ("Max Resident Set Size:   %li\n", usage.ru_maxrss);
	printf ("Shared Text Memory Size: %li\n", usage.ru_ixrss);
	printf ("Unshared Data Size:      %li\n", usage.ru_idrss);
	printf ("Unshared Stack Size:     %li\n", usage.ru_isrss);
}

int main (const int argc, const char **argv) {
	int loop, loops;

	loops = 1000;
	if (argc == 2) {
		loops = atoi (argv [1]);
		if (loops < 0) {
			loops = 0 - loops;
		}
	}

	printf ("Pre-defer tests:\n");
	printMemory ();

	for (loop = 0; loop < loops; loop++) {
		readClose ();
	}

	printf ("\nPost-defer tests:\n");
	printMemory ();
	return (0);
}
