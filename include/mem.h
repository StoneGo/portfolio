#ifndef MEMMAPPING_H
#define MEMMAPPING_H

#include "portfolio_global.h"

#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#ifndef NAME_MAX
#define NAME_MAX 255
#endif // name_max

#define MEMORY_SHARED_FILE_READ_WRITE 0
#define MEMORY_SHARED_FILE_READ_ONLY 1

#define RECEIVER_STATUS_NONE -1
#define RECEIVER_STATUS_WAITING_FOR_DATA 0
#define RECEIVER_STATUS_RECEIVEING_DATA 1
#define RECEIVER_STATUS_FINISH_ONE_DAY_DATA 2

#define MDERR_MEMMAPPING_SHMOPEN            0x10000012
#define MDERR_MEMMAPPING_FAILED             0x10000013
#define MDERR_MAMMAPPING_MUNMAP             0x10000014
#define MDERR_MAMMAPPING_SHMUNLINK          0x10000015
#define MDERR_MEMMAPPING_FTRUNCATE          0x10000016


#ifdef _WINDOWS
typedef uint64_t key_t;
#endif //_WINDOWS


namespace portfolio {

  class PORTFOLIO_API SharedMemory {
  public:
    SharedMemory() : is_writble_(false) {}

    virtual ~SharedMemory() {}

  public:
    static SharedMemory *getMemMapping();

    virtual void Delete() = 0;

    virtual int OpenRead(const char *memname, size_t nFileSize = 0) = 0;

    virtual int OpenWrite(const char *filepath, const char *memname,
                          size_t nFileSize) = 0;

    virtual int CloseFile() = 0;

    virtual bool IsValidFile() = 0;

    virtual void *GetDataPtr() = 0;

    virtual key_t getKey() = 0;

  protected:
    bool is_writble_;
  };


#ifdef _WINDOWS
#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
  // Windows Header Files:
#include <windows.h>

  // C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

  class PORTFOLIO_API MemMapping : public SharedMemory {
  public:
    MemMapping();
    ~MemMapping();

    void Delete();

    int OpenRead(const char *memname, size_t nFileSize = 0);
    int OpenWrite(const char *filepath, const char *memname, size_t nFileSize);

    int CloseFile();
    bool IsValidFile() { return (m_pView != 0); }

    void *GetDataPtr() { return m_pView; }
    key_t getKey() { return 0; }

  private:
    HANDLE m_hFile;
    HANDLE m_hMapping;
    LPVOID m_pView;
    TCHAR m_strPath[MAX_PATH];
    int m_nMode;
    size_t m_nFileSize;
  };
#endif //_WINDOWS

#ifdef _UNIX

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

class PORTFOLIO_API MemMapping : public SharedMemory {
public:
  MemMapping();

  ~MemMapping();

  void Delete();

  int OpenRead(const char *memname, size_t nFileSize = 0);

  int OpenWrite(const char *filepath, const char *memname, size_t nFileSize);

  int CloseFile();

  bool IsValidFile();

  void *GetDataPtr();

  key_t getKey() { return _key; }

private:
  int _shfd;
  char _name[NAME_MAX];
  void *_dataptr;
  size_t _size;
  key_t _key;
};

#endif //_UNIX

}  // namespace portfolio

#endif // MEMMAPPING_H
