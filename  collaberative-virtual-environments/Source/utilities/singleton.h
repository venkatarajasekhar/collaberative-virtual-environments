#ifndef SINGLETON_H
#define SINGLETON_H

/**
 * A singleton template class used when you only need to ever have one of that
 * class running.  Useful for things like texture or settings managers.  
 * The user is responsible for creating and destroying the singleton by calling
 * new someClass() and delete someClass::getSingletonPtr() sets pointer back
 * to 0 so we can recreate it again later on if we want.
 */
template<typename T>
class Singleton
{
	static T* ms_singleton;
	protected:

		/// Constructor
		Singleton()
		{
			//assert(!ms_singleton);
			ms_singleton = static_cast<T*>(this);
		}

		/// Destructor
		~Singleton()
		{
			//assert(ms_singleton);
			ms_singleton=0;
		}

	public:
		/// Used to create singleton
		static inline void Create()
		{
			if(ms_singleton)return;
			new T();
		}
		/// Used to return the singleton object
		static inline T& GetSingleton()
		{
//			assert(ms_singleton);
			return *ms_singleton;
		}
		/// Used to return the singleton pointer
		static inline T* GetSingletonPtr()
		{
	//		assert(ms_singleton);
			return ms_singleton;
		}
		/// Used to destroy the singleton object and reset pointer to 0
		static inline void Destroy()
		{
			if(ms_singleton)delete ms_singleton;
			ms_singleton=0;
		}
};

template <typename T> T* Singleton <T>::ms_singleton = 0;

#define SINGLETON_BEGIN(class_name) \
	class class_name : public Singleton<class_name> { \
		friend class Singleton<class_name>;

#define SINGLETON_END() };

#define SINGLETON_GET(class_name, var) \
	class_name& var = class_name::GetSingleton();


#endif