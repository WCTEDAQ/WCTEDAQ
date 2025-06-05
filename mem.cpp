struct MemoryUsage{

  uint32_t total_allocated = 0;
  uint32_t total_freed = 0;

  uint32_t CurrentUsage() { return total_allocated - total_free)

  void PrintUsage(){ printf("total_allocated = %u, total_freed = %u, current_usage = %u\n", total_allocated, total_freed, CurrentUsage());} 

};

static MemoryUsage memory_usage;

  


void* operator new(size_t size){

  memory_usage.total_allocated += size;
  
  return malloc(size);
}

void operator delete (void* memory, size_t size){

  memory_usage.total_freed += size;

  free(memory);
}
