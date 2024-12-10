#include "file_loader.h"

boolean file_loadstring(String* result, char* path) {
	char* buffer = 0;
	u32 length;

	FILE* file = fopen(path, "rb");
	if(!file) {
        return false;
    }

	fseek(file, 0, SEEK_END);
	length = ftell(file);
	fseek(file, 0, SEEK_SET);

	buffer = malloc(sizeof(char) * length + 1);
	if(!buffer) {
        return false;
    }

	fread(buffer, sizeof(char), length, file);
	
	fclose(file);
	
	buffer[length] = '\0';
    
    result->length = length-1;
    result->chars = buffer;

	return true;
}

boolean file_loadxml(XmlNode* root, char* path) {
	String file_contents;
	if (!file_loadstring(&file_contents, path)) {
		return false;
	}

	XmlNode* current_node = root;
	xmlnode_initialise(current_node);
	XmlAttribute* attribute = 0;
	int marker = 0;
	for(int i = 0; i < file_contents.length; i++) {		
		// start tag
		if (file_contents.chars[i] == '<') {
			// Header, skip
			if (file_contents.chars[i+1] == '?') {
				i+=2;
				while (file_contents.chars[i] != '?' && file_contents.chars[i+1] != '>') {
					i++;
				}
				i+=2;
				continue;
			}

			// check for tag match & inner text
			if (file_contents.chars[i+1] == '/') {
				if (current_node->children.element_count == 0) {
					string_copy_n(&current_node->inner_text, file_contents.chars + marker, i - marker);
				}

				i+=2;
				marker = i;
				
				String end_tag;
				string_copy_n(&end_tag, file_contents.chars + marker, current_node->tag.length);

				if (!string_equals(current_node->tag, end_tag)) {
					printf("[ERROR] Mismatching xml tags: %s -> %s\n", current_node->tag.chars, end_tag.chars);
					return false;
				}

				i+= current_node->tag.length;

				if (current_node->parent) {
					current_node = current_node->parent;
				}

				continue;
			}
			// not end tag, push new node
			else {
				XmlNode* new_node = arraylist_push(&current_node->children);
				xmlnode_initialise(new_node);

				new_node->parent = current_node;
				current_node = new_node;
			}

			marker = i+1;
			// tag and attributes
			while(i < file_contents.length && file_contents.chars[i] != '>') {
				// tag
				if (file_contents.chars[i] == ' ' && current_node->tag.length == 0) {
					string_copy_n(&current_node->tag, file_contents.chars + marker, i - marker);
					marker = i+1;
				}

				// attribute key
				if (file_contents.chars[i] == '=') {
					attribute = arraylist_push(&current_node->attributes);
					string_copy_n(&attribute->key, file_contents.chars + marker, i - marker);
					marker = i+1;
				}

				// attribute value
				if (file_contents.chars[i] == '"') {
					if (!attribute) {
						printf("XML ERROR: invalid token '\"' at index %d, no attribute key to match value\n", i);
						return false;
					}

					marker = i+1;
					i++;
					while(file_contents.chars[i] != '"') {
						if (i >= file_contents.length) {
							printf("XML ERROR: unmatched quote starting at idx %d\n", marker-1);
							return false;
						}
						i++;
					}
					string_copy_n(&attribute->value, file_contents.chars + marker, i-marker);
					attribute = 0;
					marker = i+2;
				}

				// empty node tag '<node/>'
				if (file_contents.chars[i] == '/') {
					if (current_node->tag.length == 0) {
						string_copy_n(&current_node->tag, file_contents.chars + marker, i - marker);
					}
					marker = i+1;

					if (current_node->parent) {
						current_node = current_node->parent;
					}
				}

				i++;
			}

			
			// Set tag if not set yet and it's not root (has parent)
			if (current_node->tag.length == 0 && current_node->parent) {
				string_copy_n(&current_node->tag, file_contents.chars + marker, i - marker);
			}

			marker = i+1;
		}
	}

	return true;
}


