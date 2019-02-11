# Output-Ordner erstellen
mkdir -p out
# Build-Ordner löschen (falls vorhanden)
rm -rf build
# Neuen Build-Ordner erstellen
mkdir build

# Alle C++ Dateien kompilieren
g++ -c src/main.cpp src/renderer.cpp src/config_parser.cpp src/cli_parser.cpp -O3 -pthread -ffast-math -lmpfr -lgmp `libpng-config --cflags`
# Den CUDA-Kernel kompilieren
nvcc -c src/kernel_gpu.cu --expt-relaxed-constexpr -O3
# Alle erstellten Objekt-Dateien in den Build-Ordner verschieben
mv *.o build/

# Die kompilierten Objekt-Dateien linken
g++ -o out/renderer build/main.o build/config_parser.o build/kernel_gpu.o build/cli_parser.o build/renderer.o -L/opt/cuda/lib64 -pthread -lmpfr -lgmp -lcuda -lcudart `libpng-config --ldflags`