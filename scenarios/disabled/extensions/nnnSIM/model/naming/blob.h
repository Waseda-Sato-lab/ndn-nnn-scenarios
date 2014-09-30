/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  blob.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  blob.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with blob.h.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Special thanks to University of California for initial implementation
 */

#ifndef NNN_BLOB_H
#define NNN_BLOB_H

#include <vector>

#include "../nnn-common.h"

NNN_NAMESPACE_BEGIN

/**
 * @brief Class representing a general-use binary blob
 */
class Blob
{
public:
  typedef std::vector<char> base;

  typedef base::value_type             value_type;
  typedef base::pointer                pointer;
  typedef base::const_pointer          const_pointer;
  typedef base::reference              reference;
  typedef base::const_reference        const_reference;
  typedef base::iterator               iterator;
  typedef base::const_iterator         const_iterator;
  typedef base::const_reverse_iterator const_reverse_iterator;
  typedef base::reverse_iterator       reverse_iterator;
  typedef base::size_type              size_type;
  typedef base::difference_type        difference_type;
  typedef base::allocator_type         allocator_type;

public:
  /**
   * @brief Creates an empty blob
   */
  Blob ()
  {
  }

  Blob (const std::string &data)
    : m_data (data.begin (), data.end ())
  {
  }

  Blob (const void *buf, size_t length)
    : m_data (reinterpret_cast<const char*> (buf), reinterpret_cast<const char*> (buf) + length)
  {
  }

  /**
   * @brief Get pointer to the first byte of the binary blob
   */
  inline char*
  buf ()
  {
    return &m_data.front ();
  }

  /**
   * @brief Get const pointer to the first byte of the binary blob
   */
  inline const char*
  buf () const
  {
    return &m_data.front ();
  }

  iterator begin () { return m_data.begin (); }
  const_iterator begin () const { return m_data.begin (); }
  iterator end () { return m_data.end (); }
  const_iterator end () const { return m_data.end (); }
  size_t size () const { return m_data.size (); }

  void swap (Blob &x) { m_data.swap (x.m_data); }
  void push_back (value_type val) { m_data.push_back (val); }

  bool empty () const { return m_data.empty (); }

  Blob &
  operator = (const Blob &other) { m_data = other.m_data; return *this; }

  reference operator [] (size_type pos) { return m_data [pos]; }
  const_reference operator [] (size_type pos) const { return m_data [pos]; }

  char getItem (size_type pos) const { return m_data [pos]; }

  void clear () { m_data.clear (); }

private:
  friend bool operator == (const Blob &a, const Blob &b);
  friend bool operator <  (const Blob &a, const Blob &b);
  friend bool operator <= (const Blob &a, const Blob &b);
  friend bool operator >  (const Blob &a, const Blob &b);
  friend bool operator >= (const Blob &a, const Blob &b);

private:
  std::vector< char > m_data;
};

inline bool operator == (const Blob &a, const Blob &b)  { return a.m_data == b.m_data; }
inline bool operator <  (const Blob &a, const Blob &b)  { return a.m_data <  b.m_data; }
inline bool operator <= (const Blob &a, const Blob &b)  { return a.m_data <= b.m_data; }
inline bool operator >  (const Blob &a, const Blob &b)  { return a.m_data >  b.m_data; }
inline bool operator >= (const Blob &a, const Blob &b)  { return a.m_data >= b.m_data; }

NNN_NAMESPACE_END

#include <boost/functional/hash.hpp>
namespace boost
{
inline std::size_t
hash_value (const ns3::nnn::Blob v)
{
  return boost::hash_range (v.begin(), v.end());
}
}

#endif // NNN_BLOB_H
