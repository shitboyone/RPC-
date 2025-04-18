#ifndef ROCKET_TCP_TCP_BUFFER_H
#define ROCKET_TCP_TCP_BUFFER_H
#include <memory>


#include <vector>

    namespace rocket{

        class TCpBUffer{
            public:
                typedef std::shared_ptr<TCpBUffer> s_ptr;
                TCpBUffer(int size);
                ~TCpBUffer();

                //返回可读字节数
                int readAble();

                //返回可写字节数
                int writeAble();

                int readIndex();

                int writeIndex();

                void writeToBUffer(const char *buf,int size);

                
                void readFromBuffer(std::vector<char>&re,int size);

                
                void resizeBuffer(int new_size);

                void adjustBuffer();
                
                void moveReadIndex(int size);

                void moveWriteIndex(int size);

                 std::vector<char>m_buffer;

            private:
                int m_read_index {0};
                int m_write_index {0};
                int m_size {0};
               
        };




    }

#endif