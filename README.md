# jarvisc - Speech-to-Text Utility Using [april-asr](https://github.com/abb128/april-asr)

## Generating/Building (w/ CMake)

Compiling this project will additionally compile april-asr for its shared library file if one cannot be found on the system. Refer to april-asr (link above) for additional dependency requirements.

To begin, create a directory to hold intermediate build files.
```bash
mkdir build
cd build
````

Then run `cmake ..`, followed by `make` (or your preferred tool). Library files for april-asr, if generated, can be found in `/external/lib`. The application itself will output to `/bin`.
