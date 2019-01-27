/*-----------------------------------------------------------------------------+
|                                                                              |
|   SCL - Simulation Class Library                                             |
|                                                                              |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                              |
|       University of Essen, Germany                                           |
|                                                                              |
+---------------+-------------------+---------------+-------------------+------+
|   Module      |   File            |   Created     |   Project         |      |
+---------------+-------------------+---------------+-------------------+------+
| SCMem         |   SCMem.cc        | 10. Mar 1996  |   SCL             |      |
+---------------+-------------------+---------------+-------------------+------+
|                                                                              |
|   Change Log                                                                 |
|                                                                              |
|   Nr.      Date        Description                                           |
|  -----    --------    ------------------------------------------------------ |
|   001                                                                        |
|   000     09.08.94    Neu angelegt                                           |
|                                                                              |
+-----------------------------------------------------------------------------*/

/*  Lineal
00000000001111111111222222222233333333334444444444555555555566666666667777777777
01234567890123456789012345678901234567890123456789012345678901234567890123456789
*/

#include "SCMem.h"
#include "SCStream.h"

#include <string.h>

#if _SC_DMALLOC
  #include <dmalloc.h>
#endif

SCMem::SCMem(const SCSize &size) :
  offset(0),
  hist_offset(0)
{
  save_area_size = size.size;

  if (save_area_size)
  {
    save_area = new char[save_area_size];
    assert(save_area);
  }
  else
  {
    save_area = NULL;
  }

  hist_area_size = size.historySize;

  if (hist_area_size)
  {
    hist_area = new char[hist_area_size];
    assert(hist_area);
  }
  else
  {
    hist_area = NULL;
  }
}


SCMem::~SCMem(void)
{
  if (save_area) delete[] save_area;
  if (hist_area) delete[] hist_area;
}


size_t SCMem::GetOffset(void) const
{
  return offset;
}


void SCMem::SetOffset(size_t new_offset)
{
#if _SC_VALIDATION_DEBUG
  if (new_offset > save_area_size)
  {
    std::cerr << "SCMem::SetOffset(): new offset behind save area size: " << new_offset << std::endl;
    std::cerr << "SCMem::SetOffset(): save area size: " << save_area_size << std::endl;
  }
#endif

  assert(new_offset <= save_area_size);

  offset = new_offset;
}


void SCMem::SkipOffset (size_t new_offset)
{
  assert(offset + new_offset <= save_area_size);

  offset += new_offset;
}


void SCMem::SkipHistoryOffset (size_t new_offset)
{
  assert(hist_offset + new_offset <= hist_area_size);

  hist_offset += new_offset;
}


void * SCMem::GetSaveArea(void) const
{
  return save_area;
}


size_t SCMem::GetSize(void) const
{
  return save_area_size;
}


size_t SCMem::GetHistoryOffset(void) const
{
  return hist_offset;
}


void SCMem::SetHistoryOffset(size_t new_offset)
{
  assert(new_offset <= hist_area_size);

  hist_offset = new_offset;
}


void * SCMem::GetHistoryArea(void) const
{
  return hist_area;
}


size_t SCMem::GetHistorySize(void) const
{
  return hist_area_size;
}


void SCMem::Store(const void *const address, size_t size)
{
#if _SC_VALIDATION_DEBUG
  if (size > save_area_size - offset)
  {
    std::cerr << "SCMem::Store(): size of new segment too large: " << size << std::endl;
    std::cerr << "SCMem::Store(): space left: " << save_area_size - offset << std::endl;
  }
#endif

  assert(size <= save_area_size - offset);

#if _SC_DMALLOC
  memcpy ((char *)((size_t)save_area + offset), (const char *)address, size);
#else
  memcpy ((void *)((size_t)save_area + offset), address, size);
#endif

  offset += size;  
}


void SCMem::Restore(const void *const address, size_t size)
{
#if _SC_VALIDATION_DEBUG
  if (size > save_area_size - offset)
  {
    std::cerr << "SCMem::Restore(): size of new segment too large: " << size << std::endl;
    std::cerr << "SCMem::Restore(): save area size: " << save_area_size << std::endl;
    std::cerr << "SCMem::Restore(): current offset: " << offset << std::endl;
    std::cerr << "SCMem::Restore(): space left: " << save_area_size - offset << std::endl;
  }
#endif

  assert(size <= save_area_size - offset);

#if _SC_DMALLOC
  memcpy ((char *)address, (const char *)((size_t)save_area + offset), size);
#else
  memcpy ((void *)address, (const void *)((size_t)save_area + offset), size);
#endif

  offset += size;
}


void SCMem::HistoryStore(const void *const address, size_t size)
{
#if _SC_VALIDATION_DEBUG
  if (size > hist_area_size - hist_offset)
  {
    std::cerr << "SCMem::HistoryStore(): size of new segment too large: " << size << std::endl;
    std::cerr << "SCMem::HistoryStore(): space left: " << hist_area_size - hist_offset << std::endl;
  }
#endif

  assert(size <= hist_area_size - hist_offset);

#if _SC_DMALLOC
  memcpy((char *)((size_t)hist_area + hist_offset), (const char *)address, size);
#else
  memcpy((void *)((size_t)hist_area + hist_offset), address, size);
#endif

  hist_offset += size;  
}


void SCMem::HistoryRestore(const void *const address, size_t size)
{
#if _SC_VALIDATION_DEBUG
  if (size > hist_area_size - hist_offset)
  {
    std::cerr << "SCMem::HistoryRestore(): size of new segment too large: " << size << std::endl;
    std::cerr << "SCMem::HistoryRestore(): space left: " << hist_area_size - hist_offset << std::endl;
  }
#endif

  assert(size <= hist_area_size - hist_offset);

#if _SC_DMALLOC
  memcpy((char *)address, (const char *)((size_t)hist_area + hist_offset), size);
#else
  memcpy((void *)address, (const void *)((size_t)hist_area + hist_offset), size);
#endif

  hist_offset += size;
}


void SCMem::Append(SCMem &source,
                   size_t source_offset, size_t source_length,
                   size_t source_hist_offset, size_t source_hist_length)
{
  void * dest_address;
  void * dest_hist_address;

  assert(offset + source_length <= save_area_size);
  assert(hist_offset + source_hist_length <= hist_area_size);

  source.SetOffset(source_offset);
  source.SetHistoryOffset(source_hist_offset);

  dest_address = (void *)((char *)save_area + offset);
  dest_hist_address = (void *)((char *)hist_area + hist_offset);

  source.Restore(dest_address, source_length);
  source.HistoryRestore(dest_hist_address, source_hist_length);

  offset += source_length;
  hist_offset += source_hist_length;
}


SCBoolean SCMem::operator==(const SCMem &second) const
{
  if (save_area_size != second.save_area_size)
    return false;
    
#if _SC_DMALLOC
  return !memcmp((const char *)save_area,
                 (const char *)second.save_area,
                 save_area_size);
#else
  return !memcmp((const void *)save_area,
                 (const void *)second.save_area,
                 save_area_size);
#endif  
}

SCBoolean SCMem::operator!=(const SCMem &second) const
{
  return !(*this == second);
}


SCStream& operator<< (SCStream& pStream, const SCMem& pData)
{
  pStream << "Size           : " << pData.GetSize() << std::endl;
  pStream << "History Size   : " << pData.GetHistorySize() << std::endl;
  pStream << "Offset         : " << pData.GetOffset() << std::endl;
  pStream << "History Offset : " << pData.GetHistoryOffset() << std::endl;

  return pStream;
}
