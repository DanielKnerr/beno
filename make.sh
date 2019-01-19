# Build-Ordner löschen (falls vorhanden)
rm -rf build
# Neuen Build-Ordner erstellen
mkdir build

# Alle C++ Dateien kompilieren
g++ -c src/main.cpp src/cli_parser.cpp -lgmpxx -lgmp `libpng-config --cflags`
# Den CUDA-Kernel kompilieren
nvcc -c src/kernel.cu --expt-relaxed-constexpr
# Alle erstellten Objekt-Dateien in den Build-Ordner verschieben
mv *.o build/

# Die kompilierten Objekt-Dateien linken
g++ -o build/executable build/main.o build/cli_parser.o build/kernel.o -L/opt/cuda/lib64 -lcuda -lcudart `libpng-config --ldflags`