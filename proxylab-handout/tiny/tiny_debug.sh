#!/bin/bash
gdb \
	-ex 'b tiny.c:56' \
	-ex 'b tiny.c:read_requesthdrs' \
	-ex 'run' \
	--args ./tiny 8550