#pragma once
#include "../../common/DataObject.hpp"
#include "Mbr.hpp"
#include <cassert>

namespace Rtree
{

template<unsigned D, unsigned C>
class Node;

template<unsigned D, unsigned C>
class Entry
{
	public:
		using Id = DataObject::Id;
		using M = Mbr<D>;
		using N = Node<D, C>;

		union { //TODO: This may not be very clever (64bit vs 32bit)?
			Id id;
			N * node;
		};

		M mbr;

		/**
		 * Default constructor.
		 */
		Entry() = default;

		/**
		 * Create a new entry from a node.
		 */
		Entry(N * node, M mbr) : node(node), mbr(mbr)
		{
		};

		/**
		 * Create a new entry from the given object.
		 */
		Entry(const DataObject& object)
		{
			id = object.getId();
			mbr = object.getPoint();
		};

		
		/**
		 * Add an entry as a child of this entry.
		 * The entry will be added to the child node, and the MBR updated.
		 *
		 * @param entry Entry to add
		 */
		void add(const Entry& entry)
		{
			node->add(entry);
			mbr += entry.mbr;
		}


		/**
		 * Split this entry and include the given entry.
		 *
		 * @return A new entry
		 */
		Entry split(const Entry& e, N * newNode)
		{
			if (!node->isFull()) {
				throw std::logic_error("Node not full when splitting");
			}

			Entry other (newNode, M());

			// Contruct buffer with all entries
			std::vector<Entry> entries (node->entries, node->entries + C);
			entries.push_back(e);


			// Choose the two first by checking all combinations
			struct {
				typename std::vector<Entry>::iterator entries[2];
				float wasted = -1.0f;
			} best;

			for (auto i = entries.begin(); i != entries.end(); ++i) {
				for (auto j = i + 1; j != entries.end(); ++j) {

					float waste = (i->mbr + j->mbr).volume() - (
							i->mbr.volume() + j->mbr.volume()
						);

					if (waste > best.wasted) {
						best.entries[0] = i;
						best.entries[1] = j;
						best.wasted = waste;
					}
				}
			}

			assert(best.wasted > -0.1f);

			// Distribute the remaning entries
			mbr = best.entries[0]->mbr;
			other.mbr = best.entries[1]->mbr;

			node->reset(*best.entries[0]);
			newNode->reset(*best.entries[1]);

			for (auto e = entries.begin(); e != entries.end(); ++e) {
				if (best.entries[0] == e || best.entries[1] == e) {
					continue;
				}

				if (
					newNode->nEntries >= C/2 ||
					(mbr.enlargement(e->mbr) < other.mbr.enlargement(e->mbr)
						&& node->nEntries < C/2)
				) {
					add(*e);
				} else {
					other.add(*e);
				}
			}


			return other;
		};
};

}
