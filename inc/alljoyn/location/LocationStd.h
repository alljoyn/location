#ifndef _ALLJOYN_LOCATIONSTD_H
#define _ALLJOYN_LOCATIONSTD_H

/**
 * @file
 * This file provides definitions for standard Location interfaces
 */

/******************************************************************************
 * Copyright AllSeen Alliance. All rights reserved.
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

#ifdef __cplusplus

#include <qcc/platform.h>

#include <alljoyn/Status.h>

namespace ajn {

namespace org {
namespace alljoyn {

/** Interface Definitions for org.alljoyn.locationservices */
namespace locationservices {
extern const char* InterfaceName; /**< Name of the interface */
extern const char* InterfaceXml; /**< XML description of the interface */

/** Definitions for org.alljoyn.locationservices.PresenceTracker */
namespace PresenceTracker {
extern const char* InterfaceName; /**< Name of the interface */
extern const char* InterfaceXml; /**< XML description of the interface */
}

/** Definitions for org.alljoyn.locationservices.DistanceTracker */
namespace DistanceTracker {
extern const char* InterfaceName; /**< Name of the interface */
extern const char* InterfaceXml; /**< XML description of the interface */
}

/** Definitions for org.alljoyn.locationservices.GeofenceTracker */
namespace GeofenceTracker {
extern const char* InterfaceName; /**< Name of the interface */
extern const char* InterfaceXml; /**< XML description of the interface */
}

/** Definitions for org.alljoyn.locationservices.LocationTracker */
namespace LocationTracker {
extern const char* InterfaceName; /**< Name of the interface */
extern const char* InterfaceXml; /**< XML description of the interface */
}

}

}
}

}

#endif

#endif
