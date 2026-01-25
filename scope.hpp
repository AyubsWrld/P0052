#pragma once


#include <type_traits>
namespace Atlas
{
    template<typename E>
    class scope_exit;

    template<typename E>
    class scope_fail;

    template<typename E>
    class scope_success;

    template<typename R, typename D>
    class unique_resource;

    template<typename R, typename D, typename S=std::decay<R>>
    unique_resource<std::decay_t<R>, std::decay_t<D>> 
    make_unique_resource_checked(R&& r, const S& invalid, D&& d) noexcept;
}
