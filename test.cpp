#include  <string>
#include <stdio.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>

template <typename... Arg>
void test_snpstr(Arg &&...args){
    (std::cout << ... << args) << std::endl;
}


struct Person{
    char a;
    char b;
    char c;
    int *d=new int();
};

class base{
    public:
    base(std::string name_):name(name_){
        std::cout<<"调用base的构造函数"<<std::endl;
    }
    std::string name;
  virtual  std::string getname(){
        return name;
    }
};

class chil :public base{
    public:
    chil(std::string name):base(name){
        std::cout<<"调用chil的无参构造"<<std::endl;
    }
    std::string getname(){
        return "chid";
    }
};


int main(){
    // std::string buffer;
    // buffer.resize(10);
    // int j=snprintf(&buffer[0],6,"%s\n","abcde");
    // std::cout<<j<<std::endl;
    // std::cout<<buffer<<std::endl;

        

}