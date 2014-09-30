/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  name-component.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  name-component.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with name-component.h.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Special thanks to University of California for initial implementation
 */

#ifndef NNN_NAME_COMPONENT_H
#define NNN_NAME_COMPONENT_H

#include "blob.h"

NNN_NAMESPACE_BEGIN

namespace name {

/**
 * @brief Class to representing binary blob of NDN name component
 *
 * This class is based on Blob (std::vector<char>) and just provides several helpers
 * to work with name components, as well as operator to apply canonical
 * ordering on name components
 */
class Component : public Blob
{
public:
	/**
	 * @brief Default constructor an empty exclude
	 */
	Component ();

	/**
	 * @brief Create component from URI encoded string
	 * @param uri URI encoded name component (convert escaped with % characters)
	 */
	Component (const std::string &uri);

	/**
	 * @brief Create component from URI encoded string, with string specified by a pair of iterators
	 * @param begin begin iterator pointing to the URI encoded name
	 * @param end end iterator
	 */
	Component (std::string::const_iterator begin, std::string::const_iterator end);

	/**
	 * @brief Create component using a binary blob
	 * @param buf pointer to first byte of binary blob to store as a name component
	 * @param length length of the binary blob
	 */
	Component (const void *buf, size_t length);

	/**
	 * @brief Apply canonical ordering on component comparison
	 * @return 0  They compare equal
	 *         <0 If *this comes before other in the canonical ordering
	 *         >0 If *this comes after in the canonical ordering
	 *
	 * @see http://www.ccnx.org/releases/latest/doc/technical/CanonicalOrder.html
	 */
	int
	compare (const Component &other) const;

	/**
	 * @brief Apply canonical ordering on component comparison (less or equal)
	 *
	 * @see http://www.ccnx.org/releases/latest/doc/technical/CanonicalOrder.html
	 */
	inline bool
	operator <= (const Component &other) const;

	/**
	 * @brief Apply canonical ordering on component comparison (less)
	 *
	 * @see http://www.ccnx.org/releases/latest/doc/technical/CanonicalOrder.html
	 */
	inline bool
	operator < (const Component &other) const;

	/**
	 * @brief Apply canonical ordering on component comparison (greater or equal)
	 *
	 * @see http://www.ccnx.org/releases/latest/doc/technical/CanonicalOrder.html
	 */
	inline bool
	operator >= (const Component &other) const;

	/**
	 * @brief Apply canonical ordering on component comparison (greater)
	 *
	 * @see http://www.ccnx.org/releases/latest/doc/technical/CanonicalOrder.html
	 */
	inline bool
	operator > (const Component &other) const;

	////////////////////////////////////
	// Component construction helpers //
	////////////////////////////////////

	/**
	 * @brief Create component from URI encoded string
	 * @param uri URI encoded name component (convert escaped with % characters)
	 * @return *this
	 */
	Component &
	fromUri (const std::string &uri);

	/**
	 * @brief Create component from URI encoded string, with string specified by a pair of iterators
	 * @param begin begin iterator pointing to the URI encoded name
	 * @param end end iterator
	 * @return *this
	 */
	Component &
	fromUri (std::string::const_iterator begin, std::string::const_iterator end);

	/**
	 * @brief Create network-ordered numeric component
	 *
	 * @param number number to be encoded and added as a component
	 * @return *this
	 *
	 * Number is encoded and added in network order. Tail zero-bytes are not included.
	 * For example, if the number is 1, then 1-byte binary blob will be added  0x01.
	 * If the number is 256, then 2 binary blob will be added: 0x01 0x01
	 *
	 * If the number is zero, an empty component will be created
	 */
	Component &
	fromNumber (uint64_t number);

	/**
	 * @brief Create network-ordered numeric component from string
	 *
	 * @param begin begin iterator pointing to dot separated NNN string
	 * @param end end iterator
	 * @return *this
	 *
	 * Number is encoded and added in network order. Tail zero-bytes are not included.
	 * For example, if the number is 1, then 1-byte binary blob will be added  0x01.
	 * If the number is 256, then 2 binary blob will be added: 0x01 0x01
	 *
	 * If the number is zero, an empty component will be created
	 */
	Component &
	fromDotHexStr (std::string::const_iterator begin, std::string::const_iterator end);

