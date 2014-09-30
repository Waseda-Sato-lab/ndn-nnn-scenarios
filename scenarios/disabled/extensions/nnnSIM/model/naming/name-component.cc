/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  name-component.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  name-component.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with name-component.cc.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Special thanks to University of California for initial implementation
 */

#include "error.h"
#include "nnn-name-format.h"
#include "name-component.h"

NNN_NAMESPACE_BEGIN

namespace name {

Component::Component ()
{
}

Component::Component (const std::string &name)
{
  copy (name.begin (), name.end (), std::back_inserter (*this));
}

Component::Component (std::string::const_iterator begin, std::string::const_iterator end)
{
  copy (begin, end, std::back_inserter (*this));
}

Component::Component (const void *buf, size_t length)
{
  copy (static_cast<const char*> (buf),
        static_cast<const char*> (buf)+length,
        std::back_inserter (*this));
}

Component &
Component::fromUri (const std::string &uri)
{
  try
    {
      Uri::fromEscaped (uri.begin (), uri.end (), std::back_inserter (*this));
    }
  catch (error::NNNAddress &err)
    {
      // re-throwing different exception
      BOOST_THROW_EXCEPTION (error::name::Component ()
                             << error::msg (uri)
                             << error::pos (error::get_pos (err)));
    }

  return *this;
}

Component &
Component::fromUri (std::string::const_iterator begin, std::string::const_iterator end)
{
  try
    {
      Uri::fromEscaped (begin, end, std::back_inserter (*this));
    }
  catch (error::NNNAddress &err)
    {
      // re-throwing different exception
      BOOST_THROW_EXCEPTION (error::name::Component ()
                             << error::msg (std::string (begin, end))
                             << error::pos (error::get_pos (err)));
    }
  return *this;
}



int
Component::compare (const Component &other) const
{
  if (size () < other.size ())
    return -1;

  if (size () > other.size ())
    return +1;

  // now we know that sizes are equal

  std::pair<const_iterator, const_iterator> diff = mismatch (begin (), end (), other.begin ());
  if (diff.first == end ()) // components are actually equal
    return 0;

  //return (std::lexicographical_compare (diff.first, end (), diff.second, other.end ())) ? -1 : +1;

  // Our comparison are actually numerical, so we have to convert to integers
  uint64_t curr = toNumber();
  uint64_t otherNum = other.toNumber();

  return (curr > otherNum) ? +1 : -1;
}

Component &
Component::fromNumber (uint64_t number)
{
  while (number > 0)
    {
      this->push_back (static_cast<unsigned char> (number & 0xFF));
      number >>= 8;
    }
  std::reverse (this->begin (), this->end ());
  return *this;
}

Component &
Component::fromDotHexStr (std::string::const_iterator begin, std::string::const_iterator end)
{
	// Obtain the actual string given
	std::string tmp (begin, end);

	// Create a string stream
	std::stringstream str;

	// We assume that the string given is in hex and pass it to the stream
	str << tmp;

	// We then read the hex into value, an unsigned integer
	uint64_t value;
	str >> std::hex >> value;

	// Use the fromNumber function to do the heavy lifting
	return fromNumber(value);
}

Component &
Component::fromNumberWithMarker (uint64_t number, unsigned char marker)
{
  this->push_back (marker);

  while (number > 0)
    {
      this->push_back (static_cast<unsigned char> (number & 0xFF));
      number >>= 8;
    }

  std::reverse (this->begin () + 1, this->end ());
  return *this;
}

std::string
Component::toBlob () const
{
  return std::string (begin (), end ());
}

void
Component::toBlob (std::ostream &os) const
{
  os.write (buf (), size ());
}

std::string
Component::toUri () const
{
  std::ostringstream os;
  toUri (os);
  return os.str ();
}

void
Component::toUri (std::ostream &os) const
{
  const uint8_t* valuePtr = reinterpret_cast<const uint8_t*>(buf());
  size_t valueSize = size();

  bool gotNonDot = false;
  for (unsigned i = 0; i < valueSize; ++i) {
    if (valuePtr[i] != 0x2e) {
      gotNonDot = true;
      break;
    }
  }
  if (!gotNonDot) {
    // Special case for component of zero or more periods.  Add 3 periods.
    os << "...";
    for (size_t i = 0; i < valueSize; ++i)
      os << '.';
  }
  else {
    // In case we need to escape, set to upper case hex and save the previous flags.
    std::ios::fmtflags saveFlags = os.flags(std::ios::hex | std::ios::uppercase);

    for (size_t i = 0; i < valueSize; ++i) {
      uint8_t x = valuePtr[i];
      // Check for 0-9, A-Z, a-z, (+), (-), (.), (_)
      if ((x >= 0x30 && x <= 0x39) || (x >= 0x41 && x <= 0x5a) ||
          (x >= 0x61 && x <= 0x7a) || x == 0x2b || x == 0x2d ||
          x == 0x2e || x == 0x5f)
        os << x;
      else {
        os << '%';
        if (x < 16)
          os << '0';
        os << static_cast<unsigned int>(x);
      }
    }

    // Restore.
    os.flags(saveFlags);
  }
}

std::string
Component::toHex () const
{
  std::ostringstream os;
  toHex (os);
  return os.str ();
}

void
Component::toHex (std::ostream &os) const
{
	// Obtain the component information via toNumber
	uint64_t ret = toNumber();

	// Use std:hex to get the representation we want
	os << std::hex << ret;
}

uint64_t
Component::toNumber () const
{
  uint64_t ret = 0;
  for (const_iterator i = begin (); i != end (); i++)
    {
      ret <<= 8;
      ret |= static_cast<unsigned char> (*i);
    }
  return ret;
}

uint64_t
Component::toNumberWithMarker (unsigned char marker) const
{
  if (empty () ||
      static_cast<unsigned char> (*(begin ())) != marker)
    {
      BOOST_THROW_EXCEPTION (error::name::Component ()
                             << error::msg ("Name component does not have required marker [" + toUri () + "]"));
    }

  uint64_t ret = 0;
  for (const_iterator i = begin () + 1; i != end (); i++)
    {
      ret <<= 8;
      ret |= static_cast<unsigned char> (*i);
    }
  return ret;
}

} // name

NNN_NAMESPACE_END
