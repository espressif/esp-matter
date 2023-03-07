/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#define MEM_NUM_MEM_ALLOC 32

typedef struct {
    void* heap_ptr;
    int size;
}mem_obj_dev_t;

typedef struct {
    void* heap_ptr;
    int size;
}mem_obj_comp_t;

typedef struct {
    mem_obj_dev_t mem_dev[MEM_NUM_MEM_ALLOC];
    mem_obj_comp_t mem_comp[MEM_NUM_MEM_ALLOC];
    int num_malloc_dev;
    int num_malloc_comp;
    int persi_mem_dev;
    int persi_mem_comp;
}mem_obj_t;

void* mem_malloc(int size, int id);
void mem_free(void * heap_ptr, int id);
void* mem_init();
void mem_exit();
int mem_get_alloc_size(mem_obj_t* mem_handle, int id);



