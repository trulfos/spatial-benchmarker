#pragma once
#include "BaseNode.hpp"
#include "spatial/Coordinate.hpp"
#include "immintrin.h"

namespace Rtree
{

	/**
	 * Represents a vectorized node where an arrays of subfields approach has
	 * been combined with vectorized scans (and full node scans).
	 *
	 * @tparam D Dimension
	 * @tparam C Max capacity
	 * @tparam P Plugin
	 */
	template<unsigned D, unsigned C, class P = EntryPlugin>
	class FullScanNode : public BaseNode<FullScanNode<D, C, P>, C, P>
	{
		using Base = BaseNode<FullScanNode<D, C, P>, C, P>;
		using Coordinate = Spatial::Coordinate;


		static_assert(
				std::is_same<Coordinate, double>::value,
				"Vector node assumes doubles"
			);

		static constexpr unsigned BLOCK_SIZE = 4;
		static constexpr unsigned N_BLOCKS = (C + BLOCK_SIZE - 1) / BLOCK_SIZE;


		public:
			using Mbr = ::Rtree::Mbr<D>;
			using Link = ::Rtree::Link<FullScanNode>;
			using Plugin = P;

			// Inherit constructor and operator=
			using Base::Base;
			using Base::operator=;

			// Depends on template parameters
			using Base::getSize;


			class ScanIterator
			{
				// Bit set block type
				using BS = unsigned;

				// Number of bits in each bit set block
				static constexpr unsigned bss = 8 * sizeof(BS);

				public:
					using iterator_category = std::input_iterator_tag;
					using value_type = Link;
					using difference_type = int;
					using pointer = const Link *;
					using reference = const Link&;

					ScanIterator() = default;

					/**
					 * Construct a new scan iterator starting at the given
					 * index.
					 */
					ScanIterator(
							const FullScanNode * node,
							const Mbr& mbr,
							unsigned index
						) : node(node), index(index)
					{
						if (index >= node->getSize()) {
							return;
						}

						// The other cases are not currently implemented
						assert(index == 0);

						// Initialize bitset
						for (unsigned i = 0; i < bitset.size(); ++i) {
							bitset[i] = ~0;
						}

						const double * base = reinterpret_cast<const double *>(
								node->coordinates
							);

						auto highs = mbr.getTop();
						auto lows = mbr.getBottom();
						unsigned blocks = (node->getSize() + BLOCK_SIZE - 1) / BLOCK_SIZE;

						// Compare across all dimensions
						for (unsigned d = 0; d < D; ++d) {
							// Check top of query
							scanStrip<_CMP_LE_OS>(
									base + 2 * d * BLOCK_SIZE * N_BLOCKS,
									&highs[d],
									blocks
								);

							// Check bottom of query
							scanStrip<_CMP_GE_OS>(
									base + (2 * d + 1) * BLOCK_SIZE * N_BLOCKS,
									&lows[d],
									blocks
								);
						}

						findNext();
					};

					ScanIterator& operator++()
					{
						assert(index < node->getSize());

						findNext();

						return *this;
					}

					ScanIterator operator++(int)
					{
						ScanIterator it = *this;
						operator++();
						return it;
					}

					reference operator*()
					{
						assert(index < node->getSize());
						return node->links[index];
					}

					pointer operator->()
					{
						return &operator*();
					}

					bool operator==(const ScanIterator& other) const
					{
						assert(other.node == node);
						return other.index == index;
					}

					bool operator!=(const ScanIterator& other) const
					{
						return !(*this == other);
					}

				private:
					const FullScanNode * node;;

					// Current index (as seen from the outside)
					unsigned index;

					// Entries intersecting query
					std::array<BS, (C + 63)/bss> bitset;


					/**
					 * Scans through the bitset until the next matching entry is
					 * found.
					 *
					 * Scan forward through the bitset and locate the next set
					 * bit. Reset the bit and update the index to the location
					 * found.
					 */
					void findNext()
					{
						unsigned block = index / bss;

						while (index < node->getSize()) {

							// Find least significant non-zero bit
							unsigned i = __builtin_ffsll(bitset[block]);

							if (i > 0) {
								i -= 1;
								bitset[block] ^= (1ll << i);
								index = bss * block + i;
								break;
							}

							index = bss * (++block);
						}

						// None found?
						if (index > node->getSize()) {
							index = node->getSize();
						}
					}


