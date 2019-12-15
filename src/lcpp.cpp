#include "ptf_allocator.h"
#include <iostream>

using namespace portfolio;

// template <typename T>
// std::vector<T, PreAllocator<T>> MyVector;
using IntVector = std::vector<int, PreAllocator<int>>;

static void test_allocator(int *arr, IntVector &vec) {
  vec.push_back(1024);
  std::cout << arr << ", " << vec.data() << ", " << (arr == vec.data()) << "\n";
  std::cout << "My_Vec[0]: " << vec[0] << "\n";
  std::cout << "My_Arr[0]: " << arr[0] << "\n";
  std::cout << vec.size() << ", " << vec.max_size() << "\n";
  vec.resize(10);
  std::cout << vec.size() << ", " << vec.max_size() << "\n";
  for (size_t i = 0; i < vec.size(); ++i)
    std::cout << vec[i] << ", ";
  std::cout << "\n";
  for (size_t i = 0; i < vec.size(); ++i)
    arr[i] = i + 1;
  for (size_t i = 0; i < vec.size(); ++i)
    std::cout << vec[i] << ", ";
  std::cout << "\n";
  std::cout << arr << ", " << vec.data() << ", " << (arr == vec.data()) << "\n";
}

int main_2() {
  int my_arr[100] = {0};
  IntVector my_vec(0, PreAllocator<int>(&my_arr[0], 100));
  test_allocator(my_arr, my_vec);

  int *my_heap_ptr = new int[100]();
  IntVector my_heap_vec(0, PreAllocator<int>(&my_heap_ptr[0], 100));
  test_allocator(my_heap_ptr, my_heap_vec);

  delete[] my_heap_ptr;
  my_heap_ptr = nullptr;

  return 0;
}


void main_3() {
  int *ary = new int(10);
  for (int i = 0; i < 10; ++i)
    ary[i] = i + 1;

  std::vector<int> vec;
  vec.assign(ary, ary + 3);
  std::cout << "size " << vec.size() << std::endl;


  std::cout << ary << ", " << vec.data() << std::endl;

  vec.assign(ary, ary + 5);
  std::cout << "size " << vec.size() << std::endl;

  vec.resize(9);
  std::cout << "size " << vec.size() << std::endl;

  for (size_t i = 0; i < vec.size(); ++i)
    std::cout << vec[i] << ' ';
  std::cout << std::endl;

  for (int i = 0; i < 10; ++i)
    ary[i] = i + 1;

  for (size_t i = 0; i < vec.size(); ++i)
    std::cout << vec[i] << ' ';
  std::cout << std::endl;

  delete ary;
}


template<typename T,
  template<class> class C
>
class XCs {
private:
  C<T> c;
public:
  XCs() {
    for (int i = 0; i < 100; ++i)
      c.insert(c.end(), T());
  }
};


int main() {
  // main_1();
  main_2();

  printf("test_vector_ctpmd\n");
  portfolio::test_vector_ctpmd();
  printf("test_vector_ctpmd\n");

  return 0;
}
