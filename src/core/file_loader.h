#ifndef FILE_LOADER_H
#define FILE_LOADER_H

#include "stdio.h"
#include "core/string.h"
#include "datastructures/arraylist.h"

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS // enables use of fopen
#endif

struct XmlNode {
    String tag;
    String inner_text;
    struct XmlNode* parent;
    ArrayList attributes;
    ArrayList children;
};
typedef struct XmlNode XmlNode;

struct XmlAttribute {
    String key;
    String value;
};
typedef struct XmlAttribute XmlAttribute;

void xmlnode_initialise(XmlNode* xml_node);
void xmlnode_free(XmlNode* xml_node);

boolean file_loadstring(String* result, char* path);
boolean file_loadxml(XmlNode* root, char* path);

#endif