					/**
					 * Scan all blocks in a strip and update bitset.
					 *
					 * Scan through all values in a block and compare them
					 * against a reference value using the given operation.
					 *
					 * @tparam OP Compare operation to use (e.g. _CMP_LT_OS)
					 *
					 * @param base Pointer to first value in strip
					 * @param value Pointer to reference value
					 * @param blocks Number of blocks to scan
					 */
					template<unsigned OP>
					void scanStrip(
							const double * base,
							const double * value,
							const unsigned& blocks
						)
					{
						static_assert(bss % BLOCK_SIZE == 0);

						// Load reference value
						__m256d reference = _mm256_broadcast_sd(value);

						unsigned b = 0;

						// Scan entire bit set blocks while possible
						while (BLOCK_SIZE * blocks - b > bss) {
							BS results = 0;

							for (unsigned i = 0; i < bss; i += BLOCK_SIZE) {
								// Load subject value
								__m256d subject = _mm256_load_pd(
										base + b + i
									);

								// Compare and update bitset
								results |= ((BS)_mm256_movemask_pd(
										_mm256_cmp_pd(subject, reference, OP)
									)) << i;
							}

							bitset[b / bss] &= results;
							b += bss;
						}

						// Handle the remaining blocks
						BS results = 0;

						for (unsigned i = 0; i < blocks * BLOCK_SIZE - b; i += BLOCK_SIZE) {
							// Load subject value
							__m256d subject = _mm256_load_pd(
									base + b + i
								);

							// Compare and update bitset
							results |= ((BS) _mm256_movemask_pd(
									_mm256_cmp_pd(subject, reference, OP)
								)) << i;

						}

						bitset[b / bss] &= results;
					}

			};




			/**
			 * Scan node and return set of matching entries.
			 */
			std::pair<ScanIterator, ScanIterator> scan(const Mbr& mbr) const
			{
				return std::make_pair(
						ScanIterator(this, mbr, 0),
						ScanIterator(this, mbr, getSize())
					);
			}


			/**
			 * Override new operator to make sure memory is aligned.
			 */
			void * operator new(std::size_t count)
			{
				void * p;
				int error = posix_memalign(&p, sizeof(__m256d), count);

				// Convert C-style error to exception
				if (error) {
					throw std::bad_alloc();
				}

				//TODO: Remove
				if (reinterpret_cast<uintptr_t>(p) % sizeof(__m256d) != 0) {
					throw std::runtime_error("Unaligned memory :-(");
				}

				return p;
			}


			/**
			 * Override delete operator to use correct delete method.
			 */
			void operator delete (void * pointer)
			{
				free(pointer);
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
				std::array<Coordinate, D> top, bottom;

				auto base = reinterpret_cast<const double *>(coordinates) + index;

				for (unsigned d = 0; d < D; ++d) {
					bottom[d] = base[0];
					top[d] = base[BLOCK_SIZE * N_BLOCKS];
					base += 2 * BLOCK_SIZE * N_BLOCKS;
				}

				return Mbr(top, bottom);
			}


			/**
			 * Set the MBR of an entry in this node.
			 *
			 * @param index Index of entry for which to set MBR
			 */
			void setMbr(unsigned index, const Mbr& m)
			{
				auto top = m.getTop();
				auto bottom = m.getBottom();

				auto base = reinterpret_cast<double *>(coordinates) + index;

				for (unsigned d = 0; d < D; ++d) {
					base[0] = bottom[d];
					base[BLOCK_SIZE * N_BLOCKS] = top[d];
					base += 2 * BLOCK_SIZE * N_BLOCKS;
				}
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
			__m256d coordinates[N_BLOCKS * 2 * D];
			Link links[C];
			Plugin plugins[C];
	};

}
