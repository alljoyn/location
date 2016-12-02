#ifndef _ALLJOYN_LOCATIONSTD_H
#define _ALLJOYN_LOCATIONSTD_H

/**
 * @file
 * This file provides definitions for standard Location interfaces
 */

/******************************************************************************
 *  * 
 *    Copyright (c) 2016 Open Connectivity Foundation and AllJoyn Open
 *    Source Project Contributors and others.
 *    
 *    All rights reserved. This program and the accompanying materials are
 *    made available under the terms of the Apache License, Version 2.0
 *    which accompanies this distribution, and is available at
 *    http://www.apache.org/licenses/LICENSE-2.0

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