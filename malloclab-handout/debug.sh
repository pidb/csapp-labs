#!/bin/bash
gdb \
	-ex 'break mm.c:mm_init'     \
	-ex 'break mm.c:coalesce'    \
	-ex 'break mm.c:insert_root' \
	-ex 'break mm.c:find_fit'    \
	-ex 'break mm.c:place'       \
	-ex 'break mm.c:mm_malloc'   \
	-ex 'break mm.c:mm_free'     \
	--args ./mdriver -f traces/short1-bal.rep
