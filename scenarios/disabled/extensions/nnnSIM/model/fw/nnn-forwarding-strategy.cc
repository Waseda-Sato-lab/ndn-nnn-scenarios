/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2011 University of California, Los Angeles
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author:  Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 *          Ilya Moiseenko <iliamo@cs.ucla.edu>
 */

#include "nnn-forwarding-strategy.h"

//#include "ns3/nnn-pit.h"
//#include "ns3/nnn-pit-entry.h"
#include "ns3/nnn-so.h"
#include "ns3/nnn-do.h"
//#include "ns3/nnn-pit.h"
#include "ns3/nnn-nnst.h"
//#include "ns3/nnn-content-store.h"
#include "ns3/nnn-face.h"

#include "ns3/assert.h"
#include "ns3/ptr.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/boolean.h"
#include "ns3/string.h"

//#include "ns3/nnnSIM/utils/nnn-fw-hop-count-tag.h"

#include <boost/ref.hpp>
#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/tuple/tuple.hpp>
namespace ll = boost::lambda;

namespace ns3 {
namespace nnn {

NS_OBJECT_ENSURE_REGISTERED (NNNForwardingStrategy);

NS_LOG_COMPONENT_DEFINE (NNNForwardingStrategy::GetLogName ().c_str ());

std::string
NNNForwardingStrategy::GetLogName ()
{
  return "nnn.fw";
}

TypeId NNNForwardingStrategy::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::nnn::fw::NNNForwardingStrategy")
    .SetGroupName ("Nnn")
    .SetParent<Object> ()

