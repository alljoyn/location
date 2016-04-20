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
#ifndef POSITION_MANAGER_INTERFACE_H_
#define POSITION_MANAGER_INTERFACE_H_

namespace org_alljoyn_locationservices { namespace position { namespace Manager {
const unsigned short int INTERFACE_VERSION = 1;

const char* const INTERFACE_NAME = 
    "org.alljoyn.locationservices.position.Manager";

const char* const INTERFACE_PATH = 
    "/Locationservices/Position/Manager";

const char* const INTERFACE_DESCRIPTION = 
    "Location Services Position Manager";

const char* const INTERFACE_XML =
    "<interface name=\"org.alljoyn.locationservices.position.Manager\">"
    "  <description language=\"en\">"
    "    This interface contains methods for applications to access position services data" 
    "  </description>"
    "  <property name=\"Version\" type=\"q\" access=\"read\"/>"
    "  <method name=\"PositionSubscribe\">"
    "    <description language=\"en\">"
    "      Request position tracking based on the supplied filter"
    "    </description>"
    "    <arg name=\"positionFilter\" type=\"((ss)a(ss))\" direction=\"in\"/>"
    "    <arg name=\"positionTracker\" type=\"o\" direction=\"out\"/>"
    "  </method>"
    "  <method name=\"QueryPosition\">"
    "    <description language=\"en\">"
    "      Query for position information based on the supplied filter"
    "    </description>"
    "    <arg name=\"positionFilter\" type=\"((ss)a(ss))\" direction=\"in\"/>"
    "    <arg name=\"positionEntities\" type=\"a((ss)(sdddd))\" direction=\"out\"/>"
    "  </method>"
    "</interface>";
}}}

#endif /* POSITION_MANAGER_INTERFACE_H_ */