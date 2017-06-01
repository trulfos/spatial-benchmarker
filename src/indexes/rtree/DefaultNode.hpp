#pragma once
#include "BaseNode.hpp"
#include "ProxyScanIterator.hpp"


namespace Rtree
{
	/**
	 * Node where the fields of each entry is stored together in the naive way.
	 *
	 * @tparam D Dimension
	 * @tparam C Max capacity
	 * @tparam P Plugin
	 */
	template<unsigned D, unsigned C, class P = EntryPlugin>
	class DefaultNode : public BaseNode<DefaultNode<D, C, P>, C, P>
	{
		using Base = BaseNode<DefaultNode<D, C, P>, C, P>;

		public:
			using Mbr = ::Rtree::Mbr<D>;
			using Link = ::Rtree::Link<DefaultNode>;
			using Plugin = P;

			// Inherit constructor and operator=
			using Base::Base;
			using Base::operator=;

			// Depends on template parameters
			using Base::getSize;

			/**
			 * Iterator running through links in this node matching a query.
			 */
			class ScanIterator : public ProxyScanIterator<DefaultNode>
			{
				using Base = ProxyScanIterator<DefaultNode>;
				using Base::entry;

				public:
					/**
					 * Construct a singular iterator.
					 */
					ScanIterator() = default;

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
							const DefaultNode * node,
							const Mbr * mbr,
							unsigned index
						) : Base(node, index), mbr(mbr)
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
						++entry.index;
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

				private:
					const Mbr * mbr;

					/**
					 * Finds the first position intersecting with the MBR.
					 *
					 * This includes the current position if it intersects with
					 * the MBR. `i` is updated to the found position.
					 */
					void findNext()
					{
						unsigned& index = entry.index;
						const DefaultNode * node = entry.node;
						while (
							index < node->getSize() &&
							!node->entries[index].mbr.intersects(*mbr)
						) {
							++index;
						}
					}
			};


			/**
			 * Scan node and return set of matching entries.
			 */
			template<class E>
			std::pair<ScanIterator, ScanIterator> scan(
					const Mbr& mbr,
					const E&
				) const
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
				return entries[index].plugin;
			}


			/**
			 * Get the link of an entry in this node.
			 *
			 * @param index Index of entry
			 * @return Link of entry at the given index
			 */
			Link getLink(unsigned index) const
			{
				return entries[index].link;
			}


			/**
			 * Get the MBR of an entry in this node.
			 *
			 * @param index Index of entry
			 * @return MBR of entry at the given index
			 */
			Mbr getMbr(unsigned index) const
			{
				return entries[index].mbr;
			}


			/**
			 * Set the MBR of an entry in this node.
			 *
			 * @param index Index of entry for which to set MBR
			 */
			void setMbr(unsigned index, const Mbr& m)
			{
				entries[index].mbr = m;
			}


			/**
			 * Set the link of an entry in this node.
			 *
			 * @param index Index of entry for which to set link
			 */
			void setLink(unsigned index, const Link& l)
			{
				entries[index].link = l;
			}


			/**
			 * Set the plugin of an entry in this node.
			 *
			 * @param index Index of entry for which to set plugin
			 */
			void setPlugin(unsigned index, const Plugin& p)
			{
				entries[index].plugin = p;
			}


		private:
			/**
			 * Simple struct for storing MBR, link and plugin in the same place.
			 */
			struct SimpleEntry
			{
				Mbr mbr;
				Link link;
				Plugin plugin;
			};

			SimpleEntry entries[C];

	};

}
