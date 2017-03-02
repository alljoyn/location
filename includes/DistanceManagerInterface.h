/******************************************************************************
 *    Copyright (c) Open Connectivity Foundation (OCF), AllJoyn Open Source
 *    Project (AJOSP) Contributors and others.
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
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 *    WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 *    WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 *    AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 *    DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 *    PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 *    TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 *    PERFORMANCE OF THIS SOFTWARE.
******************************************************************************/
#ifndef DISTANCE_MANAGER_INTERFACE_H_
#define DISTANCE_MANAGER_INTERFACE_H_

namespace org_alljoyn_locationservices { namespace distance { namespace Manager {
const unsigned short int INTERFACE_VERSION = 1;

const char* const INTERFACE_NAME = 
    "org.alljoyn.locationservices.distance.Manager";

const char* const INTERFACE_PATH = 
    "/Locationservices/Distance/Manager";

const char* const INTERFACE_DESCRIPTION = 
    "Location Services Distance Manager";

const char* const INTERFACE_XML =
    "<interface name=\"org.alljoyn.locationservices.distance.Manager\">"
    "  <description language=\"en\">"
    "    This interface contains methods for applications to access distance services data" 
    "  </description>"
    "  <property name=\"Version\" type=\"q\" access=\"read\"/>"
    "  <method name=\"DistanceSubscribe\">"
    "    <description language=\"en\">"
    "      Request distance tracking based on the supplied filter"
    "    </description>"
    "    <arg name=\"distanceFilter\" type=\"((ss)(ss)a(ss))\" direction=\"in\"/>"
    "    <arg name=\"distanceDelta\" type=\"d\" direction=\"in\"/>"
    "    <arg name=\"distanceTracker\" type=\"o\" direction=\"out\"/>"
    "  </method>"
    "  <method name=\"QueryDistance\">"
    "    <description language=\"en\">"
    "      Query for distance information based on the supplied filter"
    "    </description>"
    "    <arg name=\"distanceFilter\" type=\"((ss)(ss)a(ss))\" direction=\"in\"/>"
    "    <arg name=\"distanceEntities\" type=\"a((ss)dd)\" direction=\"out\"/>"
    "  </method>"
    "</interface>";
}}}

#endif /* DISTANCE_MANAGER_INTERFACE_H_ */