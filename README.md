# tstr
Qt *.ts files automatic translator using [Yandex translation api](https://tech.yandex.ru/translate/?ncrnd=4705).

# Features
* Pure C project
* Easy and fast 
* Minimal dependencies - use only [jasmine](https://github.com/zserge/jsmn) as submodule for json response parsing and curl as external library.
* Translated file is equal the source except translated block.
* Now hardcoded en-ru translation and api key + force translation - translate all source blocks

# Building
Building almost the same for Windows/Linux.

1. Install curl(with SSL support)
  * Ubuntu: sudo apt-get install libcurl4-openssl-dev
  * Windows: download curl from official site and build it using instructions in winbuild/BUILD.WINDOWS.txt. Use mode=static and enable ssl support
2. Checkout project:
  * git clone
  * cd tstr
  * git submodule init
  * git submodule update
3. Generate make files
  * Linux: mkdir build && cd build && cmake .. && make
  * Windows: mkdir build. set CMAKe_PREFIX_PATH=your_curl_build_location. Win64/VC2015 project: cmake .. -G"Visual Studio 14 Win64". Open solution and compile.
4. Usage(now in developing still):
  * Linux: cat some_file.ts | ./tstr > translated.ts
  * Windows: type some_file.ts | tstr > translated.ts



