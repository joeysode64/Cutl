#include "cutl.hpp"
#include "test.hpp"

using namespace cu;

int main()
{
    query(Context::init());

    Window window{};
    query(Window::create(window, WIN_W, WIN_H, "Window Test C++"));

    Renderer renderer{};
    query(Renderer::create(renderer, window));

    while (!window.should_close()) {
        window.update();
    }

    success();
}
