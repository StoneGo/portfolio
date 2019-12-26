//
// Created by stone on 11/20/19.
//

#ifndef CTPAPI_PTF_ALLOCATOR_H
#define CTPAPI_PTF_ALLOCATOR_H

#include "portfolio_global.h"
#include <cstdint>
#include <vector>

namespace portfolio {

template<typename T>
class PreAllocator {
private:
  T *memory_ptr;
  std::size_t memory_size;

public:
  typedef std::size_t size_type;
  typedef T *pointer;
  typedef T value_type;

  PreAllocator(T *memory_ptr, std::size_t memory_size)
    : memory_ptr(memory_ptr), memory_size(memory_size) {}

  PreAllocator(const PreAllocator &other) throw()
    : memory_ptr(other.memory_ptr), memory_size(other.memory_size) {};

  template<typename U>
  PreAllocator(const PreAllocator<U> &other) throw()
    : memory_ptr(other.memory_ptr), memory_size(other.memory_size) {};

  template<typename U>
  PreAllocator &operator=(const PreAllocator<U> &other) { return *this; }

  PreAllocator<T> &operator=(const PreAllocator &other) { return *this; }

  ~PreAllocator() {}


  pointer allocate(size_type n, const void *hint = 0) { return memory_ptr; }

  void deallocate(T *ptr, size_type n) {}

  size_type max_size() const { return memory_size; }
};

template<class T>
using SharedMemVector = std::vector<T, PreAllocator<T>>;


enum SHARED_DATA_TYPE {
  SHARED_DATA_TYPE_NONE = 0,
  SHARED_DATA_TYPE_DOUBLE = 1,
  SHARED_DATA_TYPE_INT = 2,
  SHARED_DATA_TYPE_TIMET = 3,
  SHARED_DATA_TYPE_DOUBLE_TIME = 4,
};


uint8_t get_size_by_type(SHARED_DATA_TYPE t) {
  uint8_t s = 0;
  switch (t) {
    case SHARED_DATA_TYPE_DOUBLE:
    case SHARED_DATA_TYPE_TIMET:
    case SHARED_DATA_TYPE_DOUBLE_TIME:
      s = 8;
      break;
    case SHARED_DATA_TYPE_INT:
      s = 4;
      break;
    default:
      s = 0;
      break;
  }
  return s;
}

// the size-infomation
typedef struct DataSizeInfo {
    size_t amount_of_member = 0;  // amout of member of a data struct or class
    size_t size_of_data = 0;      // the size of one data
    size_t max_size = 0;          // the capability of memory-block
    size_t size = 0;              // the size in current dataset
} DataSize;

// Interface
class DataInfo {
public:
  DataInfo() {
    dsi = new DataSizeInfo;
    my_dsi = true;
  }
  DataInfo(DataSizeInfo* dsi) : dsi(dsi), my_dsi(false) { }
  virtual ~DataInfo() { if (my_dsi) SafeDeletePtr(dsi); }

  std::vector<uint8_t> &get_field_size() { return field_size; }
  std::vector<SHARED_DATA_TYPE> &get_field_type() { return field_type; }
  size_t get_amount_of_member() { return dsi->amount_of_member; }
  size_t get_size_of_data() { return dsi->size_of_data; }
  size_t size() { return  dsi->size; }
  size_t max_size() { return dsi->max_size; }

  DataSizeInfo* get_data_info_ptr() { return dsi; }
  void set_data_info_ptr(DataSizeInfo* dsi) {this->dsi = dsi; my_dsi = false;}

public:
  DataSizeInfo* dsi = nullptr;

protected:
  virtual void set_amount_of_member() = 0;
  virtual void set_size_of_data() = 0;
  virtual void set_field_type() = 0;

  bool my_dsi = false;
  std::vector<uint8_t> field_size;
  std::vector<SHARED_DATA_TYPE> field_type;
};

typedef struct CTPMD {
  double lp;
  double ap;
  double bp;
  int lv;
  int av;
  int bv;
  double ft;
} CTPMD;

class CTPMDInfo : public DataInfo {
public:
  CTPMDInfo() {
    set_amount_of_member();
    set_size_of_data();
    set_field_type();
  }

  virtual ~CTPMDInfo() {}

private:
  virtual void set_amount_of_member() { dsi->amount_of_member = 5; }
  virtual void set_size_of_data() { dsi->size_of_data = sizeof(CTPMD); }
  virtual void set_field_type() {
    field_type = {
      SHARED_DATA_TYPE_DOUBLE, SHARED_DATA_TYPE_DOUBLE, SHARED_DATA_TYPE_DOUBLE,
      SHARED_DATA_TYPE_INT, SHARED_DATA_TYPE_INT, SHARED_DATA_TYPE_INT,
      SHARED_DATA_TYPE_DOUBLE_TIME};
    field_size.clear();
    for (auto &type: field_type) {
      field_size.emplace_back(get_size_by_type(type));
    }
  }
};

template<typename T, class TInfo>
class SharedMemDataset {
public:
  SharedMemDataset() {}

  SharedMemDataset(size_t size) {
    char* buf = new char[sizeof(T) * size + sizeof(TInfo)];
    info = reinterpret_cast<TInfo*>(buf);
    data = reinterpret_cast<T*>(buf + sizeof(TInfo));
    writable = true;
    vec = new SharedMemVector<T>(0, PreAllocator<T>(data, size));
  }

  SharedMemDataset(TInfo* info, T* data, size_t size)
    : writable(false) , info(info), data(data) {
    vec = new SharedMemVector<T>(0, PreAllocator<T>(this->data, size));
  }

  virtual ~SharedMemDataset() {
    SafeDeletePtr(vec);
    if (writable) {
      char * buf = reinterpret_cast<char *>(info);
      SafeDeleteAry(buf);
    } else {
      info = nullptr;
    }
    data = nullptr;
  }

  TInfo* get_data_info_ptr() {return info;}
  T* get_data_ptr() { return data; }

  void push_back(T& t) {
    if (writable) {
      vec->push_back(t);
      info->dsi->size = vec->size();
    }
  }

  size_t size() { return info->dis->data_size; }

  void resize(size_t size) {
    if (writable) {
      vec->resize(size);
      info->dsi->size = size;
    } else {
      info->dsi->size = size;
    }
  }

  T& operator[](uint64_t i) {
    if (writable)
      return vec->at(i);
    else
      return data[i];
  }

  T& at(uint64_t i) {
    if (writable)
      return vec->at(i);
    else
      return data[i];
  }

public:
  bool writable = false;
  TInfo* info = nullptr;
  T* data = nullptr;
  SharedMemVector<T>* vec = nullptr;
};

using SharedCtpmdData = SharedMemDataset<CTPMD, CTPMDInfo>;

extern void test_vector_ctpmd();

} // namespace portfolio

#endif //CTPAPI_PTF_ALLOCATOR_H
