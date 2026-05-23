# sndhllhc-raw-to-digi

Performs standalone raw to digi conversion and real time data quality monitoring for silicon strip (SiStrip) data in the SND@HL-LHC experiment.
Input raw data is produced by CMS DAQ. Currently only Zero Suppressed DAQ mode (used for physics) is supported.

## Features

- SiStrip raw to digi conversion
- Real time data quality monitoring
- Dump info from raw data

## Dependencies

- ROOT 6.36 or later

## Project Structure
- `sistrip_io/` contains all necessary input-output classes used to generate ROOT dictionaries
- `raw_info/` contains source file to dump info from raw data
- `raw_to_digi/` contains headers and source files for the raw to digi conversion and real time data quality monitoring
- `tests/` contains source files for ctest, used to check data consistency against a small dataset produced by CMSSW
- `docs/` contains additional documentation
- `Dockerfile` for building an image with this project installed

## Build instructions
```
cd sndhllhc-raw-to-digi
mkdir build
cd build
cmake ..
cmake --build
ctest # optionally
```

## Run instructions
### SiStrip raw to digi conversion
`./raw_to_digi <input_path> <detector_info> <output_path>`
For example:
`./bin/raw_to_digi tests/data/run_1000779_raw.root tests/data/detector_info.csv tests/data/run_1000779_digi.root`
### Real time data quality monitoring
`./real_time_monitoring <input_root_file> <detector_info> <output_root_file> <n_treads>`
For example:
`./bin/real_time_monitoring tests/data/run_1000779_raw.root tests/data/detector_info.csv tests/data/histos.root 2`
### Dump info from raw data
`./raw_info_dump <input_root_path>`
For example:
`./bin/raw_info_dump tests/data/run_1000779_raw.root`
