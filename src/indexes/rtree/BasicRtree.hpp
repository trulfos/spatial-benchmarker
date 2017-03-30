#pragma once
#include "Rtree.hpp"

namespace Rtree
{

/**
 * Abstract R-tree with the default insert algorithm.
 *
 * Inserts are performed by digging down the tree using a `chooseSubtree`
 * method. Nodes may then be split upwards and entries distributed between the
 * two new nodes using the `redistriute` method.
 */
template<class N, unsigned m>
class BasicRtree : public Rtree<N, m>
{
	using E = typename N::Entry;

	public:
		/**
		 * Insert an entry in the tree.
		 *
		 * @param object DataObject to insert
		 */
		void insert(const DataObject& object) override;

	protected:

		/**
		 * Select a child of parent to be the destination of entry.
		 *
		 * @param parent Parent node to select child from
		 * @param entry Node subject to insertion
		 * @return Best fitting node in parent
		 */
		virtual E& chooseSubtree(E& parent, const E& entry) = 0;


		/**
		 * Redistribute the children of two entries.
		 *
		 * This is used during splitting to distribute entries between the two
		 * nodes.
		 *
		 * @param a First node (with children)
		 * @param b Second node (with children)
		 * @param level Level of the given entries
		 */
		virtual void redistribute(E& a, E& b, unsigned level) = 0;

	private:

		/**
		 * Split the root and include the given entry in the new root node.
		 *
		 * @param entry Entry to include in new root
		 */
		void splitRoot(const E& entry);
};


/**

 ___                 _                           _        _   _             
|_ _|_ __ ___  _ __ | | ___ _ __ ___   ___ _ __ | |_ __ _| |_(_) ___  _ __  
 | || '_ ` _ \| '_ \| |/ _ \ '_ ` _ \ / _ \ '_ \| __/ _` | __| |/ _ \| '_ \ 
 | || | | | | | |_) | |  __/ | | | | |  __/ | | | || (_| | |_| | (_) | | | |
|___|_| |_| |_| .__/|_|\___|_| |_| |_|\___|_| |_|\__\__,_|\__|_|\___/|_| |_|
              |_|                                                           
*/

template<class N, unsigned m>
void BasicRtree<N, m>::insert(const DataObject& object)
{
	E entry (object);

	// No nodes - set entry as root
	if (this->getHeight() == 0) {
		return this->addLevel(entry);
	}

	// Single entry - add new root
	if (this->getHeight() == 1) {
		return splitRoot(entry);
	}

	// Dig down to destination leaf node
	std::vector<E *> path {&this->getRoot()};

	while (path.size() < this->getHeight() - 1) {
		path.back()->getMbr() += entry.getMbr();
		path.push_back(
				&chooseSubtree(*path.back(), entry)
			);
	}

	// Split nodes bottom-up as long as necessary
	auto top = path.rbegin();

	while (top != path.rend() && (*top)->getNode()->isFull()) {
		entry = E(this->allocateNode(), {entry});
		redistribute(**top, entry, top - path.rbegin());
		++top;
	}

	// Split root?
	if (top == path.rend()) {
		return splitRoot(entry);
	}

	(*top)->add(entry);
};


template<class N, unsigned m>
void BasicRtree<N, m>::splitRoot(const E& entry)
{
	this->addLevel(
			E(this->allocateNode(), {this->getRoot(), entry})
		);
}

}
