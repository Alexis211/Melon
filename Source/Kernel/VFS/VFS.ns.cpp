#include "VFS.ns.h"
#include <VFS/FileNode.class.h>

namespace VFS {

DirectoryNode *rootNode;

//TODO : mount stuff

bool setRootNode(DirectoryNode* node) {
	rootNode = node;
	return true;
}

DirectoryNode* getRootNode() {
	return rootNode;
}

FSNode* find(const String& path, FSNode* start) {
	if (start == 0) start = rootNode;

	Vector<String> p = path.split("/");

	FSNode* node = start;
	if (p[0].empty()) node = rootNode;	//Empty first element means string starts with "/"
	for (u32int i = 0; i < p.size(); i++) {
		if (p[i] == "..") {
			node = node->getParent();
		} else if (!p[i].empty() and p[i] != ".") {
			if (node->type() == NT_DIRECTORY) {
				node = ((DirectoryNode*)node)->getChild(p[i]);
			} else {
				node = NULL;
			}
		}
		if (node == NULL) return node;
	}

	return node;
}

FSNode* createFile(const String& path, FSNode* start, bool vrfyperm) {
	if (find(path, start) != NULL) return NULL;	//Something already has that name.
	if (start == 0) start = rootNode;

	Vector<String> p = path.split("/");
	String name = p.back();
	p.pop();

	FSNode* node = start;
	if (!path.empty()) {
		if (p[0].empty()) node = rootNode;
		for (u32int i = 0; i < p.size(); i++) {
			if (p[i] == "..") {
				node = node->getParent();
			} else if (!p[i].empty() and p[i] != ".") {
				if (node->type() == NT_DIRECTORY) {
					node = ((DirectoryNode*)node)->getChild(p[i]);
				} else {
					node = NULL;
				}
			}
			if (node == NULL) return node;
		}
	}

	if (node->type() == NT_DIRECTORY) {
		if ((vrfyperm && node->writable()) or !vrfyperm) return ((DirectoryNode*)node)->createFile(name);
	}
	return NULL;
}

FSNode* createDirectory(const String& path, FSNode* start, bool vrfyperm) {
	if (find(path, start) != NULL) return NULL;	//Something already has that name.
	if (start == 0) start = rootNode;

	Vector<String> p = path.split("/");
	String name = p.back();
	p.pop();

	FSNode* node = start;
	if (!path.empty()) {
		if (p[0].empty()) node = rootNode;
		for (u32int i = 0; i < p.size(); i++) {
			if (p[i] == "..") {
				node = node->getParent();
			} else if (!p[i].empty() and p[i] != ".") {
				if (node->type() == NT_DIRECTORY) {
					node = ((DirectoryNode*)node)->getChild(p[i]);
				} else {
					node = NULL;
				}
			}
			if (node == NULL) return node;
		}
	}

	if (node->type() == NT_DIRECTORY) {
		if ((vrfyperm && node->writable()) or !vrfyperm) return ((DirectoryNode*)node)->createDirectory(name);
	}
	return NULL;
}

bool remove(FSNode* node) {
	FSNode* parent = node->getParent();
	if (parent == NULL) return false;

	if (parent->type() == NT_DIRECTORY) {
		return ((DirectoryNode*)parent)->remove(node);
	} else {
		return false;	//Something wierd happenned
	}
}

bool remove(const String& path, FSNode* start) {
	FSNode* node = find(path, start);
	if (node == NULL) return false;
	return remove(node);
}

String path(FSNode* node) {
	String path;

	while (node != NULL) {
		String t = "/";
		t += node->getName();
		if (t != "//") {
			t += path;
			path = t;
		}
		node = node->getParent();
	}

	if (path.empty()) path = "/";

	return path;
}

}
