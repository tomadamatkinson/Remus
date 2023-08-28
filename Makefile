MAKEFLAGS += --silent

INTERNAL_BUILD_TYPE := Debug

ifdef $$BUILD_TYPE
	INTERNAL_BUILD_TYPE := $$BUILD_TYPE
endif

ifeq ($(OS),Windows_NT)
    PARALLEL=--parallel
else
    PARALLEL=-j$(shell nproc)
endif

.PHONY: init help configure build clean build-tests test pre-commit

help: # Show help for each of the Makefile recipes.
	@grep -E '^[a-zA-Z0-9 -]+:.*#'  Makefile | sort | while read -r l; do printf "\033[1;32m$$(echo $$l | cut -f 1 -d':')\033[00m:$$(echo $$l | cut -f 2- -d'#')\n"; done

init: # Initialize the repository.
	git submodule update --init --recursive

install-hooks: # Install the Git hooks.
	pre-commit install
	pre-commit install --hook-type commit-msg

configure: # Configure the project with CMake.
	cmake -Bbuild -S. -DREMUS_BUILD_TESTING=ON -DCMAKE_BUILD_TYPE=${INTERNAL_BUILD_TYPE}

ifeq (build,$(firstword $(MAKECMDGOALS)))
  # use the rest as arguments for "run"
  BUILD_TARGET := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  # ...and turn them into do-nothing targets
  $(eval $(BUILD_TARGET):;@:)
endif

build: configure # Build the project with CMake. Usage: make build [target]
ifeq (all,$(BUILD_TARGET))
	cmake --build build --config ${INTERNAL_BUILD_TYPE} -- ${PARALLEL}
else
	cmake --build build --config ${INTERNAL_BUILD_TYPE}  --target $(BUILD_TARGET) ${PARALLEL}
endif

clean: # Clean the project with CMake.
	rm -r build

test: # Run the tests with CTest.
	make build remus__tests
	ctest -C ${INTERNAL_BUILD_TYPE} --test-dir build --output-on-failure

pre-commit: # Run the pre-commit checks.
	pre-commit run --all-files
