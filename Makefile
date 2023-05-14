sgwennu: sgwennu.c
	# This is the command that is run to build the executable.
	# make expands $(CC) to cc when run.
	# All, extra and pedantic warnings.
	# Standard C99 (allows declaration of variables within
	#  function.)
	$(CC) sgwennu.c -o sgwennu -Wall -Wextra -pedantic -std=c99
