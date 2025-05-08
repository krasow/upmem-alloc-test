/* Communication with a DPU via the MRAM. */
/* Populate the MRAM with a collection of bytes and request the DPU to */
/* compute the checksum. */

#include <dpu.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#ifndef DPU_BINARY
#define DPU_BINARY "trivial_checksum_example"
#endif

// #define GROUP_ALLOC


/* Size of the buffer for which we compute the checksum: 64KBytes. */
#define BUFFER_SIZE (1 << 16)

void populate_mram(struct dpu_set_t set) {
  uint8_t buffer[BUFFER_SIZE];

  for (int byte_index = 0; byte_index < BUFFER_SIZE; byte_index++) {
    buffer[byte_index] = (uint8_t)byte_index;
  }
  DPU_ASSERT(dpu_broadcast_to(set, "buffer", 0, buffer, BUFFER_SIZE, DPU_XFER_DEFAULT));
}

int main() {
  struct dpu_set_t set, dpu;
  uint32_t checksum;

#ifdef GROUP_ALLOC
  DPU_ASSERT(dpu_alloc(64, NULL, &set));
  DPU_ASSERT(dpu_load(set, DPU_BINARY, NULL));
  printf("allocated 64 DPUS at once\n"); 
#else   
  for (uint64_t i = 0; i < 64; i++) {
	struct dpu_set_t single_dpu;  	
	DPU_ASSERT(dpu_alloc(1, NULL, &single_dpu));
	DPU_ASSERT(dpu_load(single_dpu, DPU_BINARY, NULL));
	set = single_dpu;   
  	printf("allocated single dpu %li\n", i);
  }
#endif 

  populate_mram(set);

  DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
  DPU_FOREACH(set, dpu) {
    DPU_ASSERT(dpu_copy_from(dpu, "checksum", 0, (uint8_t *)&checksum, sizeof(checksum)));
//    printf("Computed checksum = 0x%08x\n", checksum);
  }
  DPU_ASSERT(dpu_free(set));
  return 0;
}

