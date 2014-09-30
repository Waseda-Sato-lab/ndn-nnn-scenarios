/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-address.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-address.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-address.h.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef NNN_ADDRESS_H
#define NNN_ADDRESS_H

#include <iostream>

#include <ns3-dev/ns3/simple-ref-count.h>
#include <ns3-dev/ns3/attribute.h>
#include <ns3-dev/ns3/attribute-helper.h>

#include "../nnn-common.h"
#include "name-component.h"

#define SEP '.'

NNN_NAMESPACE_BEGIN

/**
 * @brief Class for NNN Address
 */
class NNNAddress : public SimpleRefCount<NNNAddress>
{
public:
	typedef std::vector<name::Component>::iterator iterator;
	typedef std::vector<name::Component>::const_iterator const_iterator;
	typedef std::vector<name::Component>::reverse_iterator reverse_iterator;
	typedef std::vector<name::Component>::const_reverse_iterator const_reverse_iterator;
	typedef std::vector<name::Component>::reference reference;
	typedef std::vector<name::Component>::const_reference const_reference;

	typedef name::Component partial_type;
	///////////////////////////////////////////////////////////////////////////////
	//                              CONSTRUCTORS                                 //
	///////////////////////////////////////////////////////////////////////////////

	/**
	 * @brief Default constructor to create an NNN Address
	 */
	NNNAddress ();

	/**
	 * @brief Copy constructor
	 *
	 * @param other reference to a NNN Address object
	 */
	NNNAddress (const NNNAddress &other);

	/**
	 * @brief Create a name from string
	 *
	 * @param name Dot separated address
	 */
	NNNAddress (const std::string &name);

	/**
	 * @brief Create a name from a vector of name::Components
	 *
	 * @param name vector of name::Components
	 */
	NNNAddress (const std::vector<name::Component> name);

	/**
	 * @brief Assignment operator
	 */
	NNNAddress &
	operator= (const NNNAddress &other);


	///////////////////////////////////////////////////////////////////////////////
	//                                SETTERS                                    //
	///////////////////////////////////////////////////////////////////////////////

	/**
	 * @brief Append components from another ndn::Name object
	 *
	 * @param comp reference to Name object
	 * @returns reference to self (to allow chaining of append methods)
	 */
	inline NNNAddress &
	append (const name::Component &comp);

	/**
	 * @brief Append a string as a name component
	 *
	 * @param compStr a string
	 * @returns reference to self (to allow chaining of append methods)
	 *
	 * No conversions will be done to the string.  The string is included in raw form,
	 * without any leading '\0' symbols.
	 */
	inline NNNAddress &
	append (const std::string &compStr);

	/**
	 * @brief Append a binary blob as a name component
	 * @param comp a binary blob
	 *
	 * This version is a little bit more efficient, since it swaps contents of comp and newly added component
	 *
	 * Attention!!! This method has an intended side effect: content of comp becomes empty
	 */
	inline NNNAddress &
	appendBySwap (name::Component &comp);

	/**
	 * @brief Append a binary blob as a name component
	 *
	 * @param buf pointer to the first byte of the binary blob
	 * @param size length of the binary blob
	 * @returns reference to self (to allow chaining of append methods)
	 */
	inline NNNAddress &
	append (const void *buf, size_t size);

	/**
	 * @brief Get number of the name components
	 * @return number of name components
	 */
	inline size_t
	size () const;

	/**
	 * @brief Get binary blob of name component
	 * @param index index of the name component.  If less than 0, then getting component from the back:
	 *              get(-1) getting the last component, get(-2) is getting second component from back, etc.
	 * @returns const reference to binary blob of the requested name component
	 *
	 * If index is out of range, an exception will be thrown
	 */
	const name::Component &
	get (int index) const;

	/**
	 * @brief Get binary blob of name component
	 * @param index index of the name component.  If less than 0, then getting component from the back
	 * @returns reference to binary blob of the requested name component
	 *
	 * If index is out of range, an exception will be thrown
	 */
	name::Component &
	get (int index);

	/////
	///// Iterator interface to name components
	/////
	inline NNNAddress::const_iterator
	begin () const;           ///< @brief Begin iterator (const)

	inline NNNAddress::iterator
	begin ();                 ///< @brief Begin iterator

	inline NNNAddress::const_iterator
	end () const;             ///< @brief End iterator (const)

	inline NNNAddress::iterator
	end ();                   ///< @brief End iterator

	inline NNNAddress::const_reverse_iterator
	rbegin () const;          ///< @brief Reverse begin iterator (const)

	inline NNNAddress::reverse_iterator
	rbegin ();                ///< @brief Reverse begin iterator

	inline NNNAddress::const_reverse_iterator
	rend () const;            ///< @brief Reverse end iterator (const)

	inline NNNAddress::reverse_iterator
	rend ();                  ///< @brief Reverse end iterator

	/////
	///// Static helpers to convert name component to appropriate value
	/////

	/*
	 * @brief Obtain an address minus the ending component
	 */
	NNNAddress
	getSectorName () const;

	/**
	 * @brief Get text representation of the name (Dot notation)
	 */
	std::string
	toDotHex () const;

	/**
	 * @brief Write name in Dot notation to the specified output stream
	 * @param os output stream
	 */
	void
	toDotHex (std::ostream &os) const;

	/////////////////////////////////////////////////
	// Helpers and compatibility wrappers
	/////////////////////////////////////////////////

