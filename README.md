# jarvisc - Speech-to-Text Utility Using [april-asr](https://github.com/abb128/april-asr)

## Dependencies

This project uses [april-asr](https://github.com/abb128/april-asr) for speech recognition. If CMake cannot find an existing april-asr library on your system, it will attempt to build/compile it from source during the jarvisc build process. Refer to their page for additional dependency requirements.

The source code required to build april-asr is included in this project as a git submodule. You can pull the submodule by entering the project directory, then running `git submodule init` and `git submodule update`.

## Generating/Building (w/ CMake)

To begin, create and enter a directory to hold intermediate build files.

```bash
mkdir build
cd build
````

Run `cmake ..` to begin the CMake build process. This will additionally attempt to build and compile the april-asr libraries if april-asr was not found on the system. If generated, these libraries can be found in `/external/lib`.

Finally, run `cmake --build .` to compile the program. The jarvisc binary will be placed in `/bin` along with a start script.

## Running and Recording

An april model is required to perform speech detection for the program. Download a [model](https://abb128.github.io/april-asr/models.html) and place it in the same directory (or subdirectory) as the jarvisc binary.

For now, audio must be piped into the program with a utility such as parec. The `pipe-parec` bash script outputted along with the binary can be used to select an audio device and start the program.
