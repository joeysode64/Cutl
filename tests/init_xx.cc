#include "cutl.hpp"
#include "test.hpp"

using namespace cu;

int main()
{
    query(Context::init());

    Renderer renderer{};
    query(Renderer::create(renderer, nullptr));

    success();
}
