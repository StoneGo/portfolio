#include "mem.h"
#include "mylog.h"
#include "str.h"
#include <thread>


namespace portfolio {
  SharedMemory *SharedMemory::getMemMapping() { return new MemMapping; }

#define USING_MMAP 0

#ifdef _WINDOWS
  MemMapping::MemMapping()
      : m_hMapping(0), m_hFile(0), m_pView(0),
        m_nMode(MEMORY_SHARED_FILE_READ_WRITE) // for readonly
        ,
        m_nFileSize(0) {}

  MemMapping::~MemMapping() { CloseFile(); }

  void MemMapping::Delete() { delete this; }

  int MemMapping::OpenRead(const char *memname, size_t nFileSize) {
    UNUSED(nFileSize);
    m_nMode = MEMORY_SHARED_FILE_READ_ONLY;
    if (!memname)
      return -1;

    //	TCHAR ws_memname[MAX_PATH];
    //	MultiByteToWideChar(CP_ACP, 0, memname, strlen(memname) + 1, ws_memname,
    // sizeof(ws_memname)/sizeof(ws_memname[0]));

    m_hMapping = OpenFileMappingA(FILE_MAP_READ, FALSE, memname);
    if (!m_hMapping)
      return -2;

    m_pView = MapViewOfFile(m_hMapping, FILE_MAP_READ, 0, 0, 0);
    if (!m_pView) {
      CloseHandle(m_hMapping);
      m_hMapping = 0;
      return -3;
    }

    return 0;
  }

