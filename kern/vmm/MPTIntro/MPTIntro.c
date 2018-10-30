#include <lib/gcc.h>
#include <lib/x86.h>
#include <lib/debug.h>

#include "import.h"

#define PT_PERM_UP  0
#define PT_PERM_PTU (PTE_P | PTE_W | PTE_U)

/**
 * Page directory pool for NUM_IDS processes.
 * mCertiKOS maintains one page structure for each process.
 * Each PDirPool[index] represents the page directory of the page structure
 * for the process # [index].
 * In mCertiKOS, we statically allocate page directories, and maintain the second
 * level page tables dynamically.
 */
unsigned int *PDirPool[NUM_IDS][1024] gcc_aligned(PAGESIZE);

/**
 * In mCertiKOS, we use identity page table mappings for the kernel memory.
 * IDPTbl is a statically allocated identity page table that will be reused for
 * all the kernel memory.
 * That is, in every page directory, the entries that fall into the range of
 * addresses reserved for the kernel will point to an entry in IDPTbl.
 */
unsigned int IDPTbl[1024][1024] gcc_aligned(PAGESIZE);

// Sets the CR3 register with the start address of the page structure for process # [index].
void set_pdir_base(unsigned int index)
{
    set_cr3(PDirPool[index]);
}

// Returns the page directory entry # [pde_index] of the process # [proc_index].
// This can be used to test whether the page directory entry is mapped.
unsigned int get_pdir_entry(unsigned int proc_index, unsigned int pde_index)
{
    // TODO
    return (unsigned int) PDirPool[proc_index][pde_index];
}

// Sets the specified page directory entry with the start address of physical
// page # [page_index].
// You should also set the permissions PTE_P, PTE_W, and PTE_U.
void set_pdir_entry(unsigned int proc_index, unsigned int pde_index,
                    unsigned int page_index)
{
    // TODO
    unsigned int real_address = (page_index << 12)|PT_PERM_PTU; //to let the lower 12 bits available
    PDirPool[proc_index][pde_index]=(unsigned int*) real_address;
}

// Sets the page directory entry # [pde_index] for the process # [proc_index]
// with the initial address of page directory # [pde_index] in IDPTbl.
// You should also set the permissions PTE_P, PTE_W, and PTE_U.
// This will be used to map a page directory entry to an identity page table.
void set_pdir_entry_identity(unsigned int proc_index, unsigned int pde_index)
{
    // TODO
    unsigned int real_address = ((unsigned int) IDPTbl[pde_index])|PT_PERM_PTU;
    PDirPool[proc_index][pde_index]=(unsigned int*) real_address;
}

// Removes the specified page directory entry (sets the page directory entry to 0).
// Don't forget to cast the value to (unsigned int *).
void rmv_pdir_entry(unsigned int proc_index, unsigned int pde_index)
{
    // TODO
    PDirPool[proc_index][pde_index]=(unsigned int*)0x00000000;
}

// Returns the specified page table entry.
// Do not forget that the permission info is also stored in the page directory entries.
unsigned int get_ptbl_entry(unsigned int proc_index, unsigned int pde_index,
                            unsigned int pte_index)
{
    // TODO
    unsigned int base_address = (unsigned int) PDirPool[proc_index][pde_index];
    base_address&=0xfffff000; //remove lower 12 bits
    unsigned int *res;
    res = (unsigned int *) (base_address + (pte_index<<2)); // each entry is 4 bytes;
    return *res;
}

// Sets the specified page table entry with the start address of physical page # [page_index]
// You should also set the given permission.
void set_ptbl_entry(unsigned int proc_index, unsigned int pde_index,
                    unsigned int pte_index, unsigned int page_index,
                    unsigned int perm)
{
    // TODO
    // Sets the specified page table entry!!! use pointers
    unsigned int base_address =  (unsigned int)PDirPool[proc_index][pde_index];
    base_address&=0xfffff000;
    base_address+=pte_index<<2;

    unsigned int* target;
    target = (unsigned int *)base_address;//set pointer to address
    *target=(page_index<<12)|perm;
}

// Sets up the specified page table entry in IDPTbl as the identity map.
// You should also set the given permission.
void set_ptbl_entry_identity(unsigned int pde_index, unsigned int pte_index,
                             unsigned int perm)
{
    // TODO
    IDPTbl[pde_index][pte_index]=((pde_index<<10)+pte_index)<<12;
    IDPTbl[pde_index][pte_index]|=perm;
}

// Sets the specified page table entry to 0.
void rmv_ptbl_entry(unsigned int proc_index, unsigned int pde_index,
                    unsigned int pte_index)
{
    // TODO
    unsigned int target = (unsigned int)PDirPool[proc_index][pde_index];
    target&=0xfffff000;
    target+=pte_index<<2;

    unsigned int *t=(unsigned int*) target;
    *t=0x00000000;
}
