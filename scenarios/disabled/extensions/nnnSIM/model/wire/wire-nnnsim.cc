/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  wire-nnnsim.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  wire-nnnsim.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with wire-nnnsim.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "wire-nnnsim.h"
#include <boost/foreach.hpp>

NNN_NAMESPACE_BEGIN

namespace wire {

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

size_t
NnnSim::SerializeName (Buffer::Iterator &i, const NNNAddress &name)
{
	Buffer::Iterator start = i;

	i.WriteU16 (static_cast<uint16_t> (SerializedSizeName (name)-2));

	for (NNNAddress::const_iterator item = name.begin ();
			item != name.end ();
			item++)
	{
		i.WriteU16 (static_cast<uint16_t> (item->size ()));
		i.Write (reinterpret_cast<const uint8_t*> (item->buf ()), item->size ());
	}

	return i.GetDistanceFrom (start);
}

size_t
NnnSim::SerializedSizeName (const NNNAddress &name)
{
	size_t nameSerializedSize = 2;

	for (NNNAddress::const_iterator i = name.begin ();
			i != name.end ();
			i++)
	{
		nameSerializedSize += 2 + i->size ();
	}
	return nameSerializedSize;
}

Ptr<NNNAddress>
NnnSim::DeserializeName (Buffer::Iterator &i)
{
	Ptr<NNNAddress> name = Create<NNNAddress> ();

	uint16_t nameLength = i.ReadU16 ();
	while (nameLength > 0)
	{
		uint16_t length = i.ReadU16 ();
		nameLength = nameLength - 2 - length;

		uint8_t tmp[length];
		i.Read (tmp, length);

		name->append (tmp, length);
	}

	return name;
}
}

NNN_NAMESPACE_END