  int MemMapping::OpenWrite(const char *filepath, const char *memname,
                            size_t nFileSize) {
    m_nMode = MEMORY_SHARED_FILE_READ_WRITE;
    m_nFileSize = nFileSize;

    if (!memname)
      return -1;

    if (!filepath)
      return -2;

    char file[MAX_PATH];
    _snprintf_s(file, MAX_PATH, _TRUNCATE, "%s\\%s", filepath, memname);

    //		TCHAR ws_filepath[MAX_PATH];
    //		MultiByteToWideChar(CP_ACP, 0, filepath, strlen(filepath) + 1,
    // ws_filepath, sizeof(ws_filepath)/sizeof(ws_filepath[0]));

    m_hFile = CreateFileA(file, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE,
                          NULL, CREATE_ALWAYS, 0, NULL);
    if (m_hFile == INVALID_HANDLE_VALUE)
      return -3;

    //	TCHAR ws_memname[MAX_PATH];
    //	MultiByteToWideChar(CP_ACP, 0, memname, strlen(memname) + 1, ws_memname,
    // sizeof(ws_memname)/sizeof(ws_memname[0]));
    m_hMapping = CreateFileMappingA(m_hFile, NULL, PAGE_READWRITE, 0,
                                    (DWORD)nFileSize, memname);
    if (!m_hMapping) {
      CloseHandle(m_hFile);
      return -4;
    }
    /*if (GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(m_hMapping);
        CloseHandle(m_hFile);
        return (0);
    }*/

    m_pView = MapViewOfFile(m_hMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
    if (!m_pView) {
      CloseHandle(m_hMapping);
      CloseHandle(m_hFile);
      return -5;
    } else {
      memset(m_pView, 0, nFileSize);
    }

    return 0;
  }

  int MemMapping::CloseFile() {
    if (m_pView && !m_nMode)
      FlushViewOfFile(m_pView, m_nFileSize);
    if (m_pView)
      UnmapViewOfFile(m_pView);
    if (m_hMapping)
      CloseHandle(m_hMapping);
    if (m_hFile)
      CloseHandle(m_hFile);

    m_hMapping = 0;
    m_pView = 0;
    m_hFile = 0;
    m_nMode = (MEMORY_SHARED_FILE_READ_WRITE);
    m_nFileSize = (0);

    return (0);
  }
#endif //_WINDOWS

#ifdef _UNIX

#include <fcntl.h> /* For O_* constants */
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <unistd.h>

  MemMapping::MemMapping() : _shfd(-1), _dataptr(MAP_FAILED), _size(0) {
    *_name = 0;
  }

  MemMapping::~MemMapping() {}

  int MemMapping::OpenRead(const char *memname, size_t nFileSize) {
    // cout << "open read " << memname << " " << nFileSize << endl;

    CloseFile();

    _size = nFileSize;

#if (USING_MMAP)
    snprintf(_name, NAME_MAX, "/%s", memname);
    _shfd = shm_open(_name, O_RDONLY, S_IROTH);
    if (_shfd == -1) {
      printf("ERROR shm_open for reading, file %s, errno %d %s\n", _name, errno, strerror(errno));
      return MDERR_MEMMAPPING_SHMOPEN;
    } else {
      // printf("GOOD! %s opend for reading\n", _name);
    }

    off_t offset = 0 * sysconf(_SC_PAGE_SIZE);
    _dataptr = mmap(NULL, nFileSize, PROT_READ, MAP_SHARED, _shfd, offset);

    if (MAP_FAILED == _dataptr) {
      printf("ERROR mmap get pointer for reading, file %s, errno %d %s\n", _name, errno, strerror(errno));
      return MDERR_MEMMAPPING_FAILED;
    } else {
      // printf("GOOD! %s got memory for reading, size %lu\n", _name, _size);
    }
#else
    snprintf(_name, NAME_MAX, "%s", memname);
    // srand(time(0));
    key_t key = atoi(_name);
    _shfd = shmget(key, _size, 0644 | IPC_CREAT);
    if (_shfd == -1)
      return MDERR_MEMMAPPING_SHMOPEN;
    if ((_dataptr = shmat(_shfd, nullptr, 0)) == MAP_FAILED)
      return MDERR_MEMMAPPING_FAILED;
#endif // USING_MMAP
    is_writble_ = (false);
    return (0);
  }

  int MemMapping::OpenWrite(const char *filepath, const char *memname,
                            size_t nFileSize) {
    UNUSED(filepath);
    // cout << "open write " << filepath << " " << memname << " " << nFileSize <<
    // endl;
    CloseFile();

    _size = nFileSize;

#if (USING_MMAP)
    snprintf(_name, NAME_MAX, "/%s", memname);
    _shfd = shm_open(_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IROTH);
    if (_shfd == -1) {
      printf("ERROR shm_open for wirting, file %s, errno %d %s\n", _name, errno, strerror(errno));
      return MDERR_MEMMAPPING_SHMOPEN;
    } else {
        // printf("GOOD! %s opend for wirting\n", _name);
    }

    int ret = ftruncate(_shfd, _size);
    if (-1 == ret)
      return MDERR_MEMMAPPING_FTRUNCATE;

    off_t offset = 0 * sysconf(_SC_PAGE_SIZE);
    _dataptr =
        mmap(NULL, nFileSize, PROT_READ | PROT_WRITE, MAP_SHARED, _shfd, offset);

    if (MAP_FAILED == _dataptr) {
      printf("ERROR mmap get pointer for wirting, file %s, errno %d %s\n", _name, errno, strerror(errno));
      return MDERR_MEMMAPPING_FAILED;
    }
    else {
      memset(_dataptr, 0, nFileSize);
      // printf("GOOD! %s got memory for wirting, size %lu\n", _name, _size);
    }
#else
    snprintf(_name, NAME_MAX, "%s", memname);
    srand(static_cast<uint32_t>(time(nullptr)));
    _key = atoi(_name);
    _shfd = shmget(_key, _size, IPC_CREAT | 0666);
    if (_shfd == -1) {
      printf("Open Mem To Write error is %d, %s, %lu, %d\n",
             errno, strerror(errno), _size, _key);
      return MDERR_MEMMAPPING_SHMOPEN;
    }

    if ((_dataptr = shmat(_shfd, nullptr, 0)) == MAP_FAILED)
      return MDERR_MEMMAPPING_FAILED;
    else
      memset(_dataptr, 0, _size);
#endif // USING_MMAP

    is_writble_ = true;
    return (0);
  }

  int MemMapping::CloseFile() {
#if (USING_MMAP)
    if (MAP_FAILED != _dataptr) {
      if (-1 == munmap(_dataptr, _size))
        return MDERR_MAMMAPPING_MUNMAP;
      _dataptr = MAP_FAILED;
    }

    if (-1 != _shfd && is_writble_) {
      if (-1 == shm_unlink(_name))
        return MDERR_MAMMAPPING_SHMUNLINK;
      _shfd = -1;
    }
#else
    if (MAP_FAILED != _dataptr) {
      if (-1 == shmdt(_dataptr))
        return MDERR_MAMMAPPING_MUNMAP;
      _dataptr = MAP_FAILED;
    }

    if (-1 != _shfd && is_writble_) {
      if (shmctl(_shfd, IPC_RMID, nullptr) == -1) {
        printf("shmctl error\n");
        return MDERR_MAMMAPPING_MUNMAP;
      }
      _shfd = -1;
    }
#endif // USING_MMAP

    return 0;
  }

  void *MemMapping::GetDataPtr() { return _dataptr; }

  bool MemMapping::IsValidFile() {
    return (MAP_FAILED != _dataptr && -1 != _shfd);
  }

  void MemMapping::Delete() { delete this; }


#endif //_UNIX


} // namespace portfolio