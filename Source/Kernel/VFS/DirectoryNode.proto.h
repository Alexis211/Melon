#ifndef DEF_DIRECTORYNODE_PROTO_H
#define DEF_DIRECTORYNODE_PROTO_H

#include <VFS/FSNode.proto.h>
#include <Library/Vector.class.h>

class DirectoryNode : public virtual FSNode {
	protected:
	Vector<FSNode*> m_children;
	bool m_contentLoaded;

	protected:
	DirectoryNode() : m_children(), m_contentLoaded(false) {}

	virtual bool FSLoadContent() = 0;

	public:
	bool loadContent() {
		if (m_contentLoaded) return true;
		bool b = FSLoadContent();
		m_length = m_children.size();
		if (b) m_contentLoaded = true;
		return b;
	}

	FSNode* getChild(u32int index) {
		if (!m_contentLoaded)
			if (!loadContent)
				return NULL;
		if (index >= m_children.size()) return NULL;
		return m_children[index];
	}

	FSNode* getChild(String name) {
		if (!m_contentLoaded)
			if (!loadContent)
				return NULL;
		for (u32int i = 0; i < m_children.size(); i++) {
			if (m_children[i]->getName() == name)
				return m_children[i];
		}
		return NULL;
	}
};

#endif
