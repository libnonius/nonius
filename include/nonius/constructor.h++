#include <type_traits>

namespace nonius {
	namespace detail {
		template <typename T, bool destructUnderlying>
		struct object_storage
		{
			typedef typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type TStorage;

			object_storage() : data() {}

			object_storage(const object_storage& other)
			{
				new(&data) T(other.stored_object());
			}

			object_storage(object_storage&& other)
			{
				new(&data) T(std::move(other.stored_object()));
			}

			~object_storage() { destructOnExit<T>(); }

			template <typename... Args>
			void construct(Args&&... args)
			{
				new (&data) T(std::forward<Args>(args)...);
			}

			template <bool allowManualDestruct = !destructUnderlying>
			typename std::enable_if<allowManualDestruct>::type destruct()
			{
				stored_object().~T();
			}

		private:
			// If this is a constructor benchmark, destroy the underlying object
			template <typename U>
			void destructOnExit(typename std::enable_if<destructUnderlying, U>::type* = 0) { destruct<true>(); }
			// Otherwise, don't
			template <typename U>
			void destructOnExit(typename std::enable_if<!destructUnderlying, U>::type* = 0) { }

			T& stored_object()
			{
				return *static_cast<T*>(static_cast<void*>(&data));
			}

			TStorage data;
		};
	}

	template <typename T>
	using storage_for = detail::object_storage<typename T, true>;

	template <typename T>
	using destructable_object = detail::object_storage<typename T, false>;
}
