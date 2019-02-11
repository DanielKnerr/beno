# Output-Ordner erstellen
mkdir -p out

g++ src/util.cpp src/config_parser.cpp src/cli_parser.cpp -o out/util -lmpfr -lgmp