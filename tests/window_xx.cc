#include "cutl.hpp"
#include "test.h"

using namespace cu;

int main()
{
    query(Context::init());
    const auto contextGuard = Context::Guard{};

    Window window{};
    query(window.init(WIN_W, WIN_H, "Window Test C++"));

    Renderer renderer{};
    query(renderer.init(window));

    while (!window.should_close()) {
        window.update();
    }

    success();
}
