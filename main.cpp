#include "mygp_type_traits.h"
#include <memory>

std::unique_ptr<int> h(new int);

struct base {};
struct derived : public base {};
struct individue {};

template <typename T,
    typename mygp::enable_if_t<mygp::is_convertible_v<T, base>, int> = 0>
    struct emplement {
    T num;
};

int main(int argc, char* argv[]) {
    ::emplement<::derived> a;
    int c_a = sizeof(a);
    if (c_a) {}
    //::emplement<::individue> b;
    return 0;
}