void xmlnode_initialise(XmlNode* xml_node) {
	xml_node->tag.length = 0;
	xml_node->tag.chars = 0;
	xml_node->inner_text.length = 0;
	xml_node->inner_text.chars = 0;
	arraylist_initialise(&xml_node->attributes, 5, sizeof(XmlAttribute));
	arraylist_initialise(&xml_node->children, 5, sizeof(XmlNode));
}

XmlNode* xmlnode_childbytag(XmlNode* node, char* tag) {
	ARRAYLIST_FOREACH(node->children, XmlNode, child) {
		if (string_equals_lit(child->tag, tag)) {
			return child;
		}
	}

	return NULL;
}

XmlNode* xmlnode_nested_childbytag(XmlNode* node, char delimiter, char* path) {
	String tag_to_match = {0};
	int marker = 0;
	int path_length = strlen(path);
	for (int i = 0; i < path_length; i++) {
		if (path[i] == delimiter) {
			if (tag_to_match.length != 0) {
				tag_to_match.length = 0;
				free(tag_to_match.chars);
			}

			string_copy_n(&tag_to_match, path + marker, i - marker);
			node = xmlnode_childbytag(node, tag_to_match.chars);
			if (!node) {
				return NULL;
			}
			marker = i+1;
		}
	}
	
	string_copy_n(&tag_to_match, path + marker, path_length - marker);
	node = xmlnode_childbytag(node, tag_to_match.chars);
	free(tag_to_match.chars);

	return node;
}

String* xmlnode_attribute(XmlNode* xml_node, char* key) {
	ARRAYLIST_FOREACH(xml_node->attributes, XmlAttribute, attribute) {
		if (string_equals_lit(attribute->key, key)) {
			return &attribute->value;
		}
	}

	return NULL;
}

u32 xmlnode_childcount_withtag(XmlNode* node, char* tag) {
	u32 count = 0;
	ARRAYLIST_FOREACH(node->children, XmlNode, child) {
		if (string_equals_lit(child->tag, tag)) {
			count ++;
		}
	}
	return count;
}

void xmlnode_free(XmlNode* xml_node) {
	{
		ARRAYLIST_FOREACH(xml_node->attributes, XmlAttribute, attribute) {
			free(attribute->key.chars);
			free(attribute->value.chars);
		}
	}

	arraylist_free(&xml_node->attributes);
	{
		ARRAYLIST_FOREACH(xml_node->children, XmlNode, node) {
			xmlnode_free(node);
		}
	}
	arraylist_free(&xml_node->children);

	if (xml_node->tag.length > 0) {
		free(xml_node->tag.chars);
	}

	if (xml_node->inner_text.length > 0) {
		free(xml_node->inner_text.chars);
	}
}

boolean file_loadcollada(ColladaData* collada_data, char* path) {
	XmlNode root = {0};
	if(!file_loadxml(&root, path)) {
		return false;
	}

	MaterialEffect* material_effects;		
	XmlNode* library_materials = xmlnode_nested_childbytag(&root, '.', "COLLADA.library_materials");
	u32 materials_count = library_materials->children.element_count;
	{ // material effects
		material_effects = malloc(sizeof(MaterialEffect) * materials_count);
		{ // Get name and url for materials
			ARRAYLIST_FOREACHI(library_materials->children, idx, XmlNode, child) {
				string_copy(&material_effects[idx].material_name, xmlnode_attribute(child, "id"));

				XmlNode* effect_url_node = arraylist_at(&child->children, 0);
				string_copy(&material_effects[idx].effect_url, xmlnode_attribute(effect_url_node, "url"));
			}
		}
		{ // Get color for material
			XmlNode* library_effects = xmlnode_nested_childbytag(&root, '.', "COLLADA.library_effects");
			ARRAYLIST_FOREACHI(library_effects->children, idx, XmlNode, child) {
				for(int i = 0; i < materials_count; i++) {
					MaterialEffect* material = material_effects + i;
					String* node_id = xmlnode_attribute(child, "id");
					if (string_endswith_lit(material->effect_url, node_id->chars)) {
						XmlNode* color_node = xmlnode_nested_childbytag(child, '.', "profile_COMMON.technique.lambert.diffuse.color");
						f32* colors;
						string_tofloatarray(color_node->inner_text, &colors, ' ', 4);
						material->color[0] = colors[0];
						material->color[1] = colors[1];
						material->color[2] = colors[2];
					}
				}
			}
		}
	}
	NodeScene* node_scenes;
	u32 node_count = 0;
	{ // node scenes
		XmlNode* visual_scene = xmlnode_nested_childbytag(&root, '.', "COLLADA.library_visual_scenes.visual_scene");
		{
			ARRAYLIST_FOREACH(visual_scene->children, XmlNode, node) {
				u32 instance_geometry_count = xmlnode_childcount_withtag(node, "instance_geometry");
				if (instance_geometry_count > 0) {
					node_count++;
				}
			}
		}

		node_scenes = malloc(sizeof(NodeScene) * node_count);
		int current_node = 0;
		ARRAYLIST_FOREACH(visual_scene->children, XmlNode, node) {
			u32 instance_geometry_count = xmlnode_childcount_withtag(node, "instance_geometry");
			if (instance_geometry_count > 0) {
				NodeScene* node_scene = node_scenes + current_node;
				XmlNode* matrix_node = xmlnode_childbytag(node, "matrix");
				string_tofloatarray(matrix_node->inner_text, &node_scene->transform, ' ', 16);
				
				XmlNode* instance_node = xmlnode_childbytag(node, "instance_geometry");
				string_copy(&node_scene->geometry_name, xmlnode_attribute(instance_node, "name"));
				current_node++;
			}
		}

	}
	{ // library geometries
		XmlNode* library_geometries = xmlnode_nested_childbytag(&root, '.', "COLLADA.library_geometries");
		{
			collada_data->mesh_count = 0;
			ARRAYLIST_FOREACH(library_geometries->children, XmlNode, child) {
				if (string_equals_lit(child->tag, "geometry")) {
					collada_data->mesh_count++;
				}
			}
			collada_data->meshes = malloc(collada_data->mesh_count * sizeof(Mesh));
		}

		int mesh_idx = 0;
		ARRAYLIST_FOREACH(library_geometries->children, XmlNode, geometry_node) {
			if (string_equals_lit(geometry_node->tag, "geometry")) {
				Mesh* mesh = collada_data->meshes + mesh_idx;

				XmlNode* mesh_node = xmlnode_childbytag(geometry_node, "mesh");
				
				// positions
				XmlNode* positions_mesh = arraylist_at(&mesh_node->children, 0);
				XmlNode* positions_float_array = xmlnode_childbytag(positions_mesh, "float_array");

				String positions_str = positions_float_array->inner_text;
				u32 positions_count = atoi(xmlnode_attribute(positions_float_array, "count")->chars);
				float* positions = 0;
				string_tofloatarray(positions_str, &positions, ' ', positions_count);

				// normals
				XmlNode* normals_mesh = arraylist_at(&mesh_node->children, 1);
				XmlNode* normals_float_array = xmlnode_childbytag(positions_mesh, "float_array");

				String normals_str = normals_float_array->inner_text;
				u32 normals_count = atoi(xmlnode_attribute(normals_float_array, "count")->chars);
				float* normals = 0;
				string_tofloatarray(normals_str, &normals, ' ', normals_count);

				// uvs
				XmlNode* uvs_mesh = arraylist_at(&mesh_node->children, 2);
				XmlNode* uvs_float_array = xmlnode_childbytag(positions_mesh, "float_array");

				String uvs_str = uvs_float_array->inner_text;
				u32 uvs_count = atoi(xmlnode_attribute(uvs_float_array, "count")->chars);
				float* uvs = 0;
				string_tofloatarray(uvs_str, &uvs, ' ', uvs_count);

				// indices
				XmlNode* triangles_node = arraylist_at(&mesh_node->children, 4);
				u32 triangle_count = atoi(xmlnode_attribute(triangles_node, "count")->chars);
				int* indices = 0;
				XmlNode* indices_node = xmlnode_childbytag(triangles_node, "p");
				string_tointarray(indices_node->inner_text, &indices, ' ', triangle_count*3*3);

				mesh_init(mesh, triangle_count * 3);
				
				{
					// set mesh transform
					String* geometry_name = xmlnode_attribute(geometry_node, "name");
					string_copy(&mesh->name, geometry_name);
					for (int i = 0; i < node_count; i++) {
						NodeScene* node_scene = node_scenes + i;
						if (string_equals(node_scene->geometry_name, *geometry_name)) {
							for (int j = 0; j < 16; j++) {
								mesh->transform[j] = node_scene->transform[j];
							}
						}
					}
				}

				u32 vertices_marker = 0, uvs_marker = 0;
				for(int i = 0; i < triangle_count*3*3; i+=3) {
					u32 positions_index = indices[i] * 3;
					mesh->positions[vertices_marker] = positions[positions_index];
					mesh->positions[vertices_marker+1] = positions[positions_index+1];
					mesh->positions[vertices_marker+2] = positions[positions_index+2];

					u32 uvs_index = indices[i+2] * 2;
					mesh->uvs[uvs_marker] = uvs[uvs_index];
					mesh->uvs[uvs_marker+1] = uvs[uvs_index+1];

					vertices_marker += 3;
					uvs_marker += 2;
				}

				// calculate normals from positions
				{
					vec3 a = GLM_VEC3_ZERO_INIT, 
						 b = GLM_VEC3_ZERO_INIT, 
						 c = GLM_VEC3_ZERO_INIT, 
						 ab = GLM_VEC3_ZERO_INIT, 
						 ac = GLM_VEC3_ZERO_INIT, 
						 normal = GLM_VEC3_ZERO_INIT;

					for (int i = 0; i < mesh->position_count; i+=9) {
						a[0] = mesh->positions[i];
						a[1] = mesh->positions[i+1];
						a[2] = mesh->positions[i+2];

						b[0] = mesh->positions[i+3];
						b[1] = mesh->positions[i+4];
						b[2] = mesh->positions[i+5];

						c[0] = mesh->positions[i+6];
						c[1] = mesh->positions[i+7];
						c[2] = mesh->positions[i+8];

						glm_vec3_sub(a, b, ab);
						glm_vec3_sub(a, c, ac);

						glm_vec3_normalize(ab);
						glm_vec3_normalize(ac);

						glm_vec3_cross(ab, ac, normal);
						glm_vec3_normalize(normal);

						mesh->normals[i] = normal[0];
						mesh->normals[i+1] = normal[1];
						mesh->normals[i+2] = normal[2];
						mesh->normals[i+3] = normal[0];
						mesh->normals[i+4] = normal[1];
						mesh->normals[i+5] = normal[2];
						mesh->normals[i+6] = normal[0];
						mesh->normals[i+7] = normal[1];
						mesh->normals[i+8] = normal[2];
					}
				}

				{ // Set mesh material
					String* material_name = xmlnode_attribute(triangles_node, "material");
					if (material_name) {
						for(int i = 0; i < materials_count; i++) {
							MaterialEffect* material = material_effects + i;
							if (string_equals(material->material_name, *material_name)) {
								glm_vec3_copy(material->color, mesh->color);
							}
						}
					}
				}

				free(positions);
				free(normals);
				free(uvs);
				free(indices);

				mesh_idx++;
			}
		}
		free(material_effects);
		free(node_scenes);
	}

	return true;
}

void mesh_init(Mesh* mesh, u32 vertex_count) {
	mesh->position_count = vertex_count * 3;
	mesh->positions = malloc(sizeof(f32) * mesh->position_count);
	
	mesh->normals_count = vertex_count * 3;
	mesh->normals = malloc(sizeof(f32) * mesh->normals_count);
	
	mesh->uvs_count = vertex_count * 2;
	mesh->uvs = malloc(sizeof(f32) * mesh->uvs_count);

	mesh->transform = malloc(sizeof(f32) * 16);
}

void mesh_free(Mesh* mesh) {
	free(mesh->positions);
	free(mesh->normals);
	free(mesh->uvs);
	free(mesh->transform);
}

void colladadata_free(ColladaData* collada_data) {
	for(int i = 0; i < collada_data->mesh_count; i++) {
		Mesh* mesh = collada_data->meshes + i;
		mesh_free(mesh);
	}

	free(collada_data->meshes);
}

boolean file_loadkeyvalue(ArrayList* key_value_list, char* path) {
	String file_content;
	if (!file_loadstring(&file_content, path)) {
		printf("[ERROR] Failed to load font file string");
		return false;
	}

	arraylist_initialise(key_value_list, 200, sizeof(KeyValue));
	
	int marker = 0;
	KeyValue* current_key_value = arraylist_push(key_value_list);
	for(int i = 0; i < file_content.length; i++) {
		while(file_content.chars[i] != '=') {
			i++;
		}

		string_copy_n(&current_key_value->key, file_content.chars + marker, i - marker);
		i++;
		marker = i;

		while (file_content.chars[i] != ' ' && file_content.chars[i] != '\n') {
			i++;
		}
		
		string_copy_n(&current_key_value->value, file_content.chars + marker, i - marker);
		i++;
		marker = i;

		current_key_value = arraylist_push(key_value_list);

		while (file_content.chars[i] == ' ' || file_content.chars[i] == '\n' || string_chars_startswith(file_content.chars + i, "\r\n")) {
			i++;
		}
		marker = i;
	}

	free(file_content.chars);
	return true;
}

boolean file_loadfont(FontFileResult* result, char* path, char* texture_path) {
	ArrayList key_value_list;
	if (!file_loadkeyvalue(&key_value_list, path)) {
		printf("[ERROR] Failed to load key value list in font loading");
		return false;
	}

	
	{
		u32 character_info_index = 0;
		CharacterInfo* character_info = 0;
		ARRAYLIST_FOREACH(key_value_list, KeyValue, key_value) {
			if (string_equals_lit(key_value->key, "info face")) {
				string_copy(&result->face, &key_value->key);
			}
			if (string_equals_lit(key_value->key, "padding")) {
				// parse padding
				u32 padding_index = 0;
				u32 marker = 0;
				for (int i = 0; i < key_value->value.length; i++) {
					while (key_value->value.chars[i] != ',') {
						i++;
					}

					result->paddings[padding_index++] = atoi(key_value->value.chars + marker);
					i++;
					marker = i;
				}
			}
			if (string_equals_lit(key_value->key, "common lineHeight")) {
				result->line_height = atoi(key_value->value.chars);
			}
			if (string_equals_lit(key_value->key, "base")) {
				result->base = atoi(key_value->value.chars);
			}
			if (string_equals_lit(key_value->key, "scaleW")) {
				result->width = atoi(key_value->value.chars);
			}
			if (string_equals_lit(key_value->key, "scaleH")) {
				result->height = atoi(key_value->value.chars);
			}
			if (string_equals_lit(key_value->key, "chars count")) {
				result->character_info_count = atoi(key_value->value.chars);
				result->character_infos = malloc(sizeof(CharacterInfo) * result->character_info_count);
			}
			if (string_equals_lit(key_value->key, "char id")) {
				character_info = result->character_infos + character_info_index;
				character_info->id = atoi(key_value->value.chars);
				character_info_index++;
			}
			if (string_equals_lit(key_value->key, "x")) {
				character_info->x = atoi(key_value->value.chars);
			}
			if (string_equals_lit(key_value->key, "y")) {
				character_info->y = atoi(key_value->value.chars);
			}
			if (string_equals_lit(key_value->key, "width")) {
				character_info->width = atoi(key_value->value.chars);
			}
			if (string_equals_lit(key_value->key, "height")) {
				character_info->height = atoi(key_value->value.chars);
			}
			if (string_equals_lit(key_value->key, "xoffset")) {
				character_info->x_offset = atoi(key_value->value.chars);
			}
			if (string_equals_lit(key_value->key, "yoffset")) {
				character_info->y_offset = atoi(key_value->value.chars);
			}
			if (string_equals_lit(key_value->key, "xadvance")) {
				character_info->x_advance = atoi(key_value->value.chars);
			}
		}
	}
	
	if (!texture_init(&result->texture, texture_path)) {
		printf("[ERROR] Failed to load font texture\n");
		return false;
	}

	{
		ARRAYLIST_FOREACH(key_value_list, KeyValue, key_value) {
			free(key_value->key.chars);
			free(key_value->value.chars);
		}
	}

	arraylist_free(&key_value_list);
	return true;
}
