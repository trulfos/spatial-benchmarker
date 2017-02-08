#pragma once
#include <string>
#include <memory>
#include <dlfcn.h>

/**
 * An shared pointer to an object constructed using the `create` method of a
 * dynamic library. The `destroy` method in the library is called when the
 * object should be destroyed. This class also handles the loading and unloading
 * of the dynamic library.
 *
 * @tparam T base type returned by create metode
 */
template<class T>
class DynamicObject : public std::shared_ptr<T>
{
	public:

		/**
		 * Create a new dynamic object using the `create` method in the given
		 * library.
		 *
		 * @param name Library name
		 */
		DynamicObject(const std::string& name)
		{
			// Open library
			void * library = dlopen(name.c_str(), RTLD_NOW); //TODO: Flags?

			// Handle errors
			if (library == nullptr) {
				const char * error = dlerror();

				if (error != nullptr) {
					throw std::runtime_error(
							"Could not open shared library " + name +
							". Message: " + std::string(error)
						);
				} else {
					throw std::runtime_error(
							"Could not open shared library " + name +
							". No error message."
						);
				}
			}

			// Fetch methods
			Create create = locateMethod<Create>(library, "create");
			Destroy destroy = locateMethod<Destroy>(library, "create");

			// Set pointer and delete function
			this->reset(
					create(),
					[destroy, library](T * o) {
						destroy(o);
						dlclose(library);
					}
				);
		};

	private:
		using Create = T * (*)();
		using Destroy = void (*)(T *);

		/**
		 * Locates a symbol and returns the pointer.
		 * This method also handles error checking.
		 */
		template<class M>
		M locateMethod(void * library, const std::string& name)
		{
			// Clear any previous errors
			dlerror();

			// Request pointer
			M func = reinterpret_cast<M>(dlsym(library, name.c_str()));

			// Check for errors
			const char * error = dlerror();

			if (error != nullptr) {
				throw std::runtime_error(
						"Could not locate method " + name + "\nError:" +
						std::string(error)
					);
			}

			return func;
		};
};
