#pragma once
#include "BaseNode.hpp"


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
	class PointerArrayNode : public BaseNode<PointerArrayNode<D, C, P>, C, P>
	{
		using Base = BaseNode<PointerArrayNode<D, C, P>, C, P>;

		public:
			using Mbr = ::Rtree::Mbr<D>;
			using Link = ::Rtree::Link<PointerArrayNode>;
			using Plugin = P;

			// Inherit constructor and operator=
			using Base::Base;
			using Base::operator=;

			// Depends on template parameters
			using Base::getSize;


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
							index < node->getSize() &&
							!node->mbrs[index].intersects(*mbr)
						) {
							++index;
						}
					}
			};


			/**
			 * Scan node and return set of matching entries.
			 */
			std::pair<ScanIterator, ScanIterator> scan(const Mbr& mbr) const
			{
				return std::make_pair(
						ScanIterator(this, &mbr, 0),
						ScanIterator(this, nullptr, getSize())
					);
			}
			


			/**
			 * Get the plugin of an entry in this node.
			 *
			 * @param index Index of entry
			 * @return Plugin of entry at the given index
			 */
			Plugin getPlugin(unsigned index) const
			{
				return plugins[index];
			}


			/**
			 * Get the link of an entry in this node.
			 *
			 * @param index Index of entry
			 * @return Link of entry at the given index
			 */
			Link getLink(unsigned index) const
			{
				return links[index];
			}


			/**
			 * Get the MBR of an entry in this node.
			 *
			 * @param index Index of entry
			 * @return MBR of entry at the given index
			 */
			Mbr getMbr(unsigned index) const
			{
				return mbrs[index];
			}


			/**
			 * Set the MBR of an entry in this node.
			 *
			 * @param index Index of entry for which to set MBR
			 */
			void setMbr(unsigned index, const Mbr& m)
			{
				mbrs[index] = m;
			}


			/**
			 * Set the link of an entry in this node.
			 *
			 * @param index Index of entry for which to set link
			 */
			void setLink(unsigned index, const Link& l)
			{
				links[index] = l;
			}


			/**
			 * Set the plugin of an entry in this node.
			 *
			 * @param index Index of entry for which to set plugin
			 */
			void setPlugin(unsigned index, const Plugin& p)
			{
				plugins[index] = p;
			}

		private:
			Mbr mbrs[C];
			Link links[C];
			Plugin plugins[C];
	};

}
