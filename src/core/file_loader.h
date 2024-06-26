#ifndef FILE_LOADER_H
#define FILE_LOADER_H

#include "stdio.h"
#include "core/mystring.h"
#include "datastructures/arraylist.h"
#include "cglm/cglm.h"
#include "graphics/texture.h"

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
String* xmlnode_attribute(XmlNode* xml_node, char* key);
XmlNode* xmlnode_childbytag(XmlNode* node, char* tag);
u32 xmlnode_childcount_withtag(XmlNode* node, char* tag);
XmlNode* xmlnode_nested_childbytag(XmlNode* node, char delimiter, char* path);

boolean file_loadstring(String* result, char* path);
boolean file_loadxml(XmlNode* root, char* path);

struct Mesh {
    f32* positions;
    u32 position_count;

    f32* normals;
    u32 normals_count;

    f32* uvs;
    u32 uvs_count;

    vec3 color;
    String name;

    f32* transform;
};
typedef struct Mesh Mesh;

void mesh_init(Mesh* mesh, u32 vertex_count);
void mesh_free(Mesh* mesh);

struct ColladaData {
    Mesh* meshes;
    u32 mesh_count;
};
typedef struct ColladaData ColladaData;

struct MaterialEffect {
    String material_name;
    String effect_url;
    vec3 color;
};
typedef struct MaterialEffect MaterialEffect;

struct NodeScene {
    String geometry_name;
    f32* transform;
};
typedef struct NodeScene NodeScene;

boolean file_loadcollada(ColladaData* collada_data, char* path);
void colladadata_free(ColladaData* collada_data);

struct KeyValue {
    String key, value;
};
typedef struct KeyValue KeyValue;
boolean file_loadkeyvalue(ArrayList* key_value_list, char* path);

struct CharacterInfo {
    u32 id;
    u32 x, y, width, height, x_advance;
    int x_offset, y_offset;
};
typedef struct CharacterInfo CharacterInfo;

struct FontFileResult {
    String face;
    u32 paddings[4];
    u32 line_height, base, width, height;

    u32 character_info_count;
    CharacterInfo* character_infos;

    Texture texture;
};
typedef struct FontFileResult FontFileResult;

boolean file_loadfont(FontFileResult* result, char* path, char* texture_path);

#endif