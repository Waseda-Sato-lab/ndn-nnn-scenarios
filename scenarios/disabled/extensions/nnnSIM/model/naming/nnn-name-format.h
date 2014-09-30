/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  uri.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  uri.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with uri.h.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Special thanks to University of California for initial implementation
 */

#ifndef NNN_DETAIL_URI_H
#define NNN_DETAIL_URI_H

#include <boost/archive/iterators/transform_width.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include "error.h"
#include "../nnn-common.h"

NNN_NAMESPACE_BEGIN

namespace detail
{

static const bool ESCAPE_CHARACTER [256] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 26
  1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 53
  0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 80
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 107
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, // 134
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 161
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 188
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 215
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 242
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 // 255
};

/// @cond include_hidden
template<class CharType>
struct hex_from_4_bit
{
  typedef CharType result_type;
  CharType operator () (CharType ch) const
  {
    const char lookup_table [16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    // cout << "New character: " << (int) ch << " (" << (char) ch << ")" << "\n";
    BOOST_ASSERT (ch < 16);
    return lookup_table[static_cast<size_t>(ch)];
  }
};

typedef boost::transform_iterator<hex_from_4_bit<std::string::const_iterator::value_type>,
                                  boost::archive::iterators::transform_width<std::string::const_iterator, 4, 8, std::string::const_iterator::value_type> > string_from_binary;



template<class CharType>
struct hex_to_4_bit
{
  typedef CharType result_type;
  CharType operator () (CharType ch) const
  {
    const signed char lookup_table [] = {
      -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
      -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
      -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9,-1,-1,-1,-1,-1,-1,
      -1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,
      -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
      -1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,
      -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
    };

    signed char value = -1;
    if ((unsigned)ch < 128)
      value = lookup_table [(unsigned)ch];
    if (value == -1)
      boost::throw_exception (error::StringTransform ());

    return value;
  }
};

typedef boost::archive::iterators::transform_width<boost::transform_iterator<hex_to_4_bit<std::string::const_iterator::value_type>, std::string::const_iterator>, 8, 4> string_to_binary;
/// @endcond

} // detail

/**
 * @brief A helper class to convert to/from URI
 */
class Uri
{
public:
  template<class Iterator1, class Iterator2>
  inline static void
  fromEscaped (Iterator1 begin, Iterator1 end, Iterator2 inserter)
  {
    Iterator1 i = begin;
    while (i != end)
      {
        if (*i == '%')
          {
            try
              {
                ++i;
                Iterator1 j = i;
                advance (i, 2);

                std::copy (detail::string_to_binary (j), detail::string_to_binary (i), inserter);
              }
            catch (nnn::error::StringTransform &e)
              {
                BOOST_THROW_EXCEPTION (error::NNNAddress ()
                                       << error::pos (std::distance (i, begin)));
              }
          }
        else if (!detail::ESCAPE_CHARACTER[static_cast<unsigned char> (*i)])
          {
            *inserter = *i;
            ++inserter; ++i;
          }
        else
          {
            BOOST_THROW_EXCEPTION (error::NNNAddress ()
                                   << error::pos (std::distance (i, begin)));
          }
      }
  }

  template<class Iterator1, class Iterator2>
  inline static void
  toEscaped (Iterator1 begin, Iterator1 end, Iterator2 inserter)
  {
    const char lookup_table [16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

    for (Iterator1 i = begin; i != end; i++)
      {
        if (detail::ESCAPE_CHARACTER[static_cast<unsigned char> (*i)])
          {
            *inserter = '%';         ++inserter;
            *inserter = lookup_table [(*i >> 4) & 0x0F]; ++inserter;
            *inserter = lookup_table [(*i & 0x0F)];      ++inserter;
          }
        else
          {
            *inserter = *i; ++inserter;
          }
      }
  }
};

NNN_NAMESPACE_END

#endif // NNN_DETAIL_URI_H