    ////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////
/*
    .AddTraceSource ("OutSOs",  "OutSOs",  MakeTraceSourceAccessor (&NNNForwardingStrategy::m_outSOs))
    .AddTraceSource ("InSOs",   "InSOs",   MakeTraceSourceAccessor (&NNNForwardingStrategy::m_inSOs))
    .AddTraceSource ("DropSOs", "DropSOs", MakeTraceSourceAccessor (&NNNForwardingStrategy::m_dropSOs))

    ////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////

    .AddTraceSource ("OutDO",  "OutDO",  MakeTraceSourceAccessor (&NNNForwardingStrategy::m_outDO))
    .AddTraceSource ("InDO",   "InDO",   MakeTraceSourceAccessor (&NNNForwardingStrategy::m_inDO))
    .AddTraceSource ("DropDO", "DropDO", MakeTraceSourceAccessor (&NNNForwardingStrategy::m_dropDO))

    ////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////

    .AddTraceSource ("SatisfiedSOs",  "SatisfiedSOs",  MakeTraceSourceAccessor (&NNNForwardingStrategy::m_satisfiedSOs))
    .AddTraceSource ("TimedOutSOs",   "TimedOutSOs",   MakeTraceSourceAccessor (&NNNForwardingStrategy::m_timedOutSOs))

    .AddAttribute ("CacheUnsolicitedSOFromApps", "Cache unsolicited DO that has been pushed from applications",
                   BooleanValue (true),
                   MakeBooleanAccessor (&NNNForwardingStrategy::m_cacheUnsolicitedDOFromApps),
                   MakeBooleanChecker ())
    
    .AddAttribute ("CacheUnsolicitedDO", "Cache overheard DO that have not been requested",
                   BooleanValue (false),
                   MakeBooleanAccessor (&NNNForwardingStrategy::m_cacheUnsolicitedDO),
                   MakeBooleanChecker ())

    .AddAttribute ("DetectRetransmissions", "If non-duplicate SO is received on the same face more than once, "
                                            "it is considered a retransmission",
                   BooleanValue (true),
                   MakeBooleanAccessor (&NNNForwardingStrategy::m_detectRetransmissions),
                   MakeBooleanChecker ())
    ;
*/
  return tid;
}

NNNForwardingStrategy::NNNForwardingStrategy ()
{
}

NNNForwardingStrategy::~NNNForwardingStrategy ()
{
}

void
NNNForwardingStrategy::NotifyNewAggregate ()
{
/*  if (m_pit == 0)
    {
      m_pit = GetObject<Pit> ();
    }*/
  if (m_nnst == 0)
    {
      m_nnst = GetObject<NNST> ();
    }
/*  if (m_contentStore == 0)
    {
      m_contentStore = GetObject<ContentStore> ();
    }*/

  Object::NotifyNewAggregate ();
}

void
NNNForwardingStrategy::DoDispose ()
{
//  m_pit = 0;
//  m_contentStore = 0;
  m_nnst = 0;

  Object::DoDispose ();
}

/*
void
NNNForwardingStrategy::OnSO (Ptr<Face> inFace,
                                Ptr<SO> so_p)
{
  NS_LOG_FUNCTION (inFace << so_p->GetName ());
  m_inSOs (so_p, inFace);

  Ptr<pit::Entry> pitEntry = m_pit->Lookup (*so_p);
  bool similarSO = true;
  if (pitEntry == 0)
    {
      similarSO = false;
      pitEntry = m_pit->Create (so_p);
      if (pitEntry != 0)
        {
          DidCreatePitEntry (inFace, so_p, pitEntry);
        }
      else
        {
          FailedToCreatePitEntry (inFace, so_p);
          return;
        }
    }

  bool isDuplicated = true;
  if (!pitEntry->IsNonceSeen (so_p->GetNonce ()))
    {
      pitEntry->AddSeenNonce (so_p->GetNonce ());
      isDuplicated = false;
    }

  if (isDuplicated)
    {
      DidReceiveDuplicateSO (inFace, so_p, pitEntry);
      return;
    }

  Ptr<DO> contentObject;
  contentObject = m_contentStore->Lookup (so_p);
  if (contentObject != 0)
    {
      FwHopCountTag hopCountTag;
      if (so_p->GetPayload ()->PeekPacketTag (hopCountTag))
        {
          contentObject->GetPayload ()->AddPacketTag (hopCountTag);
        }

      pitEntry->AddIncoming (inFace/*, Seconds (1.0)*/);

      // Do data plane performance measurements
     // WillSatisfyPendingSO (0, pitEntry);

      // Actually satisfy pending SO
      //SatisfyPendingSO (0, contentObject, pitEntry);
     // return;
   // }


  if (similarSO && ShouldSuppressIncomingSO (inFace, so_p, pitEntry))
    {
      pitEntry->AddIncoming (inFace/*, so->GetSOLifetime ()*/);
      // update PIT entry lifetime
      pitEntry->UpdateLifetime (so_p->GetSOLifetime ());

      // Suppress this SO if we're still expecting Data from some other face
      NS_LOG_DEBUG ("Suppress SOs");
      m_dropSOs (so_p, inFace);

      DidSuppressSimilarSO (inFace, so_p, pitEntry);
      return;
    }

  if (similarSO)
    {
      DidForwardSimilarSO (inFace, so_p, pitEntry);
    }

  PropagateSO (inFace, so_p, pitEntry);
}
*/
/*
void
NNNForwardingStrategy::OnDO (Ptr<Face> inFace,
                            Ptr<DO> do_p)
{
  NS_LOG_FUNCTION (inFace << do_p->GetName ());
  m_inDO (do_p, inFace);

  // Lookup PIT entry
  Ptr<pit::Entry> pitEntry = m_pit->Lookup (*do_p);
  if (pitEntry == 0)
    {
      bool cached = false;

      if (m_cacheUnsolicitedDO || (m_cacheUnsolicitedDOFromApps && (inFace->GetFlags () & Face::APPLICATION)))
        {
          // Optimistically add or update entry in the content store
          cached = m_contentStore->Add (do_p);
        }
      else
        {
          // Drop DO packet if PIT entry is not found
          // (unsolicited DO packets should not "poison" content store)

          //drop dulicated or not requested DO packet
          m_dropDO (do_p, inFace);
        }

      DidReceiveUnsolicitedDO (inFace, do_p, cached);
      return;
    }
  else
    {
      bool cached = m_contentStore->Add (do_p);
      DidReceiveSolicitedDO (inFace, do_p, cached);
    }

  while (pitEntry != 0)
    {
      // Do Data plane performance measurements
      WillSatisfyPendingSO (inFace, pitEntry);

      // Actually satisfy pending SO
      SatisfyPendingSO (inFace, do_p, pitEntry);

      // Lookup another PIT entry
      pitEntry = m_pit->Lookup (*do_p);
    }
}

void
NNNForwardingStrategy::DidCreatePitEntry (Ptr<Face> inFace,
                                       Ptr<const SO> so_p,
                                       Ptr<pit::Entry> pitEntrypitEntry)
{
}

void
NNNForwardingStrategy::FailedToCreatePitEntry (Ptr<Face> inFace,
                                            Ptr<const SO> so_p)
{
  m_dropSOs (so_p, inFace);
}

void
NNNForwardingStrategy::DidReceiveDuplicateSO (Ptr<Face> inFace,
                                                 Ptr<const SO> so_p,
                                                 Ptr<pit::Entry> pitEntry)
{
  /////////////////////////////////////////////////////////////////////////////////////////
  //                                                                                     //
  // !!!! IMPORTANT CHANGE !!!! Duplicate SOs will create incoming face entry !!!! //
  //                                                                                     //
  /////////////////////////////////////////////////////////////////////////////////////////
  pitEntry->AddIncoming (inFace);
  m_dropSOs (so_p, inFace);
}
*/
/*
void
NNNForwardingStrategy::DidSuppressSimilarSO (Ptr<Face> face,
                                                Ptr<const SO> so_p                                            Ptr<pit::Entry> pitEntry)
{
}

void
NNNForwardingStrategy::DidForwardSimilarSO (Ptr<Face> inFace,
                                               Ptr<const SO> so_p,
                                               Ptr<pit::Entry> pitEntry)
{
}
*/
/*
void
NNNForwardingStrategy::DidExhaustForwardingOptions (Ptr<Face> inFace,
                                                 Ptr<const SO> so_p,
                                                 Ptr<pit::Entry> pitEntry)
{
  NS_LOG_FUNCTION (this << boost::cref (*inFace));
  if (pitEntry->AreAllOutgoingInVain ())
    {
      m_dropSOs (so_p, inFace);

      // All incoming SOs cannot be satisfied. Remove them
      pitEntry->ClearIncoming ();

      // Remove also outgoing
      pitEntry->ClearOutgoing ();

      // Set pruning timout on PIT entry (instead of deleting the record)
      m_pit->MarkErased (pitEntry);
    }
}



bool
NNNForwardingStrategy::DetectRetransmittedSO (Ptr<Face> inFace,
                                                 Ptr<const SO> so_p                                             Ptr<pit::Entry> pitEntry)
{
  pit::Entry::in_iterator existingInFace = pitEntry->GetIncoming ().find (inFace);

  bool isRetransmitted = false;

  if (existingInFace != pitEntry->GetIncoming ().end ())
    {
      // this is almost definitely a retransmission. But should we trust the user on that?
      isRetransmitted = true;
    }

  return isRetransmitted;
}
*/
/*
void
NNNForwardingStrategy::SatisfyPendingSO (Ptr<Face> inFace,
                                            Ptr<const DO> do_p,
                                            Ptr<pit::Entry> pitEntry)
{
  if (inFace != 0)
    pitEntry->RemoveIncoming (inFace);

  //satisfy all pending incoming SOs
  BOOST_FOREACH (const pit::IncomingFace &incoming, pitEntry->GetIncoming ())
    {
      bool ok = incoming.m_face->SendDO (do_p);

      DidSendOutDO (inFace, incoming.m_face, do_p, pitEntry);
      NS_LOG_DEBUG ("Satisfy " << *incoming.m_face);

      if (!ok)
        {
          m_dropDO (do_p, incoming.m_face);
          NS_LOG_DEBUG ("Cannot satisfy DO to " << *incoming.m_face);
        }
    }

  // All incoming SOs are satisfied. Remove them
  pitEntry->ClearIncoming ();

  // Remove all outgoing faces
  pitEntry->ClearOutgoing ();

  // Set pruning timout on PIT entry (instead of deleting the record)
  m_pit->MarkErased (pitEntry);
}
*/
/*
void
NNNForwardingStrategy::DidReceiveSolicitedDO (Ptr<Face> inFace,
                                             Ptr<const DO> do_p,
                                             bool didCreateCacheEntry)
{
  // do nothing
}
*/
/*
void
NNNForwardingStrategy::DidReceiveUnsolicitedDO (Ptr<Face> inFace,
                                               Ptr<const DO> do_p,
                                               bool didCreateCacheEntry)
{
  // do nothing
}
*/
/*
void
NNNForwardingStrategy::WillSatisfyPendingSO (Ptr<Face> inFace,
                                                Ptr<pit::Entry> pitEntry)
{
  pit::Entry::out_iterator out = pitEntry->GetOutgoing ().find (inFace);

  // If we have sent SO for this DO via this face, then update stats.
  if (out != pitEntry->GetOutgoing ().end ())
    {
      pitEntry->GetNNSTEntry ()->UpdateFaceRtt (inFace, Simulator::Now () - out->m_sendTime);
    }

  m_satisfiedSOs (pitEntry);
}
*/
/*
bool
NNNForwardingStrategy::ShouldSuppressIncomingSO (Ptr<Face> inFace,
                                                    Ptr<const SO> so_p,
                                                    Ptr<pit::Entry> pitEntry)
{
  bool isNew = pitEntry->GetIncoming ().size () == 0 && pitEntry->GetOutgoing ().size () == 0;

  if (isNew) return false; // never suppress new SOs

  bool isRetransmitted = m_detectRetransmissions && // a small guard
                         DetectRetransmittedSO (inFace, so_p, pitEntry);

  if (pitEntry->GetOutgoing ().find (inFace) != pitEntry->GetOutgoing ().end ())
    {
      NS_LOG_DEBUG ("Non duplicate SOs from the face we have sent SO to. Don't suppress");
      // got a non-duplicate SO from the face we have sent SO to
      // Probably, there is no point in waiting DO from that face... Not sure yet

      // If we're expecting DO from the interface we got the SO from ("producer" asks us for "his own" DO)
      // Mark interface YELLOW, but keep a small hope that DO will come eventually.

      // ?? not sure if we need to do that ?? ...

      // pitEntry->GetNNSTEntry ()->UpdateStatus (inFace, NNST::FaceMetric::NNN_NNST_YELLOW);
    }
  else
    if (!isNew && !isRetransmitted)
      {
        return true;
      }

  return false;
}
*/
/*
void
NNNForwardingStrategy::PropagateSO (Ptr<Face> inFace,
                                       Ptr<const SO> so_p,
                                       Ptr<pit::Entry> pitEntry)
{
  bool isRetransmitted = m_detectRetransmissions && // a small guard
                         DetectRetransmittedSO (inFace, so_p, pitEntry);

  pitEntry->AddIncoming (inFace/*, so_p->GetSOLifetime ()*/);
  /// @todo Make lifetime per incoming interface       */
