# Root level make file for convinience sake

.DEFAULT_GOAL := all

.PHONY: all node1 node2 \
       flash-node1 flash-node2 \
       debug-node1 debug-node2 \
       clean clean-node1 clean-node2 \
       size-node1 erase-node1

# Run the requested target (make/make clean) on both nodes and print results at the end
define RUN_BOTH
	+@node1_status=0; node2_status=0; \
	$(MAKE) -C node1 $(1) || node1_status=1; \
	$(MAKE) -C node2 $(1) || node2_status=1; \
	printf '\n========== BUILD SUMMARY ==========\n'; \
	if [ $$node1_status -eq 0 ]; then \
		printf '\033[32m[OK]   Node 1: SUCCESS\033[0m\n'; \
	else \
		printf '\033[31m[FAIL] Node 1: FAILED\033[0m\n'; \
	fi; \
	if [ $$node2_status -eq 0 ]; then \
		printf '\033[32m[OK]   Node 2: SUCCESS\033[0m\n'; \
	else \
		printf '\033[31m[FAIL] Node 2: FAILED\033[0m\n'; \
	fi; \
	printf '===================================\n'; \
	test $$node1_status -eq 0 && test $$node2_status -eq 0
endef

all:
	$(call RUN_BOTH,)

node1:
	$(MAKE) -C node1

node2:
	$(MAKE) -C node2

flash-node1:
	$(MAKE) -C node1 flash

flash-node2:
	$(MAKE) -C node2 flash

debug-node1:
	$(MAKE) -C node1 debug

debug-node2:
	$(MAKE) -C node2 debug

clean:
	$(call RUN_BOTH, clean)

clean-node1:
	$(MAKE) -C node1 clean

clean-node2:
	$(MAKE) -C node2 clean

size-node1:
	$(MAKE) -C node1 size

erase-node1:
	$(MAKE) -C node1 erase

