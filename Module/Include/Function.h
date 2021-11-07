#pragma once

#include <tuple>
#include <utility>

/*----------------------------------------------------------------
	Fast function container.
----------------------------------------------------------------*/
class CFunction
{
private:

	template 
	<
		typename T
	>
	struct Identity
	{
		typedef T type;
	};

	class CFunctionWrapper
	{
	public:

		virtual inline  ~CFunctionWrapper() {}

	public:

		virtual inline void Run() const = 0;
		virtual inline void operator()() const
		{
			Run();
		}
	};

#ifdef boost::thread
	class CServiceFunction : public CFunctionWrapper
	{
	private:

		boost::detail::thread_data_ptr Context;

	public:

		template
		<
			typename Signature
		>
		inline void Initialize
		(
			Signature&& Bound
		)
		{
			Context = GetContext(boost::thread_detail::decay_copy(boost::forward<Signature>(Bound)));
		}

		template
		<
			typename Signature
		>
		explicit inline CServiceFunction
		(
			Signature&& Bound
		)
		{
			Initialize(Bound);
		}

		template
		<
			class		Return, 
			class ...	Args
		>
		explicit inline CServiceFunction
		(
			Return		Function, 
			Args && ... Arguments
		)
		{
			Initialize(boost::bind(Function, std::forward<Args>(Arguments)...));
		}

		inline virtual ~CServiceFunction() = default;

		template
		<
			class Signature
		>
		explicit CServiceFunction
		(
			boost::thread::attributes&  Attributes, 
			Signature&&					Bound
		) 
		{
			Context = boost::thread_detail::decay_copy(boost::forward<Signature>(GetContext(Bound)));
		}

		template
		<
			typename Signature
		>
		static inline boost::detail::thread_data_ptr GetContext
		(
			Signature&& Bound
		)
		{
			return boost::detail::thread_data_ptr
			(
				boost::detail::heap_new<boost::detail::thread_data<typename boost::remove_reference<Signature>::type> >
				(
					boost::forward<Signature>(Bound)
				)
			);
		}

		virtual inline void Run() const override
		{
			Context->run();
		}
	};
#endif

	template
	<
		typename	 Return, 
		typename ... Args
	>
	class CFunctionHolder : public CFunctionWrapper
	{
	public:

		using TTuple	= std::tuple<Args...>;
		using TFunction = std::function<Return(Args...)>;

		TTuple		Tuple;
		TFunction	Function;

	public:

		inline CFunctionHolder
		(
			TFunction &	Bound,
			Args &&...	Arguments
		) :
			Tuple(std::forward<Args>(Arguments)...), Function(Bound)
		{}

		inline CFunctionHolder
		(
			Return		(*Bound)(Args &&...),
			Args &&...	Arguments
		) :
			Tuple(std::forward<Args>(Arguments)...), Function(Bound)
		{}

		inline CFunctionHolder
		(
			Return*		Bound,
			Args&&...	Arguments
		) :
			Tuple(std::forward<Args>(Arguments)...), Function(Bound)
		{}

	public:

		virtual inline void Run() const override
		{
			std::apply(Function, Tuple);
		}
	};

	template
	<
		typename	 Return, 
		typename	 Class, 
		typename ... Args
	>
	class CMemberFunctionHolder : public CFunctionWrapper
	{
	public:

		using TTuple	= std::tuple<Args...>;
		using TFunction = std::function<Return(Args...)>;

		TTuple		Tuple;
		TFunction	Function;

		class TMemberFunction
		{
		public:
			explicit TMemberFunction
			(
				Return(Class::*_Pm)(Args...)
			)
				: _Pmemfun(_Pm)
			{}

			Return operator()(Class *_Pleft, Args... _Right) const
			{
				return ((_Pleft->*_Pmemfun)(_Right...));
			}

		private:
			Return(Class::*_Pmemfun)(Args...);
		};

	public:

		inline CMemberFunctionHolder
		(
			Return				(Class::*MemberFunction)(Args&&...),
			Class		*		Instance,
			Args		&& ...	Arguments
		) :
			Tuple(std::forward<Args>(Arguments)...),
			Function(std::bind(MemberFunction, Instance, Arguments...))
		{}

	public:

		virtual inline void Run() const override
		{
			std::apply(Function, Tuple);
		}
	};

	CFunctionWrapper* FunctionWrapper;

public:

	~CFunction()
	{
		if (FunctionWrapper != NULL)
		{
			delete FunctionWrapper;
		}
	}

	CFunction()
	{
		FunctionWrapper = NULL;
	}


	/*----------------------------------------------------------------------------------------------------------------------------------
		Constructs a function wrapper with a specific signature. (ForEx: CFunction(Run))
	----------------------------------------------------------------------------------------------------------------------------------*/