	/**
	 * @brief Create network-ordered numeric component to the name with marker
	 *
	 * @param number number to be encoded and added as a component
	 * @param marker byte marker, specified by the desired naming convention
	 *
	 * Currently defined naming conventions of the marker:
	 * - 0x00  sequence number
	 * - 0xC1  control number
	 * - 0xFB  block id
	 * - 0xFD  version number
	 *
	 * This version is almost exactly as appendNumber, with exception that it adds initial marker.
	 * The number is formatted in the exactly the same way.
	 *
	 * @see fromNumber
	 */
	Component &
	fromNumberWithMarker (uint64_t number, unsigned char marker);

	//////////////////////////////////////////////////////////////////////////////////
	// helpers
	//////////////////////////////////////////////////////////////////////////////////

	/**
	 * @brief Convert binary blob name component to std::string (no conversion is made)
	 * @param comp name component to be converted
	 * @see asUriString
	 */
	std::string
	toBlob () const;

	/**
	 * @brief Write blob of the name component to the specified output stream
	 * @param os output stream
	 */
	void
	toBlob (std::ostream &os) const;

	/**
	 * @brief Convert binary blob name component to std::string, escaping all non-printable characters in URI format
	 * @param comp name component to be converted
	 * @see asString
	 */
	std::string
	toUri () const;

	/**
	 * @brief Write name as URI to the specified output stream
	 * @param os output stream
	 */
	void
	toUri (std::ostream &os) const;

	/**
	 * @brief Convert binary blob name component to std::string
	 * @param comp name component to be converted
	 * @see asString
	 */
	std::string
	toHex () const;

	/**
	 * @brief Write name in NNN DotHex format
	 * @param os output stream
	 */
	void
	toHex (std::ostream &os) const;

	/**
	 * @brief Convert binary blob name component (network-ordered number) to number
	 * @param comp name component to be converted
	 */
	uint64_t
	toNumber () const;

	/**
	 * @brief Convert binary blob name component (network-ordered number) to number, using appropriate marker from the naming convention
	 * @param comp name component to be converted
	 * @param marker required marker from the naming convention
	 *
	 * If the required marker does not exist, an exception will be thrown
	 */
	uint64_t
	toNumberWithMarker (unsigned char marker) const;

	/**
	 * @brief Convert binary blob name component, assuming sequence number naming convention (marker = 0x00)
	 * @param comp name component to be converted
	 * @see asNumberWithMarker
	 */
	inline uint64_t
	toSeqNum () const;

	/**
	 * @brief Convert binary blob name component, assuming control number naming convention (marker = 0xC1)
	 * @param comp name component to be converted
	 * @see asNumberWithMarker
	 */
	inline uint64_t
	toControlNum () const;

	/**
	 * @brief Convert binary blob name component, assuming block ID naming convention (marker = 0xFB)
	 * @param comp name component to be converted
	 * @see asNumberWithMarker
	 */
	inline uint64_t
	toBlkId () const;

	/**
	 * @brief Convert binary blob name component, assuming time-stamping version naming convention (marker = 0xFD)
	 * @param comp name component to be converted
	 * @see asNumberWithMarker
	 */
	inline uint64_t
	toVersion () const;
};

bool
Component::operator <= (const Component &other) const
{
	return (compare (other) <= 0);
}

bool
Component::operator < (const Component &other) const
{
	return (compare (other) < 0);
}

bool
Component::operator >= (const Component &other) const
{
	return (compare (other) >= 0);
}

bool
Component::operator > (const Component &other) const
{
	return (compare (other) > 0);
}

inline uint64_t
Component::toSeqNum () const
{
	return toNumberWithMarker (0x00);
}

inline uint64_t
Component::toControlNum () const
{
	return toNumberWithMarker (0xC1);
}

inline uint64_t
Component::toBlkId () const
{
	return toNumberWithMarker (0xFB);
}

inline uint64_t
Component::toVersion () const
{
	return toNumberWithMarker (0xFD);
}

/**
 * @brief Stream output operator (output in escaped URI format)
 */
inline std::ostream&
operator <<(std::ostream &os, const Component &name)
{
	name.toHex (os);
	return os;
}

} // name

NNN_NAMESPACE_END

#endif // NNN_NAME_COMPONENT_H
