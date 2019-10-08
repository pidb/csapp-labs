#!/bin/bash
gdb \
	-ex 'b mm.c:233 if (size==896)' \
	-ex 'run' \
	--args ./mdriver -f traces/realloc-bal.rep

	# -ex 'break mm.c:mm_init'     \
	# -ex 'break mm.c:coalesce'    \
	# -ex 'break mm.c:insert_root' \
	# -ex 'break mm.c:find_fit'    \
	# -ex 'break mm.c:place'       \
	# -ex 'break mm.c:mm_malloc'   \
	# -ex 'break mm.c:mm_realloc'  \
	# -ex 'break mm.c:mm_free'     \
	# -ex 'break mm.c:extend_heap' \
	# -ex 'break mm.c:get_root'    \
	# -ex 'break mm.c:check_ptrs'  \
	#-ex 'b mm.c:244 if (asize==6144)' \
	#-ex 'break mm.c:next_list' \
	#--args ./mdriver -f traces/cp-decl-bal.rep
	#--args ./mdriver -f traces/short1-bal.rep
	#-ex 'break mm.c:print_seg_lists' \
