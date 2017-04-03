#pragma once
#include "common/DataObject.hpp"
#include "Mbr.hpp"
#include "EntryPlugin.hpp"
#include <algorithm>

namespace Rtree
{

/**
 * An entry in a node of the R-tree.
 *
 * Each entry works like a handle - it contains the MBR of a node and a pointer
 * to the node itself. The MBR is separated from the node to avoid loading the
 * an entire node when only the MBRs should be scanned.
 *
 * @tparam D Dimension
 * @tparam N Node type
 * @tparam An entry plugin
 */
template<
		unsigned D,
		class N,
		template<class> class P = EntryPlugin
	>
class Entry
{
	public:
		using Id = DataObject::Id;
		using M = Mbr<D>;
		using Node = N;
		using Plugin = P<Entry>;

		using iterator = Entry *;
		using const_iterator = const Entry *;

		static constexpr unsigned dimension = D;

		/**
		 * Default constructor.
		 */
		Entry() : plugin(*this)
		{
		}


		/**
		 * Create a new entry and initialize MBR and node with the given
		 * entries.
		 *
		 * @param node Pointer to initial node
		 * @param entries Initial entries
		 */
		Entry(N * node, std::initializer_list<Entry> entries)
			: node(node), plugin(*this)
		{
			assign(entries.begin(), entries.end());
		}


		/**
		 * Create a new entry from the given object, optionally passing a set of
		 * arguments to the plugin.
		 */
		template<class ...Args>
		Entry(const DataObject& object, Args ...args)
			: id(object.getId())
			, mbr(object.getBox())
			, plugin(*this, object, std::forward<Args...>(args...))
		{
		};

		Entry(const DataObject& object)
			: id(object.getId())
			, mbr(object.getBox())
			, plugin(*this, object)
		{
		};


		/**
		 * Retrieve the MBR of this entry.
		 *
		 * @return MBR of this entry.
		 */
		const M& getMbr() const
		{
			return mbr;
		}

		M& getMbr()
		{
			return mbr;
		}


		/**
		 * Retrieve the ID of this entry.
		 */
		const Id& getId() const
		{
			return id;
		}

		Id& getId()
		{
			return id;
		}


		/**
		 * Retrieve the node of this entry
		 */
		const Node * getNode() const
		{
			return node;
		}

		Node * getNode()
		{
			return node;
		}

		
		/**
		 * Add an entry as a child of this entry.
		 * The entry will be added to the child node, and the MBR updated.
		 *
		 * @param entry Entry to add
		 */
		void add(const Entry& entry)
		{
			// Add and update MBR
			node->add(entry);
			mbr += entry.mbr;

			// Inform plugin
			plugin.include(*this, entry);
		};


		/**
		 * Inculdes the given entry in this entry without adding it to the node.
		 *
		 * This typically updates the MBR and plugin. Useful when adjusting MBRs
		 * in the tree during insertion.
		 */
		void include(const Entry& entry)
		{
			mbr += entry.mbr;
			plugin.include(*this, entry);
		}


		/**
		 * Add range of entries entries.
		 *
		 * @param first Iterator to first
		 * @param last Iterator past the end
		 */
		template<class FIt>
		void add(FIt first, FIt last)
		{
			std::for_each(
					first, last,
					[&](const Entry& entry) {
						add(entry);
					}
				);
		}


		/**
		 * Remove all entries and add from the given iterators. This also fixes
		 * the MBR.
		 *
		 * @param start First element
		 * @param end Element beyond the last
		 */
		template<class ForwardIterator>
		void assign(ForwardIterator start, ForwardIterator end)
		{
			if (start == end) {
				throw std::logic_error(
						"Entry reset needs at least one child. None given!"
					);
			}

			// Reset fields
			mbr = start->mbr;
			node->reset();
			plugin = Plugin(*this);

			// Add entries
			for (;start != end; ++start) {
				add(*start);
			}

			plugin.init(*this);
		};


		/**
		 * Remove all entries and add the given initializer list. This also
		 * fixes the MBR.
		 */
		Entry& operator=(std::initializer_list<Entry> entries)
		{
			assign(entries.begin(), entries.end());
			return *this;
		};


		/**
		 * Recalculates aggergate data for this entry.
		 *
		 * This mainly constists of the entry MBR, but may also include plugin
		 * information.
		 */
		void recalculate()
		{
			assert(node->size() > 0);

			mbr = node->begin()[0].mbr;
			plugin = Plugin(*this);

			for (const Entry& e : *this) {
				include(e);
			}
		}


		/**
		 * Return first entry in this entry's node.
		 */
		iterator begin()
		{
			return node->begin();
		};

		/**
		 * Return the entry past the last entry in this entry's node.
		 */
		iterator end()
		{
			return node->end();
		};


		const_iterator begin() const
		{
			return node->begin();
		};

		const_iterator end() const
		{
			return node->end();
		};

		/**
		 * Access the plugin of this node.
		 */
		const Plugin& getPlugin() const
		{
			return plugin;
		}

	private:
		union {
			Id id;
			N * node;
		};
		M mbr;
		Plugin plugin;

};

}
