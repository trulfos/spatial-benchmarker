#pragma once
#include <cassert>
#include <vector>
#include "SpatialIndex.hpp"

namespace Rtree
{

class QuadraticInsertStrategy
{
	public:

		/**
		 * Insert an entry in the tree.
		 *
		 * @param object DataObject to insert
		 */
		template<class I>
		static void insert(I& index, const typename I::E& entry)
		{
			using E = typename I::E;

			E rootEntry (index.getRoot(), typename E::M());
			std::vector<E *> path {&rootEntry};

			// Find leaf node
			for (unsigned i = 0; i < index.getHeight() - 1; i++) {
				E& e = leastEnlargement(*path.back(), entry.mbr);
				e.mbr += entry.mbr;
				path.push_back(&e);
			}

			// Split nodes bottom-up as long as necessary
			E e = entry;
			auto top = path.rbegin();

			while (top != path.rend() && (*(top))->node->isFull()) {
				e = E(index.allocateNode(), {e});
				redistribute(**top, e);
				top++;
			}

			// Split root?
			if (top == path.rend()) {
				E newRoot (index.allocateNode(), {**path.begin(), e});
				index.addLevel(newRoot.node);
			} else {
				(*top)->add(e);
			}
		};


	private:


		/**
		 * Find the child requiring the least MBR enlargement to include the given
		 * MBR.
		 *
		 * @param mbr MBR to include
		 * @return Entry requiring the least enlargement to include mbr
		 */
		template<class E, class M>
		static E& leastEnlargement(E& parent, M& mbr)
		{
			//TODO: Drag the algorithm out into a template?
			E * best = parent.node->entries;
			float minimum = std::numeric_limits<float>::infinity();

			for (E& entry : *(parent.node)) {
				float enlargement = entry.mbr.enlargement(mbr);

				if (enlargement < minimum) {
					minimum = enlargement,
					best = &entry;
				}
			}

			return *best;
		}



		/**
		 * Redistribute the children of the two entries between the entries.
		 *
		 * @param a The first entry (with children)
		 * @param b The second entry (with children)
		 */
		template<class E>
		static void redistribute(E& a, E& b)
		{
			// Contruct buffer with all entries
			std::vector<E> entries (
					a.node->begin(),
					a.node->end()
				);

			entries.insert(
					entries.end(),
					b.node->begin(),
					b.node->end()
				);


			// Choose the two seeds by checking all combinations
			typename std::vector<E>::iterator seeds[2];
			float wasted = -1.0f;

			for (auto i = entries.begin(); i != entries.end(); ++i) {
				for (auto j = i + 1; j != entries.end(); ++j) {

					float waste = (i->mbr + j->mbr).volume() - (
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

			const unsigned m = E::capacity / 2;

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
