CPP_SRC := $(shell find ./src -type f -name '*.cpp')
C_SRC := $(shell find ./src -type f -name '*.c')
RS_SRC := $(shell find ./src -type f -name '*.rs')
CPP_LL_SRC := $(patsubst %.cpp, %.cpp.ll, $(CPP_SRC))
C_LL_SRC := $(patsubst %.c, %.c.ll, $(C_SRC))
RS_LL_SRC := $(patsubst %.rs, %.rs.ll, $(RS_SRC))

.PHONY: all

all: $(C_LL_SRC) $(CPP_LL_SRC) $(RS_LL_SRC)

$(C_LL_SRC): %.c.ll: %.c
	clang -o $@ $< -O1 -emit-llvm -S -march=native

$(CPP_LL_SRC): %.cpp.ll: %.cpp
	clang++ -o $@ $< -O1 -emit-llvm -S -march=native

$(RS_LL_SRC): %.rs.ll: %.rs
	rustc --emit=llvm-ir -o $@ $< -C opt-level=1

clean:
	rm src/*.ll
