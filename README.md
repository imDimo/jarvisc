# jarvisc - Speech-to-Text Utility Using [april-asr](https://github.com/abb128/april-asr)

## Generating/Building (w/ CMake)

Compiling this project will additionally compile april-asr for its static library file. Refer to april-asr (link above) for additional dependency requirements.

```bash
mkdir build
cd build
````

Then run `make` or `cmake --build .`. Libraries and the compiled program will be located in `/bin`.
