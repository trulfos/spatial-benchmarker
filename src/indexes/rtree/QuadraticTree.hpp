#pragma once
#include <cassert>
#include <vector>
#include "Rtree.hpp"
#include "Node.hpp"
#include "Entry.hpp"
#include "common/Algorithm.hpp"

namespace Rtree
{

/**
 * R-tree with quadratic split as suggested by Guttman.
 *
 * @tparam D Dimenson
 * @tparam N Node type
 * @tparam m Minimum node fill grade
 */
template<unsigned D, unsigned C, unsigned m>
class QuadraticRtree : public Rtree<Node<D, C, Entry>>
{
	public:

		using N = Node<D, C, Entry>;
		using E = typename N::Entry;


		/**
		 * Insert an entry in the tree.
		 *
		 * @param object DataObject to insert
		 */
		void insert(const E& entry) override
		{
			E rootEntry (this->getRoot(), typename E::M());
			std::vector<E *> path {&rootEntry};

			// Find leaf node
			for (unsigned i = 0; i < this->getHeight() - 1; i++) {
				E& e = leastVolumeEnlargement(*path.back(), entry);
				e.mbr += entry.mbr;
				path.push_back(&e);
			}

			// Split nodes bottom-up as long as necessary
			E e = entry;
			auto top = path.rbegin();

			while (top != path.rend() && (*(top))->node->isFull()) {
				e = E(this->allocateNode(), {e});
				redistribute(**top, e);
				top++;
			}

			// Split root?
			if (top == path.rend()) {
				E newRoot (this->allocateNode(), {**path.begin(), e});
				this->addLevel(newRoot.node);
			} else {
				(*top)->add(e);
			}
		};


	protected:


		/**
		 * Find the child requiring the least MBR enlargement to include the
		 * given MBR.
		 *
		 * @param mbr MBR to include
		 * @return Entry requiring the least enlargement to include mbr
		 */
		template<class E>
		E& leastVolumeEnlargement(E& parent, const E& newEntry)
		{
			return *argmin(
					parent.begin(),
					parent.end(),
					[&](const E& entry) {
						return entry.mbr.enlargement(newEntry.mbr);
					}
				);
		}



		/**
		 * Redistribute the children of the two entries between the entries.
		 *
		 * @param a The first entry (with children)
		 * @param b The second entry (with children)
		 */
		template<class E>
		void redistribute(E& a, E& b)
		{
			// Contruct buffer with all entries
			std::vector<E> entries (a.begin(), a.end());

			entries.insert(
					entries.end(),
					b.begin(), b.end()
				);


			// Choose the two seeds by checking all combinations
			typename std::vector<E>::iterator seeds[2];
			double wasted = -std::numeric_limits<double>::infinity();

			for (auto i = entries.begin(); i != entries.end(); ++i) {
				for (auto j = i + 1; j != entries.end(); ++j) {

					double waste = (i->mbr + j->mbr).volume() - (
							i->mbr.volume() + j->mbr.volume()
						);

					if (waste > wasted) {
						seeds[0] = i;
						seeds[1] = j;
						wasted = waste;
					}
				}
			}

			// Distribute the remaining entries
			a = {*seeds[0]};
			b = {*seeds[1]};


			for (auto e = entries.begin(); e != entries.end(); ++e) {
				if (seeds[0] == e || seeds[1] == e) {
					continue;
				}

				if (
					b.node->nEntries >= m ||
					(a.mbr.enlargement(e->mbr) < b.mbr.enlargement(e->mbr)
						&& a.node->nEntries < m)
				) {
					a.add(*e);
				} else {
					b.add(*e);
				}
			}
		};
};

}
