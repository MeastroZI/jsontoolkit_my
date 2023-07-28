# Programs
CMAKE = cmake
CTEST = ctest

# Options
PRESET = Debug
BACKEND = rapidjson

all: configure compile test

configure: .always
	$(CMAKE) -S . -B ./build -DCMAKE_BUILD_TYPE=$(PRESET) \
		-DJSONTOOLKIT_BACKEND=$(BACKEND) -DJSONTOOLKIT_CONTRIB=ON -DJSONTOOLKIT_TESTS=ON

compile: .always
	$(CMAKE) --build ./build --config $(PRESET) --target clang_format
	$(CMAKE) --build ./build --config $(PRESET) --parallel
	$(CMAKE) --install ./build --prefix ./build/dist \
		--config $(PRESET) --verbose --component jsontoolkit

# Not every CTest version supports the --test-dir option
test: .always
	cd ./build && $(CTEST) --build-config $(PRESET) --output-on-failure --progress --parallel

lint: .always
	$(CMAKE) --build ./build --config $(PRESET) --target clang_tidy

clean: .always
	$(CMAKE) -E rm -R -f build

www: .always
	$(CMAKE) -S . -B ./build -DCMAKE_BUILD_TYPE=$(PRESET) -DJSONTOOLKIT_WEBSITE=ON
	$(CMAKE) --build ./build --config $(PRESET) --target www

# For NMake, which doesn't support .PHONY
.always:
