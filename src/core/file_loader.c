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
					if (current_node->tag.length != 0) {
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

void xmlnode_free(XmlNode* xml_node) {
	{
		ARRAYLIST_FOREACH(((ArrayList*)&xml_node->attributes), XmlAttribute, attribute) {
			free(attribute->key.chars);
			free(attribute->value.chars);
		}
	}

	arraylist_free(&xml_node->attributes);
	{
		ARRAYLIST_FOREACH(((ArrayList*)&xml_node->children), XmlNode, node) {
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
