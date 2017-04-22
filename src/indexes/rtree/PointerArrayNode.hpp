#pragma once
#include "Link.hpp"
#include "Mbr.hpp"
#include "Entry.hpp"
#include "EntryPlugin.hpp"
#include "ProxyIterator.hpp"


namespace Rtree
{
	/**
	 * Node where the plugins, MBRs and links are stored in separate arrays.
	 *
	 * The hope is that this will increase performance because less unused data
	 * will be loaded.
	 *
	 * @tparam D Dimension
	 * @tparam C Max capacity
	 * @tparam P Plugin
	 */
	template<unsigned D, unsigned C, class P = EntryPlugin>
	class PointerArrayNode
	{
		public:
			// Type definitions
			using Mbr = ::Rtree::Mbr<D>;
			using Link = ::Rtree::Link<PointerArrayNode>;
			using Plugin = P;
			using Entry = ::Rtree::Entry<PointerArrayNode>;


			// Constants
			static constexpr unsigned capacity = C;

			/**
			 * Proxy entry for the default node class.
			 *
			 * @see Rtree::BaseEntry
			 */
			class ProxyEntry : public BaseEntry<PointerArrayNode>
			{
				using Base = BaseEntry<PointerArrayNode>;

				public:
					// Types
					using Mbr = typename Base::Mbr;
					using Link = typename Base::Link;
					using Plugin = typename Base::Plugin;

					// Allows default constructing the iterator
					ProxyEntry() = default;


					/**
					 * Create a proxy entry based on the node and the index of
					 * the entry within the node.
					 *
					 * @param node Node in which the entry resides
					 * @param index Index of entry in node
					 */
					ProxyEntry(PointerArrayNode * node, unsigned index)
						: node(node), index(index)
					{
					}

					ProxyEntry& operator=(const ProxyEntry& other)
					{
						//TODO: Why doesn't the below template handle this?
						setMbr(other.getMbr());
						setLink(other.getLink());
						setPlugin(other.getPlugin());
						return *this;
					}

					template<class E>
					ProxyEntry& operator=(const E& other)
					{
						setMbr(other.getMbr());
						setLink(other.getLink());
						setPlugin(other.getPlugin());

						return *this;
					}

					const Mbr getMbr() const override
					{
						return node->mbrs[index];
					}


					const Link getLink() const override
					{
						return node->links[index];
					}


					const Plugin getPlugin() const override
					{
						return node->plugins[index];
					}

					/**
					 * Make sure proxy nodes are swappable
					 *
					 * This swaps what the proxies refers to and not the fields
					 * of the proxies themselves.
					 */
					friend void swap(ProxyEntry& a, ProxyEntry& b)
					{
						Entry tmp;
						tmp = a;
						a = b;
						b = tmp;
					}

				protected:
					void setMbr(const Mbr& m) override
					{
						node->mbrs[index] = m;
					}

					void setPlugin(const Plugin& p) override
					{
						node->plugins[index] = p;
					}

					void setLink(const Link& l) override
					{
						node->links[index] = l;
					}

				private:
					PointerArrayNode * node;
					unsigned index;

					friend ProxyIterator<PointerArrayNode>;
					friend ConstProxyIterator<PointerArrayNode>;
			};

			using iterator = ProxyIterator<PointerArrayNode>;
			using const_iterator = ConstProxyIterator<PointerArrayNode>;
			using reference = ProxyEntry;


