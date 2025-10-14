.PHONY: build
build:
	cmake --build build/ --parallel "$(shell nproc)"
.PHONY: clean
clean:
	cmake --build build/ --target clean
.PHONY: run-tests
run-tests:
	cd build && ctest --output-on-failure
.PHONY: tidy
tidy:
	find emu asm shared -type f | grep "\.\(hpp\|cpp\)$" | xargs -n 1 -I {} -P "$(shell nproc)" clang-tidy -p build "{}"