.PHONY: build
build:
	cmake --build build/ --parallel "$(shell nproc)"
.PHONY: clean
clean:
	cmake --build build/ --target clean
.PHONY: run-tests
run-tests:
	cd build && ctest --output-on-failure
.PHONY: format
format:
	./scripts/run-clang-format.sh
.PHONY: setup-clang
setup-clang:
	cmake -B build/ -DCMAKE_CXX_COMPILER="clang++"
.PHONY: setup-gcc
setup-gcc:
	cmake -B build/ -DCMAKE_CXX_COMPILER="g++"