			/**
			 * Iterator running through links in this node matching a query.
			 */
			class ScanIterator
				: public std::iterator<std::forward_iterator_tag, const Link>
			{
				using Base = std::iterator<std::forward_iterator_tag, const Link>;

				public:
					using reference = typename Base::reference;
					using pointer = typename Base::pointer;

					/**
					 * Construct a singular iterator.
					 */
					ScanIterator()
					{
					}

					/**
					 * Construct a scan iterator yielding entries overlapping
					 * MBR.
					 *
					 * Note that only a reference to the given MBR is store,
					 * thus it must be available during the lifetime of this
					 * iterator.
					 *
					 * @param node Node to scan through
					 * @param mbr MBR yielded entries must overlap
					 * @param i Index to start at
					 */
					ScanIterator(
							const PointerArrayNode * node,
							const Mbr * mbr,
							unsigned index
						) : node(node), mbr(mbr), index(index)
					{
						findNext();
					};

					/**
					 * Advance to next matching entry.
					 *
					 * This scans through the node until the end is reached or a
					 * new entry overlapping with the MBR is found.
					 *
					 * @return This scan iterator
					 */
					ScanIterator& operator++()
					{
						++index;
						findNext();
						return *this;
					}


					/**
					 * Advance this to the next matching entry.
					 *
					 * Scan through the node until a matching entry is found and
					 * then return the previous value of this.
					 *
					 * @return Previous scan iterator
					 */
					ScanIterator operator++(int)
					{
						ScanIterator it = *this;
						operator++();
						return it;
					}


					/**
					 * Check if this iterator is equal with another.
					 *
					 * This is the case if they are at the same position in the
					 * scan.
					 *
					 * @return True if equal
					 */
					bool operator==(const ScanIterator& other)
					{
						assert(node == other.node);
						return index == other.index;
					}

					/**
					 * Check if two iterators are unequal.
					 *
					 * The opposite of the equal operation, as expected.
					 *
					 * @return True if they are unequal
					 */
					bool operator!=(const ScanIterator& other)
					{
						assert(node == other.node);
						return index != other.index;
					}

					/**
					 * Retreive the link pointed to by this iterator.
					 *
					 * @return Reference to link
					 */
					reference operator*()
					{
						return node->links[index];
					}

					/**
					 * Access the link pointed to by this iterator.
					 */
					pointer operator->()
					{
						return node->links + index;
					}

				private:
					const PointerArrayNode * node;
					const Mbr * mbr;
					unsigned index;

					/**
					 * Finds the first position intersecting with the MBR.
					 *
					 * This includes the current position if it intersects with
					 * the MBR. `i` is updated to the found position.
					 */
					void findNext()
					{
						while (
							index < node->size &&
							!node->mbrs[index].intersects(*mbr)
						) {
							++index;
						}
					}
			};


			/**
			 * Construct an empty node.
			 */
			PointerArrayNode() : size(0)
			{
			}


			/**
			 * Construct a node with a given set of entries.
			 *
			 * @tparam E Entry type
			 * @param entries Set of entries to include
			 */
			template<class E>
			PointerArrayNode(const std::initializer_list<E>& entries)
			{
				assign(entries.begin(), entries.end());
			}


			/**
			 * Get a proxy entry to given index.
			 */
			ProxyEntry operator[](unsigned i)
			{
				assert(i < size);
				return ProxyEntry(this, i);
			}


			Entry operator[](unsigned i) const
			{
				assert(i < size);
				return Entry(
						mbrs[i],
						links[i],
						plugins[i]
					);
			}


			/**
			 * Add entry to end.
			 */
			void add(const Entry& entry)
			{
				assert(size < C);
				operator[](size++) = entry;
			}


			/**
			 * Replace the entries in this with given node.
			 *
			 * @param first Iterator to first element of range
			 * @param last Iterator to past-the-end of the range
			 */
			template<class InputIterator>
			void assign(InputIterator first, InputIterator last)
			{
				size = 0;
				add(first, last);
			}


			template<class InputIterator>
			void add(InputIterator first, InputIterator last)
			{
				while (first != last) {
					add(*first);
					++first;
				}
			}


			/**
			 * Assign from initializer list.
			 */
			template<class E>
			PointerArrayNode& operator=(const std::initializer_list<E>& entries)
			{
				assign(entries.begin(), entries.end());

				return *this;
			}


			/**
			 * Get the number of entries in this node.
			 *
			 * @return Number of entries
			 */
			unsigned getSize() const
			{
				return size;
			}


			/**
			 * Check whether this node is full.
			 *
			 * @return True if full
			 */
			bool isFull() const
			{
				return size == C;
			}


			/**
			 * Begin iteration through all entries.
			 */
			iterator begin()
			{
				return iterator(this, 0);
			}


			/**
			 * Iterator to past-the-end.
			 */
			iterator end()
			{
				return iterator(this, size);
			}

			/**
			 * Begin iteration through constant node.
			 */
			const_iterator begin() const
			{
				return const_iterator(this, 0);
			}

			/**
			 * Past-the-end const iterator.
			 */
			const_iterator end() const
			{
				return const_iterator(this, size);
			}


			/**
			 * Scan node and return set of matching entries.
			 */
			std::pair<ScanIterator, ScanIterator> scan(const Mbr& mbr) const
			{
				return std::make_pair(
						ScanIterator(this, &mbr, 0),
						ScanIterator(this, nullptr, size)
					);
			}
			


		private:
			Mbr mbrs[C];
			Link links[C];
			Plugin plugins[C];
			unsigned size;

			typename Plugin::NodeData data;
			friend Plugin;
	};

}
