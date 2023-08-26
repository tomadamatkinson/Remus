MAKEFLAGS += --silent

.PHONY: init help configure build clean build-tests test

help: # Show help for each of the Makefile recipes.
	@grep -E '^[a-zA-Z0-9 -]+:.*#'  Makefile | sort | while read -r l; do printf "\033[1;32m$$(echo $$l | cut -f 1 -d':')\033[00m:$$(echo $$l | cut -f 2- -d'#')\n"; done

init: # Initialize the repository.
	git submodule update --init --recursive

install-hooks: # Install the Git hooks.
	pre-commit install
	pre-commit install --hook-type commit-msg

configure: # Configure the project with CMake.
	cmake -Bbuild -S. -DREMUS_BUILD_TESTING=ON

ifeq (build,$(firstword $(MAKECMDGOALS)))
  # use the rest as arguments for "run"
  BUILD_TARGET := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  # ...and turn them into do-nothing targets
  $(eval $(BUILD_TARGET):;@:)
endif

build: configure # Build the project with CMake. Usage: make build [target]
ifeq (all,$(BUILD_TARGET))
	cmake --build build -- -j$(shell nproc)
else
	cmake --build build --target $(BUILD_TARGET) --parallel
endif

clean: # Clean the project with CMake.
	rm -r build

build-tests: configure # Build the tests with CMake.
	cmake --build build --target remus__tests --parallel

test: build-tests # Run the tests with CTest.
	ctest --test-dir build --output-on-failure
