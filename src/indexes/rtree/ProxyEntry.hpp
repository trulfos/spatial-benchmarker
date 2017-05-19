#pragma once
#include "Entry.hpp"
#include "ProxyIterator.hpp"

namespace Rtree
{

	/**
	 * A proxy entry delegates the calls to getters and setters to the node
	 * itself and works like a reference to an entry. Thus assigning to a proxy
	 * entry changes the node, but leaves the actual fields of the proxy entry
	 * intact.
	 *
	 * @tparam Node Type of the node the entry belongs to
	 *
	 * @see Rtree::BaseEntry
	 */
	template<class Node>
	class ProxyEntry : public BaseEntry<ProxyEntry<Node>, Node>
	{
		using Base = BaseEntry<ProxyEntry<Node>, Node>;

		friend Base;

		public:

			using Mbr = typename Base::Mbr;
			using Link = typename Base::Link;
			using Plugin = typename Base::Plugin;

			// Allows default constructing proxy iterators
			ProxyEntry() = default;


			/**
			 * Create a proxy entry based on the node and the index of
			 * the entry within the node.
			 *
			 * @param node Node in which the entry resides
			 * @param index Index of entry in node
			 */
			ProxyEntry(Node * node, unsigned index)
				: node(node), index(index)
			{
			}


			/**
			 * Override the default assign operator to provide reference
			 * semantics.
			 *
			 * @param other Another ProxyEntry to copy data from
			 */
			ProxyEntry& operator=(const ProxyEntry& other)
			{
				//TODO: Why doesn't the below template handle this?
				setMbr(other.getMbr());
				setLink(other.getLink());
				setPlugin(other.getPlugin());
				return *this;
			}

			/**
			 * Update the node with the data of the other entry.
			 *
			 * Since a proxy entry works like a reference, this actually updates
			 * the data in the node with data from the given entry. In other
			 * words, this proxy entry is actually not changed at all.
			 *
			 * @param other Other entry to copy data from
			 * @return Reference to this proxy entry
			 */
			template<class E>
			ProxyEntry& operator=(const E& other)
			{
				setMbr(other.getMbr());
				setLink(other.getLink());
				setPlugin(other.getPlugin());

				return *this;
			}

			/**
			 * Make sure proxy nodes are swappable
			 *
			 * This swaps what the proxies refers to and not the fields
			 * of the proxies themselves.
			 */
			friend void swap(ProxyEntry& a, ProxyEntry& b)
			{
				Entry<Node> tmp;
				tmp = a;
				a = b;
				b = tmp;
			}

			const Mbr getMbr() const
			{
				return node->getMbr(index);
			}


			const Link getLink() const
			{
				return node->getLink(index);
			}

			const Plugin getPlugin() const
			{
				return node->getPlugin(index);
			}

		protected:
			void setMbr(const Mbr& m)
			{
				node->setMbr(index, m);
			}

			void setPlugin(const Plugin& p)
			{
				node->setPlugin(index, p);
			}

			void setLink(const Link& l)
			{
				node->setLink(index, l);
			}


		private:
			Node * node;
			unsigned index;

			friend ProxyIterator<Node>;
			friend ConstProxyIterator<Node>;
	};

};
