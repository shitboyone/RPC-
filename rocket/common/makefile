VPATH = ../../rocket : ../../tinyxml

# 目标文件输出目录
OBJ_DIR := ./obj

# 源文件和目标文件路径
# SRC_FILES := $(wildcard /*.c src/*.cpp)
OBJ_FILES := $(addprefix $(OBJ_DIR)/, $(notdir $(SRC_FILES:.c=.o)))

# 获取所有 TinyXML 的 .o 文件
TINYXML_OBJS = $(wildcard ../lib/*.o)

test:test.cc log.cc utils.cc config.cc log.h $(TINYXML_OBJS)
	g++ -g $^ -o test -I./ -pthread


# 清理函数
.PHONY: clean
clean:
	-rm -rf $(OBJ_DIR)
	-rm -f $(TARGET)
