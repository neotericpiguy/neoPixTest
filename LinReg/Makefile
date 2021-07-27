SRCS=$(shell find -iname '*.cpp')
EXES=${SRCS:.cpp=}

$(info SRCS: ${SRCS})
$(info EXES: ${EXES})

all: ${EXES}

clean: 
	-rm -rf ${EXES}
