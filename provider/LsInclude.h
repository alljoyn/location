/******************************************************************************
 * Copyright (c) 2015, AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/
#ifndef LS_INCLUDE_H_
#define LS_INCLUDE_H_

#define NEW_VERSION 15.09

#include <algorithm>
#include <cstdio>
#include <ctype.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <unordered_map>
#include <vector>
#include <alljoyn/AboutObj.h>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/BusObject.h>

#ifdef NEW_VERSION
#include <alljoyn/Init.h>
#endif

#include <alljoyn/MsgArg.h>
#include <alljoyn/Status.h>
#include <alljoyn/version.h>
#include <ls/LsStd.h>
#include <qcc/Debug.h>
#include <qcc/Log.h>
#include <qcc/Mutex.h>
#include <qcc/String.h>
#include <qcc/StringUtil.h>
#include "LsDatabase.h"
#include "LsManager.h"
#include "LsTracker.h"
#include "PresenceTracker.h"
#include "DistanceTracker.h"
#include "Locationservices.h"



#define QCC_MODULE "lsProvider"
#define ArraySize(a)  (sizeof(a) / sizeof(a[0]))


#endif /* LS_INCLUDE_H_ */
