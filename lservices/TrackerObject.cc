/******************************************************************************
 * Copyright (c) Open Connectivity Foundation (OCF) and AllJoyn Open
 *    Source Project (AJOSP) Contributors and others.
 *
 *    SPDX-License-Identifier: Apache-2.0
 *
 *    All rights reserved. This program and the accompanying materials are
 *    made available under the terms of the Apache License, Version 2.0
 *    which accompanies this distribution, and is available at
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Copyright (c) Open Connectivity Foundation and Contributors to AllSeen
 *    Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for
 *    any purpose with or without fee is hereby granted, provided that the
 *    above copyright notice and this permission notice appear in all
 *    copies.
 *
 *     THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 *     WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 *     WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 *     AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 *     DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 *     PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 *     TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 *     PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/
#include "Constants.h"
#include "ServiceDatabase.h"
#include "ServiceObject.h"
#include "TrackerObject.h"


using namespace ajn;
using namespace std;
using namespace qcc;


TrackerObject::TrackerObject(qcc::String trackerPath, ajn::BusAttachment* msgBus, ServiceDatabase* svcDb) 
        : DbListener(), ServiceObject((trackerPath + "/" + U32ToString(GetCursor())).c_str(), msgBus, svcDb),
          ttlMutex(), ttlPeriodMs(300000), ttlSignalMs(240000), ttlTimestamp(ajn::BusAttachment::GetTimestamp()),
          matchesMutex(), matchesCache(NULL)
{
    QCC_DbgPrintf(("TrackerObject"));
    return;
}

TrackerObject::~TrackerObject()
{
    QCC_DbgPrintf(("~TrackerObject"));
    
    /* Delete the matches cache. */
    matchesMutex.Lock(MUTEX_CONTEXT);
    delete matchesCache;
    matchesMutex.Unlock();
}


bool TrackerObject::LifetimeRemaining() 
{
    //QCC_DbgPrintf(("LifetimeRemaining - %ld", ttlTimestamp));

    bool result;

    ttlMutex.Lock(MUTEX_CONTEXT);
    try {
        uint32_t lifetime = ajn::BusAttachment::GetTimestamp() - ttlTimestamp;

        if (lifetime >= ttlPeriodMs) {
            result = false;

        } else if (lifetime >= ttlSignalMs) {
            result = true;
            ttlSignalMs = ttlPeriodMs;

            // This works for Android but not Higgns.
            MsgArg msgArgs[] = { {MsgArg("o", ServiceObject::GetPath())},
                                 {MsgArg("u", ((ttlPeriodMs - lifetime) / 1000))} };

            ServiceObject::Signal(NULL, 
                                  SESSION_ID_ALL_HOSTED, 
                                  *(svcIntf->GetMember("EndOfLifeThresholdReached")), 
                                  msgArgs, 
                                  ArraySize(msgArgs));
            QCC_DbgPrintf(("EndOfLifeThresholdReached Signal - %s", ServiceObject::GetPath()));

        } else {
            result = true;
        }

    } catch(std::exception& e) {
        result = true;
        QCC_DbgPrintf(("LifetimeRemaining Exception: %s\n", e.what()));
    }
    ttlMutex.Unlock();

    return result;
}

void TrackerObject::LifetimeExtension() 
{
    QCC_DbgPrintf(("LifetimeExtension - %s", ServiceObject::GetPath()));

    ttlMutex.Lock(MUTEX_CONTEXT);
    ttlPeriodMs = 300000;
    ttlSignalMs = 240000;
    ttlTimestamp = ajn::BusAttachment::GetTimestamp();
    ttlMutex.Unlock();
}