	template
	<
		typename Signature
	>
	inline CFunction
	(
		Signature&& Bound
	)
	{
		AssignFunction(Bound);
	}


	/*----------------------------------------------------------------------------------------------------------------------------------
		Constructs a function wrapper. (ForEx: CFunction(Run))
	----------------------------------------------------------------------------------------------------------------------------------*/

	template
	<
		typename	 Return, 
		typename ... Args
	>
	inline CFunction
	(
		Return			(*Function)(Args&&...),
		Args	&&...	Arguments
	)
	{
		AssignFunction(Function, Arguments...);
	}


	/*----------------------------------------------------------------------------------------------------------------------------------
	Constructs a function wrapper. (ForEx: CFunction(Run))
	----------------------------------------------------------------------------------------------------------------------------------*/

	template
	<
		typename	 Return, 
		typename ... Args
	>
	inline CFunction
	(
		Return		(*Function)(Args...),
		Args&	...	Arguments
	)
	{
		AssignFunction(Function, std::forward<Args>(Arguments)...);
	}


	/*----------------------------------------------------------------------------------------------------------------------------------
		Constructs a void function lambda wrapper.
	----------------------------------------------------------------------------------------------------------------------------------*/

	template
	<
		typename ... Args
	>
	inline CFunction
	(
		typename Identity<std::function<void(Args&&...)>>::type Bound,
		Args&&...	Arguments
	)
	{
		AssignFunction(Bound, std::forward<Args>(Arguments)...);
	}


	/*----------------------------------------------------------------------------------------------------------------------------------
		Constructs a member function wrapper. (ForEx: CFunction(CFunction::Run, &Other))
	----------------------------------------------------------------------------------------------------------------------------------*/

	template
	<
		typename	Return, 
		typename	Class, 
		typename... Args
	> 
	inline CFunction
	(
		Return			(Class::*MemberFunction)(Args && ...),
		Class	*		Instance,
		Args	&& ...	Arguments
	)
	{
		AssignFunction(MemberFunction, Instance, std::forward<Args>(Arguments)...);
	}


	/*----------------------------------------------------------------------------------------------------------------------------------
		Assigns a member function wrapper. (ForEx: AssignFunction(CFunction::Run, &Other))
	----------------------------------------------------------------------------------------------------------------------------------*/

	template
	<
		typename	 Return, 
		typename	 Class, 
		typename ... Args
	> 
	inline void AssignFunction
	(
		Return			(Class::*MemberFunction)(Args&&...),
		Class	*		Instance,
		Args	&& ...	Arguments
	)
	{
		FunctionWrapper = new CMemberFunctionHolder<Return, Class, Args...>(MemberFunction, Instance, std::forward<Args>(Arguments)...);
	}


#ifdef boost::thread
	/*----------------------------------------------------------------------------------------------------------------------------------
		Assigns a function with a specific signature. (ForEx: AssignFunction(Run))
	----------------------------------------------------------------------------------------------------------------------------------*/

	template
	<
		typename Signature
	> 
	inline void AssignFunction
	(
		Signature&& Bound
	)
	{
		FunctionWrapper = new CServiceFunction(Bound);
	}
#endif

	/*----------------------------------------------------------------------------------------------------------------------------------
		Assigns a void lambda function.
	----------------------------------------------------------------------------------------------------------------------------------*/

	template
	<
		typename ... Args
	>
	inline void AssignFunction
	(
		typename Identity<std::function<void(Args &&...)> >::type Bound,
		Args &&... Arguments
	)
	{
		FunctionWrapper = new CFunctionHolder<void, Args...>(Bound, Arguments...);
	}


	/*----------------------------------------------------------------------------------------------------------------------------------
		Assigns a function with a specific Function and argument specification. (ForEx: AssignFunction(Run, 1))
	----------------------------------------------------------------------------------------------------------------------------------*/

	template
	<
		typename	 Return,
		typename ... Args
	>
	inline void AssignFunction
	(
		Return (*Function)(Args...),
		Args... Arguments
	)
	{
		FunctionWrapper = new CFunctionHolder<Return, Args...>(Function, Arguments...);
	}


	/*----------------------------------------------------------------------------------------------------------------------------------
		Executes the function if not null.
	----------------------------------------------------------------------------------------------------------------------------------*/

	inline void Run() const
	{
		if (FunctionWrapper)
		{
			FunctionWrapper->Run();
		}
	}


	/*----------------------------------------------------------------------------------------------------------------------------------
		Executes the function if not null.
	----------------------------------------------------------------------------------------------------------------------------------*/

	inline void operator()() const
	{
		Run();
	}
};