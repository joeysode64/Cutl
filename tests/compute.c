#include "cutl.h"
#include "test.h"

#include <stdint.h>

#define N 64

int main() {
    query(cu_context_init(nullptr));

    const CuPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .pushConstantSize = sizeof(CuDeviceAddress),
    };
    CuPipelineLayout pipelineLayout = {};
    query(cu_pipeline_layout_create(&pipelineLayout, &pipelineLayoutCreateInfo));

    CuSpirV comp = {};
    query(cu_spirv_read_from_file(&comp, "tests/shaders/compute.comp.spv"));

    CuComputePipeline computePipeline = {};
    query(cu_compute_pipeline_create(&computePipeline, pipelineLayout, comp.data, comp.n));
    cu_spirv_destroy(&comp);

    CuDynamicBuffer buffer = {};
    query(cu_dynamic_buffer_create(
        &buffer,
        1,
        N * sizeof(float),
        CU_BUFFER_STORAGE_BUFFER,
        CU_DEDICATED_ALLOCATOR_MODE));

    void* pData = nullptr;
    CuDeviceAddress address = 0;
    cu_dynamic_buffer_get_data_frame(&buffer, &pData, &address, 0);

    float* values = (float*)pData;
    for (int i = 0; i < N; i++) {
        values[i] = (float)i;
    }

    CuTask task = {};
    query(cu_task_create(&task));

    query(cu_task_begin(&task));
    cu_cmd_bind_compute_pipeline(&task, computePipeline);
    cu_cmd_write_push_constants(&task, pipelineLayout, &address, sizeof(address), 0);
    cu_cmd_dispatch(&task, 1, 1, 1);
    query(cu_task_submit(&task));

    cu_task_await(&task, 2'000'000'000);

    for (int i = 0; i < N; i++) {
        const float expected = (float)i * 2.0F;
        expect(
            values[i] == expected,
            " At index %i: expected %f, found %f\n",
            i,
            expected,
            values[i]);
    }

    cu_task_destroy(&task);
    cu_dynamic_buffer_destroy(&buffer, CU_DEDICATED_ALLOCATOR_MODE);
    cu_compute_pipeline_destroy(computePipeline);
    cu_pipeline_layout_destroy(pipelineLayout);
    cu_context_terminate();

    success();
}
