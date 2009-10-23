#include "DirectoryNode.class.h"

call_t DirectoryNode::m_callTable[] = {
	CALL1(FNIF_GETIDXCHILD, &DirectoryNode::getIdxChildSC),
	CALL1(FNIF_GETNAMECHILD, &DirectoryNode::getNameChildSC),
	CALL0(0, 0)
};

u32int DirectoryNode::getIdxChildSC(u32int idx) {
	FSNode* n = getChild(idx);
	if (n != NULL) return n->resId();
	return (u32int) - 1;
}

u32int DirectoryNode::getNameChildSC(u32int name) {
	String* w = (String*)name;
	FSNode* n = getChild(*w);
	if (n != NULL) return n->resId();
	return (u32int) - 1;
}

bool DirectoryNode::removable() {
	if (!m_contentLoaded)
		if (!loadContent())
			return false;
	return m_children.empty();
}

bool DirectoryNode::unmountable() {
	if (!m_contentLoaded) return true;
	for (u32int i = 0; i < m_children.size(); i++) {
		if (m_children[i]->type() == NT_DIRECTORY) {
			if (!((DirectoryNode*)m_children[i])->unmountable()) return false;
		} else {
			if (!m_children[i]->removable()) return false;
		}
	}
	return true;
}

bool DirectoryNode::loadContent() {
	if (m_contentLoaded) return true;
	bool b = m_fs->loadContents(this);
	if (!b) return false;
	m_length = m_children.size();
	m_contentLoaded = true;
	return b;
}

FSNode* DirectoryNode::getChild(u32int index) {
	if (!m_contentLoaded)
		if (!loadContent())
			return NULL;
	if (index >= m_children.size()) return NULL;
	return m_children[index];
}

FSNode* DirectoryNode::getChild(const String& name) {
	if (!m_contentLoaded)
		if (!loadContent())
			return NULL;
	for (u32int i = 0; i < m_children.size(); i++) {
		if (name == m_children[i]->getName())
			return m_children[i];
	}
	return NULL;
}

FileNode* DirectoryNode::createFile(const String& name) {
	FileNode* n = m_fs->createFile(this, name);
	m_length = m_children.size();
	return n;
}

DirectoryNode* DirectoryNode::createDirectory(const String& name) {
	DirectoryNode* n = m_fs->createDirectory(this, name);
	m_length = m_children.size();
	return n;
}

bool DirectoryNode::remove(FSNode* child) {
	//Check node is indeed one of our childs
	if (!m_contentLoaded)
		if (!loadContent())
			return false;
	u32int idx = (u32int) - 1;
	for (u32int i = 0; i < m_children.size(); i++) {
		if (m_children[i] == child) {
			idx = i;
			break;
		}
	}
	if (idx == (u32int) - 1) return false;

	//Check if we can remove node
	if (!m_children[idx]->removable()) return false;
	if (m_fs != m_children[idx]->getFS()) return false;	//We could !be on the same FS

	//Ask FS to remove node
	if (!m_fs->remove(this, child)) return false;

	//Remove node from our children list
	m_children[idx] = m_children.back();
	m_children.pop();
	m_length--;

	delete child;

	return true;
}
