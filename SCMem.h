/*------------------------------------------------------------------------------+
|                                                                               |
|   SCL - Simulation Class Library                                              |
|                                                                               |
|   (c) 1994-98 Marc Diefenbruch, Wolfgang Textor                               |
|       University of Essen, Germany                                            |
|                                                                               |
+---------------+-------------------+---------------+-------------------+-------+
|   Module      |   File            |   Created     |   Project         |       |
+---------------+-------------------+---------------+-------------------+-------+
| SCMem         |   SCMem.h         | 10. Mar 1996  |   SCL             |       |
+---------------+-------------------+---------------+-------------------+-------+
|                                                                               |
|   Change Log                                                                  |
|                                                                               |
|   Nr.      Date        Description                                            |
|  -----    --------    ------------------------------------------------------- |
|   001                                                                         |
|   000     09.08.94    Neu angelegt                                            |
|                                                                               |
+------------------------------------------------------------------------------*/

/*  Lineal
000000000011111111112222222222333333333344444444445555555555666666666677777777778
012345678901234567890123456789012345678901234567890123456789012345678901234567890
*/


#ifndef __SCMEM__

#define __SCMEM__

#include "SCBasicTypes.h"

class SCMem
{
  public:
    SCMem(const SCSize &pSize);
   ~SCMem(void);

  public:
    size_t  GetOffset (void) const;
    size_t  GetHistoryOffset (void) const;
    void    SetOffset (size_t new_offset);
    void    SetHistoryOffset (size_t new_offset);
    void    SkipOffset (size_t new_offset);
    void    SkipHistoryOffset (size_t new_offset);
    void   *GetSaveArea(void) const;
    void   *GetHistoryArea(void) const;
    size_t  GetSize(void) const;
    size_t  GetHistorySize(void) const;
    void    Store(const void *const address, size_t size);
    void    Restore(const void *const address, size_t size);
    void    HistoryStore(const void *const address, size_t size);
    void    HistoryRestore(const void *const address, size_t size);
    void    Append(SCMem &source,
                   size_t source_offset, size_t source_length,
                   size_t source_hist_offset, size_t source_hist_length);

    SCBoolean operator== (const SCMem & second) const;
    SCBoolean operator!= (const SCMem & second) const;

  private:
//    void    *save_area;
    char    *save_area;
//    void    *hist_area;
    char    *hist_area;
    size_t   save_area_size;
    size_t   hist_area_size;
    size_t   offset;
    size_t   hist_offset;

  friend class SCHashTable;                            // Improves performance!
};

class SCStream & operator<< (class SCStream& pStream, const SCMem& pData);

#endif  // __SCBASICTYPES__
