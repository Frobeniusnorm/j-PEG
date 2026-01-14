make -j

for llfile in src/*.llvm; do
  ../run.sh --grammar ../example/llvm.txt --in "$llfile"
done
