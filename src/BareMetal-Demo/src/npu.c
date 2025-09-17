// helloc.c -- Output a 'hello world' message

// gcc -c -m64 -nostdlib -nostartfiles -nodefaultlibs -o helloc.o helloc.c
// gcc -c -m64 -nostdlib -nostartfiles -nodefaultlibs -o libBareMetal.o libBareMetal.c
// ld -T c.ld -o helloc.app helloc.o libBareMetal.o

#include "libBareMetal.h"

struct pci_config_header {
  uint32_t vendor_id : 16;
  uint32_t device_id : 16;
  uint32_t command : 16;
  uint32_t status : 16;
  uint32_t revision_id : 8;
  uint32_t prog_if : 8;
  uint32_t subclass : 8;
  uint32_t class_code : 8;

  uint32_t cache_line_size : 8;
  uint32_t latency_timer : 8;
  uint32_t header_type : 8;
  uint32_t bist : 8;
};



typedef union  {
  uint32_t address;

  struct base_address_bf {
    uint32_t always_0 : 1;
    uint32_t type : 2;
    uint32_t prefetchable : 1;
    uint32_t aligned_base_address : 28;
  } base_address_bf;

  struct base_address_bf address_bf;
} base_address;

struct pci_config_0 {
  struct pci_config_header header;
  base_address bar0;
  base_address bar1;
  base_address bar2;
  base_address bar3;
  base_address bar4;
  base_address bar5;
  uint32_t cardbus_cis_pointer;
  uint32_t subsystem_vendor_id : 16;
  uint32_t subsystem_id : 16;
  uint32_t expansion_rom_base_address;
  uint32_t capabilities_pointer : 8;
  uint32_t reserved_0 : 24;
  uint32_t reserver_1;
  uint32_t interrupt_line : 8;
  uint32_t interrupt_pin : 8;
  uint32_t min_grant : 8;
  uint32_t max_latency : 8;
};

struct pci_config_header_0 {
  struct pci_config_0 config;
  uint64_t bar[6];
};

// 	call [0x589c] ; dump_rax
//  	call [0x58a4] ; newline
void newline() {    
    asm volatile ("call *0x58a4");
}

void dump_u64(uint64_t val){
    newline();
    asm volatile ("call *0x589c" : : "a" (val));
}

int main(void)
{
	b_output("Hello, NPU!", 11);
    

    uint32_t* p_npu_config = (uint32_t*)0xc0058000;
    struct pci_config_0* p_pci_config = (struct pci_config_0*)p_npu_config;
    uint64_t device_id = (uint64_t)p_pci_config->header.device_id;
    p_pci_config->header.command |= 0x2; // Enable MMIO
    uint64_t command = (uint64_t)p_pci_config->header.command;
    dump_u64(device_id);
    dump_u64(command);
    struct pci_config_header_0 config_ex;
    config_ex.config = *p_pci_config;

    if (p_pci_config->bar0.address_bf.type == 0x2) {
        uint64_t bar0 = (uint64_t)(p_pci_config->bar0.address & 0xfffffff0u) | ((uint64_t)(p_pci_config->bar1.address & ~0u) << 32);          
        config_ex.bar[0] = bar0;
    }
    if (p_pci_config->bar4.address_bf.type == 0x2) {
        uint64_t bar4 = (uint64_t)(p_pci_config->bar4.address & 0xfffffff0u) | ((uint64_t)(p_pci_config->bar5.address & ~0u) << 32);          
        config_ex.bar[1] = bar4;
    }

    dump_u64(config_ex.bar[0]);
    dump_u64(config_ex.bar[1]);
	return 0;
}
