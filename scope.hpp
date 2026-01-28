#pragma once

#include <cstdlib>
#include <type_traits>
#include <concepts>
#include <utility>

namespace Atlas
{
    /*
        ScopeExit is a general-purpose scope guard that calls its exit function when a scope is exited.
        The class templates ScopeFail and ScopeSuccess share the ScopeExit interface, only the situation
        when the exit function is called differs.
    */

    template<typename EF>
    class ScopeExit;

    template<typename EF>
    class ScopeFail;

    template<typename EF>
    class ScopeSuccess;

    template<typename R, typename D>
    class UniqueResource;

    template<typename R, typename D, typename S = std::decay<R>>
    UniqueResource<std::decay_t<R>, std::decay_t<D>>
    MakeUniqueResourceChecked(R&& r, const S& invalid, D&& d) noexcept;

    /*
        NOTE:
        If the exit function object of a ScopeSuccess or ScopeExit object refers to a local variable
        of the function where it is defined (e.g., as a lambda capturing the variable by reference),
        and that variable is used as a return operand in that function, that variable might have
        already been returned when the scope guard’s destructor executes. This can lead to surprising
        behavior.
    */

    template<typename EF>
    class ScopeExit
    {
    private:
        EF   ExitFunction;
        bool ExecuteOnDestruction{ true };

    public:
        template<typename EFP>
        requires std::is_nothrow_constructible_v<EF, EFP> ||
                 std::is_nothrow_constructible_v<EF, EFP&>
        explicit ScopeExit(EFP&& callable) noexcept;

        explicit ScopeExit(ScopeExit&& rhs) noexcept;

        ~ScopeExit() noexcept;

        void Release() noexcept;

        ScopeExit(const ScopeExit&)            = delete;
        ScopeExit& operator=(const ScopeExit&) = delete;
        ScopeExit& operator=(ScopeExit&&)      = delete;
    };
}

namespace Atlas
{
    template<typename EF>
    template<typename EFP>
    requires std::is_nothrow_constructible_v<EF, EFP> ||
             std::is_nothrow_constructible_v<EF, EFP&>
    ScopeExit<EF>::ScopeExit(EFP&& callable) noexcept
        : ExitFunction(std::forward<EFP>(callable))
    {
    }

    template<typename EF>
    ScopeExit<EF>::ScopeExit(ScopeExit&& rhs) noexcept
        : ExitFunction(std::move(rhs.ExitFunction))
    {
        rhs.ExecuteOnDestruction = false;
    }

    template<typename EF>
    ScopeExit<EF>::~ScopeExit() noexcept
    {
        if (ExecuteOnDestruction)
            ExitFunction();
    }

    template<typename EF>
    void ScopeExit<EF>::Release() noexcept
    {
        ExecuteOnDestruction = false;
    }
}
