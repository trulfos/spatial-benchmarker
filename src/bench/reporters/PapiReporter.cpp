#include "PapiReporter.hpp"
#include "ProgressLogger.hpp"
#include <random>
#include <algorithm>
#include <papi.h>
#include <sched.h>
#include <sys/resource.h>

namespace Bench
{

	/**
	 * Check the PAPI status code given and throw an appropriate exception if
	 * the code is an error.
	 *
	 * @param code Code to check
	 */
	void check(int code)
	{
		if (code != PAPI_OK) {
			throw std::runtime_error(
					std::string("PAPI error! ") + PAPI_strerror(code)
				);
		}
	}

	PapiReporter::PapiReporter(
			const std::string& queryPath,
			unsigned runs,
			const std::vector<std::string>& events
		) : QueryReporter(queryPath), runs(runs), events(events)
	{
	}

	void PapiReporter::run(
			const SpatialIndex& index,
			std::ostream& logStream
		)
	{
		ProgressLogger progress(logStream, runs * REORDER_RUNS);

		// Initialize PAPI and set up event set
		if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
			throw std::runtime_error("Failed to initialize PAPI library");
		}
		
		int eventSet = PAPI_NULL;
		check(PAPI_create_eventset(&eventSet));

		for (auto event : events) {
			// Find event code
			int code;
			int ret = PAPI_event_name_to_code(
						const_cast<char *>(event.c_str()),
						&code
					);
			if (ret != PAPI_OK) {
				throw std::runtime_error("Error when getting code for " + event);
			}
			check(PAPI_add_event(eventSet, code));
		}

		// Load queries into memory
		auto querySet = getQuerySet();

		std::vector<RangeQuery> originalQueries (querySet.getSize());
		std::copy(
				querySet.begin(), querySet.end(),
				originalQueries.begin()
			);

		Results r;
		r.reserve(MIN_RESULT_SIZE);

		// Make space for results
		for (unsigned j = 0; j < runs; j++) {
			std::default_random_engine engine (11);

			// Make a copy that can be reshuffeled
			std::vector<RangeQuery> queries = originalQueries;

			std::vector<int long long> totals (events.size());
			std::vector<int long long> results (events.size());
			int long long runtime = 0;
			int long long virtRuntime = 0;
			int long long switches = 0;
			int long long pageFaults = 0;


			// Test multiple times
			for (unsigned i = 0; i < REORDER_RUNS; ++i) {

				// Clear cache
				clearCache();

				rusage startUsage, endUsage;

				// Start measurements
				if (getrusage(RUSAGE_SELF, &startUsage)) {
					throw std::runtime_error("Could not get usage info");
				}

				check(PAPI_start(eventSet));
				int long long startTime = PAPI_get_real_nsec();
				int long long virtStartTime = PAPI_get_virt_nsec();

				// Run the code to 
				for (const RangeQuery& query : queries) {
					r.clear();
					index.search(r, query);
				}

				// Stop measurements
				int long long endTime = PAPI_get_real_nsec();
				int long long virtEndTime = PAPI_get_virt_nsec();
				check(PAPI_stop(eventSet, results.data()));

				if (getrusage(RUSAGE_SELF, &endUsage)) {
					throw std::runtime_error("Could not get usage info :(");
				}

				// Sum up
				for (unsigned k = 0; k < results.size(); k++) {
					totals[k] += results[k];
				}


				runtime += endTime - startTime;
				virtRuntime += virtEndTime - virtStartTime;
				switches += endUsage.ru_nivcsw - startUsage.ru_nivcsw;
				switches += endUsage.ru_nvcsw - startUsage.ru_nvcsw;
				pageFaults += endUsage.ru_majflt - startUsage.ru_majflt;

				// Rearrange queries
				std::shuffle(
						queries.begin(), queries.end(),
						engine
					);

				// Update progress bar
				progress.increment();
			}

			// Store metrics
			for (unsigned i = 0; i < events.size(); ++i) {
				addEntry(std::string(events[i]), totals[i] / REORDER_RUNS);
			}

			addEntry("PAPI_REAL_NSEC", runtime / REORDER_RUNS);
			addEntry("PAPI_VIRT_NSEC", virtRuntime / REORDER_RUNS);
			addEntry("PAPI_CTX_SW", switches);
			addEntry("PAPI_PG_FLT", pageFaults);

		}

		PAPI_shutdown();
	}

}
