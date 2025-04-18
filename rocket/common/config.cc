#include "rocket/common/config.h"
#include "rocket/tinyxml/tinyxml.h"
#include <iostream>
namespace rocket{

    static Config*g_config;

    #define READ_XML_NODE(name,parent)\
    TiXmlElement *name##_node = parent->FirstChildElement(#name);\
    if(!name##_node ){ \
        printf("start rockt server error,failed to read  node %s\n",#name);\
            exit(0);\
    }\


    #define READ_STR_FROM_XML_NODE(name,parent)\
    TiXmlElement *name##_node=parent->FirstChildElement(#name);\
    if(!name##_node||!name##_node->GetText()){\
        printf("start rockt server error,failed to read congif file %s\n",#name);\
            exit(0);\
    }\
    std::string name=std::string(name##_node->GetText());\



    Config::Config(const char *xmlfile){
        TiXmlDocument *xml_document=new TiXmlDocument();

        bool rt=xml_document->LoadFile(xmlfile);

        if(!rt){
            printf("start rockt server error,failed to read congfig file %s\n",xmlfile);
            exit(0);
        }

        READ_XML_NODE(root,xml_document);

        READ_XML_NODE(log,root_node);

        READ_STR_FROM_XML_NODE(log_level,log_node);

        m_log_level=log_level;
    }

    void Config::SetGlobalConfig(const char *xmlfile){
        if(g_config==nullptr){
            g_config=new Config(xmlfile);
        }
    }

     Config* Config::GetGlobalConfig(){
         return g_config;
     }

} 