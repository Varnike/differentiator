CC=g++
CFLAGS= -I -Wall -Werror -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++0x-compat -Wc++11-compat -Wc++14-compat -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlarger-than=8192 -Wlogical-op -Wmissing-declarations -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow -Wsign-promo -Wstack-usage=8192 -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -fcheck-new -fsized-deallocation -fstack-check -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -fPIE  -lm -pie 

DEPS = differentiator.h tree.h include/error.h akinator.h include/stack.h include/printval.h config.h include/file.h  include/input.h  include/onegin.h  include/output.h
OBJ = differentiator.o main.o tree.o include/error.o include/stack.o include/printval.o include/file.o  include/input.o  include/onegin.o  include/output.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

tree: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean :
	rm *.o
