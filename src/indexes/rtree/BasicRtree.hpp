#pragma once
#include "Rtree.hpp"

namespace Rtree
{

/**
 * Abstract R-tree with the default insert algorithm.
 *
 * Inserts are performed by digging down the tree using a `chooseSubtree`
 * method. Nodes may then be split upwards and entries distributed between the
 * two new nodes using the `redistribute` method.
 */
template<class N, unsigned m>
class BasicRtree : public Rtree<N, m>
{
	using Base = Rtree<N, m>;
	using Mbr = typename N::Mbr;

	public:
		// These depends on template parameters
		using Base::getHeight;
		using Base::getRoot;
		using Base::addLevel;


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
		virtual typename N::iterator chooseSubtree(
				Entry<N> parent,
				const Entry<N>& entry
			) = 0;


		/**
		 * Redistribute the children of two entries.
		 *
		 * This is used during splitting to distribute entries between the two
		 * nodes.
		 *
		 * @param a First node (with children)
		 * @param b Second node (with children)
		 * @param level Level of the given nodes
		 * @param enclosing MBR enclosing all items
		 */
		virtual void redistribute(
				Entry<N> a,
				Entry<N> b,
				unsigned level
			) = 0;

	private:

		/**
		 * Split the root and include the given entry in the new root node.
		 *
		 * @param entry Entry to include in new root
		 */
		void splitRoot(const Entry<N>& entry);


		/**
		 * Splits an entry.
		 *
		 * Splits the first entry with the second included as a child. Returns a
		 * new entry with a new node. This does not work for the root.
		 *
		 * @see splitRoot
		 * @see redistribute
		 *
		 * @param original Original entry with node to split
		 * @param include New entry to include as child
		 * @param level Level of original node (forwarded to redistribute)
		 *
		 * @return New entry with some children from original
		 */
		template<class E>
		Entry<N> split(E& original, const Entry<N>& include, unsigned level);
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
template<class E>
Entry<N> BasicRtree<N, m>::split(
		E& original,
		const Entry<N>& include,
		unsigned level
	)
{
	// Create new entry (and node with included entry)
	Entry<N> newEntry = Entry<N>(new N({include}));

	// Redistribute children between nodes
	redistribute(
			original,
			newEntry,
			level
		);

	// Recalculate relevant MBRs
	original.recalculate();
	newEntry.recalculate();

	return newEntry;
}

template<class N, unsigned m>
void BasicRtree<N, m>::insert(const DataObject& object)
{
	Entry<N> original (object);

	// No nodes - set entry as root
	if (getHeight() == 0) {
		return addLevel(original);
	}

	// Single entry - add new root
	if (getHeight() == 1) {
		return splitRoot(original);
	}

	Entry<N> entry = original;

	if (getHeight() > 2) {
		// Dig down to destination leaf node and update MBRs on the way
		std::vector<typename N::iterator> path {
				chooseSubtree(getRoot(), entry)
			};

		while (path.size() < getHeight() - 2) {
			path.back()->include(entry);
			path.push_back(
					chooseSubtree(*path.back(), entry)
				);
		}

		// Split nodes bottom-up as long as necessary
		auto i = path.rbegin();
		while (i != path.rend() && (*i)->getNode().isFull()) {
			entry = split(**i, entry, i - path.rbegin());
			++i;
		}

		// Add to some internal node (which is not the root)
		if (i != path.rend()) {
			(*i)->getNode().add(entry);

			while (i != path.rend()) {
				(*i)->include(original);
				++i;
			}

			getRoot().include(original);
			return ;
		}
	}

	// Split root?
	if (getRoot().getNode().isFull()) {
		return splitRoot(
				split(getRoot(), entry, getHeight() - 2)
			);
	}

	// Add to root
	getRoot().getNode().add(entry);
	getRoot().include(original);
};


template<class N, unsigned m>
void BasicRtree<N, m>::splitRoot(const Entry<N>& entry)
{
	addLevel(
			Entry<N>(new N({getRoot(), entry}))
		);
}

}
