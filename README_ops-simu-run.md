ops-simu-run.sh - Run OPS simulations
=====================================

`ops-simu-run.sh` is a convenience script to run simulations. The only mandatory
parameter is the simulations mode (`-m`). All others are optional. The order of
settings is defined as follows:

1. Use the settings defined via the command line parameters (for example the omnetpp.ini file).
2. Use the settings from the user home directory, i.e. defined in `~/.opsSettings`.
3. Use the default settings from `settings.default`. This file can be used as a template for your own `~/.opsSettings`.

The following parameters are currently implemented in `ops-simu-run.sh`:

* `-m <cmdenv | tkenv>` (Mandatory): Select the simulation environment:
    * `cmdenv`: Run the simulation in command line mode. This is meant for simulations running on a server (headless).
    * `tkenv` : Run the simulation with a graphical user interface. This is meant for debugging and showcases.
* `-c <filename.ini>` Run the simulation using the given OMNeT++ simulation configuration file.
* `-o <output dir>`: Define the output directory for the simulation logs. It is defined as a subdirectory of `simulations/out`. As default, the format `YYYY-mm-dd_HH-MM-SS_<omnetpp.ini-filename>` is used. This ensures that each simulation run result data is stored in a separate directory.
* `-p <parsers.txt>`: Run the parsers defined in `parsers.txt` after the simulation ended. An example file is available in the repository.
* `-g` : Append the KeetchiLib log file to the main log file
* `-b <backup dir>` : Makes a compressed (tar.gz) copy of all output files to the given directory

The functionality of the `-p` option is still being tested. It currently works as follows:

1. Iterate over all `.txt` files in the simulation output directory with a current date string (i.e. `*-YYYYmmddd-*`). This should get all OMNeT++ simulation logfiles.
2. For each of these files, call every parser in the `parsers.txt` and append the logfile name and path.

The user has to concern the following:

* Each parser has to be error-tolerant, i.e. it should silently ignore logfiles which cannot be parsed instead of creating erroneous (nonsense) output.
* This step is only tested with the default output path (no user-defined path)
* The parser output filenames should be created in an standardized way. One option is to use the Python `FileHelpers` in the `helper` module. For an example parser, please refer to for example `parsers/s05-keetchi-evaluation-parsers/s05-packets-sent-per-node-stats.py`


Suggestions and improvements are welcome!
