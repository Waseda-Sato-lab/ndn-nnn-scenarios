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
 */

#ifndef NNN_FORWARDING_STRATEGY_H
#define NNN_FORWARDING_STRATEGY_H

#include "ns3/packet.h"
#include "ns3/callback.h"
#include "ns3/object.h"
#include "ns3/traced-callback.h"

namespace ns3 {
namespace nnn {

/**
 * @ingroup nnn
 * @defgroup nnn-fw NNN forwarding strategies
 */


/**
 * @ingroup nnn-fw
 * @brief Namespace for Forwarding Strategy operations
 */
namespace fw {
}

class Face;

class SO;
class DO;

//class Pit;
//namespace pit { class Entry; }
class NNSTFaceMetric;
class NNST;
namespace NNST { class Entry; }
//class ContentStore;

/**
 * @ingroup nnn-fw
 * @brief Abstract base class for Nnn forwarding strategies
 */
class NNNForwardingStrategy :
    public Object
{
public:
  static TypeId GetTypeId ();

  /**
   * @brief Helper function to retrieve logging name for the forwarding strategy
   */
  static std::string GetLogName ();

  /**
   * @brief Default constructor
   */
  NNNForwardingStrategy ();
  virtual ~NNNForwardingStrategy ();

  /**
   * \brief Actual processing of incoming Nnn SOs. Note, SOs do not have payload
   *
   * Processing SO packets
   * @param face     incoming face
   * @param so_p SO packet
   */
  virtual void
  OnSO (Ptr<Face> face,
              Ptr<SO> so_p);

  /**
   * \brief Actual processing of incoming Nnn content objects
   *
   * Processing DO packets
   * @param face    incoming face
   * @param do_p    DO packet
   */
  virtual void
  OnDO (Ptr<Face> face,
          Ptr<DO> do_p);

  /**
   * @brief Event fired just before PIT entry is removed by timeout
   * @param pitEntry PIT entry to be removed

  virtual void
  WillEraseTimedOutPendingSO (Ptr<pit::Entry> pitEntry);
   */

  /**
   * @brief Event fired every time face is added to NNN stack
   * @param face face to be removed
   */
  virtual void
  AddFace (Ptr<Face> face);

  /**
   * @brief Event fired every time face is removed from NNN stack
   * @param face face to be removed
   *
   * For example, when an application terminates, AppFace is removed and this method called by NNN stack.
   */
  virtual void
  RemoveFace (Ptr<Face> face);

  /**
   * @brief Event fired every time a NNST entry is added to NNST
   * @param NNSTEntry NNST entry that was added
   */
  virtual void
  DidAddNNSTEntry (Ptr<NNST::Entry> NNSTEntry);

  /**
   * @brief Fired just before NNST entry will be removed from NNST
   * @param nnstEntry NNST entry that will be removed
   */
  virtual void
  WillRemoveNNSTEntry (Ptr<NNST::Entry> NNSTEntry);

protected:
  /**
   * @brief An event that is fired every time a new PIT entry is created
   *
   * Note that if NNN node is receiving a similar SO (SO for the same name),
   * then either DidReceiveDuplicateSO, DidSuppressSimilarSO, or DidForwardSimilarSO
   * will be called
   *
   * Suppression of similar SOs is controlled using ShouldSuppressIncomingSO virtual method
   *
   * @param inFace  incoming face
   * @param header  deserialized SO header
   * @param pitEntry created PIT entry (incoming and outgoing face sets are empty)
   *
   * @see DidReceiveDuplicateSO, DidSuppressSimilarSO, DidForwardSimilarSO, ShouldSuppressIncomingSO

  virtual void
  DidCreatePitEntry (Ptr<Face> inFace,
                     Ptr<const SO> so_p,
                     Ptr<pit::Entry> pitEntry);
   */

  /**
   * @brief An event that is fired every time a new PIT entry cannot be created (e.g., PIT container imposes a limit)
   *
   * Note that this call can be called only for non-similar SO (i.e., there is an attempt to create a new PIT entry).
   * For any non-similar SOs, either FailedToCreatePitEntry or DidCreatePitEntry is called.
   *
   * @param inFace   incoming face
   * @param so_p SO packet

  virtual void
  FailedToCreatePitEntry (Ptr<Face> inFace,
                          Ptr<const SO> so_p);
   */

  /**
   * @brief An event that is fired every time a duplicated SO is received
   *
   * This even is the last action that is performed before the SO processing is halted
   *
   * @param inFace  incoming face
   * @param so_p SO packet
   * @param pitEntry an existing PIT entry, corresponding to the duplicated SO
   *
   * @see DidReceiveDuplicateSO, DidSuppressSimilarSO, DidForwardSimilarSO, ShouldSuppressIncomingSO

  virtual void
  DidReceiveDuplicateSO (Ptr<Face> inFace,
                               Ptr<const SO> so_p,
                               Ptr<pit::Entry> pitEntry);
   */

  /**
   * @brief An event that is fired every time when a similar SO is received and suppressed (collapsed)
   *
   * This even is the last action that is performed before the SO processing is halted
   *
   * @param inFace  incoming face
   * @param so_p SO packet
   * @param pitEntry an existing PIT entry, corresponding to the duplicated SO
   *
   * @see DidReceiveDuplicateSO, DidForwardSimilarSO, ShouldSuppressIncomingSO

  virtual void
  DidSuppressSimilarSO (Ptr<Face> inFace,
                              Ptr<const SO> so_p,
                              Ptr<pit::Entry> pitEntry);
   */

  /**
   * @brief An event that is fired every time when a similar SO is received and further forwarded (not suppressed/collapsed)
   *
   * This even is fired just before handling the SO to PropagateSO method
   *
   * @param inFace  incoming face
   * @param so_p SO packet
   * @param pitEntry an existing PIT entry, corresponding to the duplicated SO
   *
   * @see DidReceiveDuplicateSO, DidSuppressSimilarSO, ShouldSuppressIncomingSO

  virtual void
  DidForwardSimilarSO (Ptr<Face> inFace,
                             Ptr<const SO> so_p,
                             Ptr<pit::Entry> pitEntry);
   */

  /**
   * @brief An even that is fired when SO cannot be forwarded
   *
   * Note that the event will not fire if  retransmission detection is enabled (by default)
   * and retransmitted SO cannot by forwarded.  For more details, refer to the implementation.
   *
   * @param inFace  incoming face
   * @param so_p SO header
   * @param pitEntry an existing PIT entry, corresponding to the duplicated SO
   *
   * @see DetectRetransmittedSO

  virtual void
  DidExhaustForwardingOptions (Ptr<Face> inFace,
                               Ptr<const SO> so_p,
                               Ptr<pit::Entry> pitEntry);
   */

  /**
   * @brief Method that implements logic to distinguish between new and retransmitted SO
   *
   * This method is called only when DetectRetransmissions attribute is set true (by default).
   *
   * Currently, the retransmission detection logic relies on the fact that list of incoming faces
   * already has inFace (i.e., a similar SO is received on the same face more than once).
   *
   * @param inFace  incoming face
   * @param so_p SO header
   * @param pitEntry an existing PIT entry, corresponding to the duplicated SO
   * @return true if SO should be considered as retransmitted

  virtual bool
  DetectRetransmittedSO (Ptr<Face> inFace,
                               Ptr<const SO> so_p,
                               Ptr<pit::Entry> pitEntry);
   */

  /**
   * @brief Even fired just before SO will be satisfied
   *
   * Note that when SO is satisfied from the cache, incoming face will be 0
   *
   * @param inFace  incoming face
   * @param pitEntry an existing PIT entry, corresponding to the duplicated SO

  virtual void
  WillSatisfyPendingSO (Ptr<Face> inFace,
                              Ptr<pit::Entry> pitEntry);
   */

  /**
   * @brief Actual procedure to satisfy SO
   *
   * Note that when SO is satisfied from the cache, incoming face will be 0
   *
   * @param inFace  incoming face
   * @param do_p    DO packet
   * @param pitEntry an existing PIT entry, corresponding to the duplicated SO

  virtual void
  SatisfyPendingSO (Ptr<Face> inFace, // 0 allowed (from cache)
                          Ptr<const DO> do_p,
                          Ptr<pit::Entry> pitEntry);
   */

  /**
   * @brief Event which is fired just after DO was send out on the face
   *
   * @param inFace   incoming face of the DO
   * @param outFace  outgoing face
   * @param do_p     DO packet
   * @param pitEntry an existing PIT entry, corresponding to the duplicated SO

  virtual void
  DidSendOutDO (Ptr<Face> inFace,
                  Ptr<Face> outFace,
                  Ptr<const DO> do_p,
                  Ptr<pit::Entry> pitEntry);
   */

  /**
   * @brief Event which is fired every time a requested (solicited) DO packet (there is an active PIT entry) is received
   *
   * @param inFace  incoming face
   * @param do_p    DO packet
   * @param didCreateCacheEntry flag indicating whether a cache entry was added for this DO packet or not (e.g., packet already exists in cache)

  virtual void
  DidReceiveSolicitedDO (Ptr<Face> inFace,
                           Ptr<const DO> do_p,
                           bool didCreateCacheEntry);
   */

  /**
   * @brief Event which is fired every time an unsolicited DO packet (no active PIT entry) is received
   *
   * The current implementation allows ignoring unsolicited DO (by default), or cache it by setting
   * attribute CacheUnsolicitedDO true
   *
   * @param inFace  incoming face
   * @param do_p    DO packet
   * @param didCreateCacheEntry flag indicating whether a cache entry was added for this DO packet or not (e.g., packet already exists in cache)

  virtual void
  DidReceiveUnsolicitedDO (Ptr<Face> inFace,
                             Ptr<const DO> do_p,
                             bool didCreateCacheEntry);
   */

  /**
   * @brief Method implementing logic to suppress (collapse) similar SOs
   *
   * In the base class implementation this method checks list of incoming/outgoing faces of the PIT entry
   * (for new Intersets, both lists are empty before this call)
   *
   * For more details, refer to the source code.
   *
   * @param inFace  incoming face
   * @param so_p SO packet
   * @param payload DO payload

  virtual bool
  ShouldSuppressIncomingSO (Ptr<Face> inFace,
                                  Ptr<const SO> so_p,
                                  Ptr<pit::Entry> pitEntry);
   */

  /**
   * @brief Method to check whether SO can be send out on the particular face or not
   *
   * In the base class, this method perfoms two checks:
   * 1. If inFace is equal to outFace (when equal, SO forwarding is prohibited)
   * 2. Whether SO should be suppressed (list of outgoing faces include outFace),
   * considering (if enabled) retransmission logic
   *
   * @param inFace     incoming face of the SO
   * @param outFace    proposed outgoing face of the SO
   * @param so_p   SO packet
   * @param pitEntry   reference to PIT entry (reference to corresponding NNST entry inside)
   *
   * @see DetectRetransmittedSO

  virtual bool
  CanSendOutSO (Ptr<Face> inFace,
                      Ptr<Face> outFace,
                      Ptr<const SO> so_p,
                      Ptr<pit::Entry> pitEntry);
   */

  /**
   * @brief Method implementing actual SO forwarding, taking into account CanSendOutSO decision
   *
   * If event returns false, then there is some kind of a problem exists
   *
   * @param inFace     incoming face of the SO
   * @param outFace    proposed outgoing face of the SO
   * @param so_p SO packet
   * @param pitEntry   reference to PIT entry (reference to corresponding NNST entry inside)
   *
   * @see CanSendOutSO

  virtual bool
  TrySendOutSO (Ptr<Face> inFace,
                      Ptr<Face> outFace,
                      Ptr<const SO> so_p,
                      Ptr<pit::Entry> pitEntry);
   */

  /**
   * @brief Event fired just after forwarding the SO
   *
   * @param inFace     incoming face of the SO
   * @param outFace    outgoing face of the SO
   * @param so_p SO packet
   * @param pitEntry   reference to PIT entry (reference to corresponding NNST entry inside)

  virtual void
  DidSendOutSO (Ptr<Face> inFace,
                      Ptr<Face> outFace,
                      Ptr<const SO> so_p,
                      Ptr<pit::Entry> pitEntry);
   */

  /**
   * @brief Wrapper method, which performs general tasks and calls DoPropagateSO method
   *
   * General tasks so far are adding face to the list of incoming face, updating
   * PIT entry lifetime, calling DoPropagateSO, and retransmissions (enabled by default).
   *
   * @param inFace     incoming face
   * @param so_p   SO packet
   * @param pitEntry   reference to PIT entry (reference to corresponding NNST entry inside)
   *
   * @see DoPropagateSO

  virtual void
  PropagateSO (Ptr<Face> inFace,
                     Ptr<const SO> so_p,
                     Ptr<pit::Entry> pitEntry);
   */

  /**
   * @brief Virtual method to perform SO propagation according to the forwarding strategy logic
   *
   * In most cases, this is the call that needs to be implemented/re-implemented in order
   * to perform forwarding of SOs according to the desired logic.
   *
   * There is also PropagateSO method (generally, do not require to be overriden)
   * which performs general tasks (adding face to the list of incoming face, updating
   * PIT entry lifetime, calling DoPropagateSO, as well as perform retransmissions (enabled by default).
   *
   * @param inFace     incoming face
   * @param so_p   SO packet
   * @param pitEntry   reference to PIT entry (reference to corresponding NNST entry inside)
   *
   * @return true if SO was successfully propagated, false if all options have failed
   *
   * @see PropagateSO

  virtual bool
  DoPropagateSO (Ptr<Face> inFace,
                       Ptr<const SO> so_p,
                       Ptr<pit::Entry> pitEntry) = 0;
   */

protected:
  // inherited from Object class
  virtual void NotifyNewAggregate (); ///< @brief Even when object is aggregated to another Object
  virtual void DoDispose (); ///< @brief Do cleanup

protected:
 // Ptr<Pit> m_pit; ///< \brief Reference to PIT to which this forwarding strategy is associated
  Ptr<NNST> m_nnst; ///< \brief NNST
 // Ptr<ContentStore> m_contentStore; ///< \brief Content store (for caching purposes only)

//  bool m_cacheUnsolicitedDOFromApps;
//  bool m_cacheUnsolicitedDO;
//  bool m_detectRetransmissions;

//  TracedCallback<Ptr<const SO>,
//                Ptr<const Face> > m_outSOs; ///< @brief Transmitted SOs trace

//  TracedCallback<Ptr<const SO>,
//                 Ptr<const Face> > m_inSOs; ///< @brief trace of incoming SOs

//  TracedCallback<Ptr<const SO>,
//                 Ptr<const Face> > m_dropSOs; ///< @brief trace of dropped SOs

  ////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////

//  TracedCallback<Ptr<const DO>,
//            bool /*from cache*/,
//                 Ptr<const Face> > m_outDO; ///< @brief trace of outgoing DO

//  TracedCallback<Ptr<const DO>,
//                 Ptr<const Face> > m_inDO; ///< @brief trace of incoming DO

//  TracedCallback<Ptr<const DO>,
//                  Ptr<const Face> > m_dropDO;  ///< @brief trace of dropped Do

  ////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////

//  TracedCallback< Ptr<const pit::Entry> > m_satisfiedSOs;
//  TracedCallback< Ptr<const pit::Entry> > m_timedOutSOs;
};

} // namespace nnn
} // namespace ns3

#endif /* NNN_FORWARDING_STRATEGY_H */