/*  pitEntry->UpdateLifetime (so_p->GetSOLifetime ());

  bool propagated = DoPropagateSO (inFace, so_p, pitEntry);

  if (!propagated && isRetransmitted) //give another chance if retransmitted
    {
      // increase max number of allowed retransmissions
      pitEntry->IncreaseAllowedRetxCount ();

      // try again
      propagated = DoPropagateSO (inFace, so_p, pitEntry);
    }

  // if (!propagated)
  //   {
  //     NS_LOG_DEBUG ("++++++++++++++++++++++++++++++++++++++++++++++++++++++");
  //     NS_LOG_DEBUG ("+++ Not propagated ["<< so->GetName () <<"], but number of outgoing faces: " << pitEntry->GetOutgoing ().size ());
  //     NS_LOG_DEBUG ("++++++++++++++++++++++++++++++++++++++++++++++++++++++");
  //   }

  // NNNForwardingStrategy will try its best to forward packet to at least one interface.
  // If no so was propagated, then there is not other option for forwarding or
  // NNNForwardingStrategy failed to find it.
  if (!propagated && pitEntry->AreAllOutgoingInVain ())
    {
      DidExhaustForwardingOptions (inFace, so_p, pitEntry);
    }
}
*/
/*
bool
NNNForwardingStrategy::CanSendOutSO (Ptr<Face> inFace,
                                        Ptr<Face> outFace,
                                        Ptr<const SO> so_p,
                                        Ptr<pit::Entry> pitEntry)
{
  if (outFace == inFace)
    {
      // NS_LOG_DEBUG ("Same as incoming");
      return false; // same face as incoming, don't forward
    }

  pit::Entry::out_iterator outgoing =
    pitEntry->GetOutgoing ().find (outFace);

  if (outgoing != pitEntry->GetOutgoing ().end ())
    {
      if (!m_detectRetransmissions)
        return false; // suppress
      else if (outgoing->m_retxCount >= pitEntry->GetMaxRetxCount ())
        {
          // NS_LOG_DEBUG ("Already forwarded before during this retransmission cycle (" <<outgoing->m_retxCount << " >= " << pitEntry->GetMaxRetxCount () << ")");
          return false; // already forwarded before during this retransmission cycle
        }
   }

  return true;
}
*/
/*
bool
NNNForwardingStrategy::TrySendOutSO (Ptr<Face> inFace,
                                        Ptr<Face> outFace,
                                        Ptr<const SO> so_p,
                                        Ptr<pit::Entry> pitEntry)
{
  if (!CanSendOutSO (inFace, outFace, so_p, pitEntry))
    {
      return false;
    }

  pitEntry->AddOutgoing (outFace);

  //transmission
  bool successSend = outFace->SendSO (so_p);
  if (!successSend)
    {
      m_dropSOs (so_p, outFace);
    }

  DidSendOutSO (inFace, outFace, so_p, pitEntry);

  return true;
}
*/
/*
void
NNNForwardingStrategy::DidSendOutSO (Ptr<Face> inFace,
                                        Ptr<Face> outFace,
                                        Ptr<const SO> so_p,
                                        Ptr<pit::Entry> pitEntry)
{
  m_outSOs (so_p, outFace);
}
*/
/*
void
NNNForwardingStrategy::DidSendOutDO (Ptr<Face> inFace,
                                    Ptr<Face> outFace,
                                    Ptr<const DO> do_p,
                                    Ptr<pit::Entry> pitEntry)
{
  m_outDO (do_p, inFace == 0, outFace);
}
*/
/*
void
NNNForwardingStrategy::WillEraseTimedOutPendingSO (Ptr<pit::Entry> pitEntry)
{
  m_timedOutSOs (pitEntry);
}
*/
void
NNNForwardingStrategy::AddFace (Ptr<Face> face)
{
  // do nothing here
}

void
NNNForwardingStrategy::RemoveFace (Ptr<Face> face)
{
  // do nothing here
}

void
NNNForwardingStrategy::DidAddNNSTEntry (Ptr<NNST::Entry> NNSTEntry)
{
  // do nothing here
}

void
NNNForwardingStrategy::WillRemoveNNSTEntry (Ptr<NNST::Entry> NNSTEntry)
{
  // do nothing here
}


} // namespace nnn
} // namespace ns3
