#!/bin/bash
gdb \
	-ex 'b proxy.c:doit' \
	-ex 'b proxy.c:read_requesthdrs' \
	-ex 'run' \
	--args ./proxy 9000