#pragma once
#include <initializer_list>
#include "Link.hpp"
#include "Mbr.hpp"
#include "Entry.hpp"
#include "ProxyEntry.hpp"
#include "ProxyIterator.hpp"


namespace Rtree
{

	/**
	 * Contains the common functionality of nodes, but leaves the storage
	 * layout to be defined by subclasses.
	 *
	 * @tparam Node Subclass type for static inheritance
	 * @tparam C Node capacity
	 */
	template<class Node, unsigned C, class P>
	class BaseNode
	{
		public:
			using Plugin = P;
			using iterator = ProxyIterator<Node>;
			using const_iterator = ConstProxyIterator<Node>;
			using reference = ProxyEntry<Node>;
			using value_type = Entry<Node>;


			/**
			 * This nodes capacity.
			 */
			static constexpr unsigned capacity = C;


			/**
			 * Construct an empty node.
			 */
			BaseNode() : size(0)
			{
			}


			/**
			 * Construct a node with a given set of entries.
			 *
			 * @tparam E Entry type
			 * @param entries Set of entries to include
			 */
			template<class E>
			BaseNode(const std::initializer_list<E>& entries)
			{
				assign(entries.begin(), entries.end());
			}


			/**
			 * Get a proxy entry to given index.
			 *
			 * @param i Index of entry to retrieve
			 * @return Reference like proxy to the entry at the given index
			 */
			reference operator[](unsigned i)
			{
				assert(i < size);
				return reference(static_cast<Node *>(this), i);
			}


			/**
			 * Const overload.
			 *
			 * @overload operator[]
			 */
			value_type operator[](unsigned i) const
			{
				assert(i < size);
				auto node = static_cast<const Node *>(this);

				return value_type(
						node->getMbr(i),
						node->getLink(i),
						node->getPlugin(i)
					);
			}


			/**
			 * Add entry to the end of this node.
			 */
			void add(const value_type& entry)
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
			Node& operator=(const std::initializer_list<E>& entries)
			{
				assign(entries.begin(), entries.end());

				return *static_cast<Node *>(this);
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
				return iterator(static_cast<Node*>(this), 0);
			}


			/**
			 * Iterator to past-the-end.
			 */
			iterator end()
			{
				return iterator(static_cast<Node *>(this), size);
			}

			/**
			 * Begin iteration through constant node.
			 */
			const_iterator begin() const
			{
				return const_iterator(static_cast<const Node *>(this), 0);
			}

			/**
			 * Past-the-end const iterator.
			 */
			const_iterator end() const
			{
				return const_iterator(static_cast<const Node *>(this), size);
			}

		private:
			unsigned size;
			typename Plugin::NodeData data;

			friend Plugin;
	};
}
