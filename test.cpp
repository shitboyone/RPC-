#include  <string>
#include <stdio.h>
#include <iostream>

template <typename... Arg>
void test_snpstr(Arg &&...args){
    (std::cout << ... << args) << std::endl;
}

int main(){
    // std::string buffer;
    // buffer.resize(10);
    // int j=snprintf(&buffer[0],6,"%s\n","abcde");
    // std::cout<<j<<std::endl;
    // std::cout<<buffer<<std::endl;

    test_snpstr("aaa",1,1.2);

}