#include <FreeRTOS.h>
#include <task.h>
#include <cli.h>
#include <bl_sys_time.h>
#include <blog.h>

void vApplicationMallocFailedHook(void)
{
    printf("Memory Allocate Failed. Current left size is %d bytes\r\n",
        xPortGetFreeHeapSize()
    );
}

static void _cli_init()
{
    /*Put CLI which needs to be init here*/
    bl_sys_time_cli_init();
}

static void system_thread_init()
{
    /*nothing here*/
}

#define HEAP_TEST_UNIT_SIZE 1024

static void __heap_test_fun (void *p_arg)
{
    int counts, words, piece;
    uint32_t *ptr, *p_heap_addr;
    uint32_t  ptr_piece_num;
    uint8_t   test_ok;
    uint32_t  len = 0;

    TaskHandle_t *p_handle;

    ptr_piece_num = xPortGetFreeHeapSize() / HEAP_TEST_UNIT_SIZE + 1;
    p_heap_addr = pvPortMalloc(ptr_piece_num * 4);

    if (p_heap_addr == NULL) {
        blog_info("mem not enough\r\n");
        p_handle = (TaskHandle_t *)p_arg;
        vTaskDelete(*p_handle);
        return;
	}

    counts = 0;
    ptr = NULL;
    while ((ptr = pvPortMalloc(HEAP_TEST_UNIT_SIZE)) != NULL) {

        if (counts == 0) {
            blog_info("ptr_start = %p\r\n", ptr);
        }

        blog_info("ptr = %p\r\n", ptr);
        for (words = 0; words < HEAP_TEST_UNIT_SIZE / 4; words++) {
            *(ptr + words) = (uint32_t)ptr + words;
        }

        p_heap_addr[counts] = (uint32_t)ptr;
        counts++;
    }
    blog_info("ptr_end = %p\r\n", p_heap_addr[counts - 1]);
    blog_info("%d pieces of test mem, %d bytes each\r\n", counts - 1, HEAP_TEST_UNIT_SIZE);

    test_ok = 1;
    for (piece = 0; piece < counts; piece++) {
        ptr = (uint32_t *)p_heap_addr[piece];
        for (words = 0; words < HEAP_TEST_UNIT_SIZE / 4; words++) {
            if (*(ptr + words) != (uint32_t)ptr + words) {
                blog_info("\r\n-------------%p error----------\r\n", ptr);
                test_ok = 0;
            }
        }
        vPortFree(ptr);
    }
    vPortFree(p_heap_addr);

    ptr = pvPortMalloc(HEAP_TEST_UNIT_SIZE);

    for ( ; len < HEAP_TEST_UNIT_SIZE; len += 32)
    {
    	ptr = pvPortRealloc(ptr, len);
    	blog_info("Realloc %d, ptr = %p\r\n", len, ptr);
    	blog_info("FreeHeapSize %d \r\n", xPortGetFreeHeapSize());
    }

    for ( ; len > 0; len -= 32)
    {
        ptr = pvPortRealloc(ptr, len);
        blog_info("Realloc %d, ptr = %p\r\n", len, ptr);
        blog_info("FreeHeapSize %d \r\n", xPortGetFreeHeapSize());
    }

    vPortFree(ptr);
    blog_info("FreeHeapSize %d \r\n", xPortGetFreeHeapSize());

    if (test_ok == 1) {
        blog_info("mem heap test ok!\r\n");
    } else {
        blog_info("mem heap test failed!\r\n");
    }

    p_handle = (TaskHandle_t *)p_arg;
    vTaskDelete(*p_handle);
}

static void cmd_heap_test(char *buf, int len, int argc, char **argv)
{
    static StackType_t heap_test_stack[1024];
    static StaticTask_t heap_test_task;
    static TaskHandle_t task_handle;

    puts("[OS] start heap test task...\r\n");
    task_handle = xTaskCreateStatic(__heap_test_fun,
                                    (char*)"test",
                                    1024,
                                    &task_handle,
                                    20,
                                    heap_test_stack,
                                    &heap_test_task);
}

const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
        { "heap_test", "Heap Test", cmd_heap_test},
};

static void proc_main_entry(void *pvParameters)
{
    _cli_init();

    vTaskDelete(NULL);
}
void main(void)
{
    system_thread_init();

    puts("[OS] proc_main_entry task...\r\n");
    xTaskCreate(proc_main_entry, (char*)"main_entry", 1024, NULL, 15, NULL);
}
