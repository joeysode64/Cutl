#include "cutl.h"
#include "test.h"

int main() {
    query(cu_context_init(nullptr));

    cu_context_terminate();

    success();
}
