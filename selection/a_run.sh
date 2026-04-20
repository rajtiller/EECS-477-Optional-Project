command -v clang-format >/dev/null && clang-format -i "/Users/rajtiller/Source/EECS 477/Optional-Project/selection/selection.cpp" || /Library/Developer/CommandLineTools/usr/bin/clang-format -i "/Users/rajtiller/Source/EECS 477/Optional-Project/selection/selection.cpp"

make clean
make
./selection
source .venv/bin/activate
python visualize_selection.py