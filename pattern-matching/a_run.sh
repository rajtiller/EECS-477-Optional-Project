command -v clang-format >/dev/null && clang-format -i "/Users/rajtiller/Source/EECS 477/Optional-Project/pattern-matching/pattern-matching.cpp" || /Library/Developer/CommandLineTools/usr/bin/clang-format -i "/Users/rajtiller/Source/EECS 477/Optional-Project/pattern-matching/pattern-matching.cpp"

make clean
make
./pattern-matching
source .venv/bin/activate
python visualize_patern_matching.py