	/**
	 * @brief Compare two names, using canonical ordering for each component
	 * @return 0  They compare equal
	 *         <0 If *this comes before other in the canonical ordering
	 *         >0 If *this comes after in the canonical ordering
	 */
	int
	compare (const NNNAddress &name) const;

	/**
	 * @brief Find out if two NNN addresses belong to the same sector
	 * @return True or False
	 */
	bool
	isSameSector (const NNNAddress &name) const;

	/**
	 * @brief Find out if NNN address is a top level address
	 * @return True or False
	 */
	bool
	isToplvlSector () const;

	/**
	 * @brief Find out if NNN address is empty
	 * @return True or False
	 */
	bool
	isEmpty () const;

	/*
	 * @brief Obtain the closest address common to the NNN addresses used
	 */
	NNNAddress
	getClosestSector (const NNNAddress &name) const;

	/**
	 * @brief Check if 2 NNNAddress objects are equal
	 */
	inline bool
	operator == (const NNNAddress &name) const;

	/**
	 * @brief Check if 2 NNNAddress objects are not equal
	 */
	inline bool
	operator != (const NNNAddress &name) const;

	/**
	 * @brief Less or equal comparison of 2 NNNAddress objects
	 */
	inline bool
	operator <= (const NNNAddress &name) const;

	/**
	 * @brief Less comparison of 2 NNNAddress objects
	 */
	inline bool
	operator < (const NNNAddress &name) const;

	/**
	 * @brief Great or equal comparison of 2 NNNAddress objects
	 */
	inline bool
	operator >= (const NNNAddress &name) const;

	/**
	 * @brief Great comparison of 2 NNNAddress objects
	 */
	inline bool
	operator > (const NNNAddress &name) const;

	/**
	 * @brief Operator [] to simplify access to name components
	 * @see get
	 */
	inline name::Component &
	operator [] (int index);

	/**
	 * @brief Operator [] to simplify access to name components
	 * @see get
	 */
	inline const name::Component &
	operator [] (int index) const;

	/**
	 * @brief Create a new Name object, by copying components from first and second name
	 */
	NNNAddress
	operator + (const NNNAddress &name) const;


public:
	// Data Members (public):
	///  Value returned by various member functions when they fail.
	const static size_t npos = static_cast<size_t> (-1);
	const static uint64_t nversion = static_cast<uint64_t> (-1);

private:
	std::vector<name::Component> m_address_comp;
};

inline std::ostream &
operator << (std::ostream &os, const NNNAddress &name)
{
	name.toDotHex (os);
	return os;
}

inline std::istream &
operator >> (std::istream &is, NNNAddress &name)
{
	std::string line;
	is >> line;
	name = NNNAddress (line);

	return is;
}

/////
///// Iterator interface to name components
/////
inline NNNAddress::const_iterator
NNNAddress::begin () const
{
  return m_address_comp.begin ();
}

inline NNNAddress::iterator
NNNAddress::begin ()
{
  return m_address_comp.begin ();
}

inline NNNAddress::const_iterator
NNNAddress::end () const
{
  return m_address_comp.end ();
}

inline NNNAddress::iterator
NNNAddress::end ()
{
  return m_address_comp.end ();
}

inline NNNAddress::const_reverse_iterator
NNNAddress::rbegin () const
{
  return m_address_comp.rbegin ();
}

inline NNNAddress::reverse_iterator
NNNAddress::rbegin ()
{
  return m_address_comp.rbegin ();
}

inline NNNAddress::const_reverse_iterator
NNNAddress::rend () const
{
  return m_address_comp.rend ();
}


inline NNNAddress::reverse_iterator
NNNAddress::rend ()
{
  return m_address_comp.rend ();
}


/////////////////////////////////////////////////////////////////////////////////////
// Definition of inline methods
/////////////////////////////////////////////////////////////////////////////////////

inline NNNAddress &
NNNAddress::append (const name::Component &comp)
{
  if (comp.size () != 0)
	  m_address_comp.push_back (comp);
  return *this;
}

inline NNNAddress &
NNNAddress::appendBySwap (name::Component &comp)
{
  if (comp.size () != 0)
    {
	  NNNAddress::iterator newComp = m_address_comp.insert (end (), name::Component ());
      newComp->swap (comp);
    }
  return *this;
}

NNNAddress &
NNNAddress::append (const void *buf, size_t size)
{
  name::Component comp (buf, size);
  return appendBySwap (comp);
}

inline bool
NNNAddress::operator ==(const NNNAddress &name) const
{
	return (compare (name) == 0);
}

inline bool
NNNAddress::operator !=(const NNNAddress &name) const
{
	return (compare (name) != 0);
}

inline bool
NNNAddress::operator <= (const NNNAddress &name) const
{
	return (compare (name) <= 0);
}

inline bool
NNNAddress::operator < (const NNNAddress &name) const
{
	return (compare (name) < 0);
}

inline bool
NNNAddress::operator >= (const NNNAddress &name) const
{
	return (compare (name) >= 0);
}

inline bool
NNNAddress::operator > (const NNNAddress &name) const
{
	return (compare (name) > 0);
}

inline name::Component &
NNNAddress::operator [] (int index)
{
  return get (index);
}

inline const name::Component &
NNNAddress::operator [] (int index) const
{
  return get (index);
}

ATTRIBUTE_HELPER_HEADER (NNNAddress);

NNN_NAMESPACE_END

#endif
