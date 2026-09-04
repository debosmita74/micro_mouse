# ── Micro Mouse Makefile ─────────────────────────────────────
# Convenience wrapper around CMake

BUILD_DIR := build

.PHONY: all clean flash rebuild

all:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. && make -j$$(nproc)

clean:
	@rm -rf $(BUILD_DIR)

flash: all
	@cd $(BUILD_DIR) && make flash

rebuild: clean all

# ── Simulation ──────────────────────────────────────────────
.PHONY: sim
sim:
	@cd simulations && python3 maze_simulator.py

# ── Tests ───────────────────────────────────────────────────
.PHONY: test
test:
	@echo "Running tests..."
	@gcc -Wall -Wextra -Isrc -Iinclude tests/test_pid.c src/pid.c -o build/test_pid -lm && ./build/test_pid
	@gcc -Wall -Wextra -Isrc -Iinclude tests/test_maze.c src/maze.c -o build/test_maze && ./build/test_maze
	@echo "All tests passed!"
