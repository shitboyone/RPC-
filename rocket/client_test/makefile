ROCKET_DIR = ../../rocket
COMMON_DIR = $(ROCKET_DIR)/common
NET_DIR = $(ROCKET_DIR)/net
CODER_DIR = $(ROCKET_DIR)/net/coder
TCP_DIR=$(ROCKET_DIR)/tcp
RPC_DIR=$(ROCKET_DIR)/net/rpc
TINYTML=$(ROCKET_DIR)/tinyxml
ORDER_DIR=$(ROCKET_DIR)/order
VPATH = $(COMMON_DIR):$(NET_DIR):$(TCP_DIR):&(TINYTML):$(CODER_DIR):$(RPC_DIR):$(ORDER_DIR)

# 目标文件输出目录
# OBJ_DIR := ./obj

# 源文件和目标文件路径
# SRC_FILES := $(wildcard /*.c src/*.cpp)
# OBJ_FILES := $(addprefix $(OBJ_DIR)/, $(notdir $(SRC_FILES:.c=.o)))

# 获取所有 TinyXML 的 .o 文件
TINYXML_OBJS = $(wildcard ../lib/*.o)
CXXFLAGS += $(shell pkg-config --cflags protobuf)
LDFLAGS  += $(shell pkg-config --libs protobuf)
test:test.cc \
tcp_server.cc\
tcp_buffer.cc\
tcp_connection.cc \
net_addr.cc\
tcp_acceptor.cc\
tcp_server.cc\
io_thread.cc\
io_thread_group.cc\
timer.cc \
timer_event.cc \
EventLoop.cc \
fd_event.cc \
wakeup_fd_event.cc \
log.cc \
utils.cc \
config.cc \
log.h \
fd_event_group.cc\
abstract_protcol.h\
string_code.h\
tcp_client.cc\
tinypb_coder.cc\
tinypb_protocol.cc\
order.pb.cc \
rpc_dispatcher.cc\
rpc_controller.cc\
rpc_channel.cc \
msg_id_util.cc \
$(TINYXML_OBJS)
	g++ -g $^ -o test -I ../../ $(CXXFLAGS) $(LDFLAGS) -pthread


# 清理函数
.PHONY: clean
clean:
	-rm -rf $(OBJ_DIR)
	-rm -f $(TARGET)