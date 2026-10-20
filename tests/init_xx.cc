#include "cutl.hpp"
#include "test.h"

using namespace cu;

int main()
{
    query(Context::init());
    const auto contextGuard = Context::Guard{};

    Renderer renderer{};
    query(Renderer::create(renderer));

    success();
}
