MAKEFLAGS += --silent

.PHONY: init help configure build clean build-tests test

help: # Show help for each of the Makefile recipes.
	@grep -E '^[a-zA-Z0-9 -]+:.*#'  Makefile | sort | while read -r l; do printf "\033[1;32m$$(echo $$l | cut -f 1 -d':')\033[00m:$$(echo $$l | cut -f 2- -d'#')\n"; done

init: # Initialize the repository.
	python3 ./extras/configure.py
	
configure: # Configure the project with CMake.
	cmake -Bbuild -S. -DREMUS_BUILD_TESTING=ON

ifeq (build,$(firstword $(MAKECMDGOALS)))
  # use the rest as arguments for "run"
  BUILD_TARGET := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  # ...and turn them into do-nothing targets
  $(eval $(BUILD_TARGET):;@:)
endif

build: configure # Build the project with CMake. Usage: make build [target]
	cmake --build build --target $(BUILD_TARGET) -- -j$(shell nproc)

clean: # Clean the project with CMake.
	rm -r build

build-tests: configure # Build the tests with CMake.
	cmake --build build --target remus__tests -- -j$(shell nproc)

test: build-tests # Run the tests with CTest.
	ctest --test-dir build --output-on-failure
