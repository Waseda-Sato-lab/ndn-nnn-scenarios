/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  error.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  error.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with error.h.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Special thanks to University of California for initial implementation
 */

/**
 * @file error.h
 * @brief This file defines basic elements for the library reporting
 *
 * The library throws a number of exceptions.
 * In general, the following example shows how to print out diagnostic information
 * when one of the exceptions is thrown
 * @code
 *     try
 *       {
 *         ... operations with ndn::Name
 *       }
 *     catch (boost::exception &e)
 *       {
 *         std::cerr << boost::diagnostic_information (e) << std::endl;
 *       }
 * @endcode
 */

#ifndef NNN_ERROR2_H
#define NNN_ERROR2_H

#include <boost/exception/all.hpp>
#include <boost/throw_exception.hpp>

#include "../nnn-common.h"

NNN_NAMESPACE_BEGIN

/*
 * @brief Namespace holding all errors from NDN.cxx API
 */
namespace error
{

struct Error           : public virtual boost::exception, public virtual std::exception {}; ///< @brief Some error with error reporting engine
struct NNNAddress      : public virtual boost::exception, public virtual std::exception {}; ///< @brief An error with URI processing
struct StringTransform : public virtual boost::exception, public virtual std::exception {};
struct Name            : public virtual boost::exception, public virtual std::exception {}; ///< @brief An error with Name
namespace name {
struct Component       : public virtual boost::exception, public virtual std::exception {}; ///< @brief An error with name::Component
}

// Diagnostic information fields

/**
 * @brief Free-formatted text message explaining the error
 *
 * @code
 * ...
 * catch (boost::exception &e)
 * {
 *     if (const std::string *error = boost::get_error_info<error::msg> (e))
 *          ...
 * }
 * @endcode
 *
 * @see get_msg
 */
typedef boost::error_info<struct tag_msg, std::string> msg;

/**
 * @brief Helper method to get error message from the exception
 *
 * Method assumes that message is present, if not, an exception will be thrown
 */
inline const std::string &
get_msg (boost::exception &e)
{
  const std::string *error = boost::get_error_info<msg> (e);
  if (error == 0)
    BOOST_THROW_EXCEPTION (Error ());
  return *error;
}

/**
 * @brief Report of the position of the error (error-specific meaning)
 *
 * @code
 * ...
 * catch (boost::exception &e)
 * {
 *     if (const int *error = boost::get_error_info<error::pos> (e))
 *          ...
 * }
 * @endcode
 *
 * @see get_pos
 */
typedef boost::error_info<struct tag_pos, int> pos;

/**
 * @brief Helper method to get position of the error from the exception
 *
 * Method assumes that position is present, if not, an exception will be thrown
 */
inline int
get_pos (boost::exception &e)
{
  const int *position = boost::get_error_info<pos> (e);
  if (position == 0)
    BOOST_THROW_EXCEPTION (Error ());
  return *position;
}

} // error

NNN_NAMESPACE_END

#endif // NNN_ERROR2_H
