/******************************************************************************
 *    Copyright (c) Open Connectivity Foundation (OCF) and AllJoyn Open
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
#ifndef POSITION_TRACKER_INTERFACE_H_
#define POSITION_TRACKER_INTERFACE_H_

namespace org_alljoyn_locationservices { namespace position { namespace Tracker {
const unsigned short int INTERFACE_VERSION = 1;

const char* const INTERFACE_NAME = 
    "org.alljoyn.locationservices.position.Tracker";

const char* const INTERFACE_PATH = 
    "/Locationservices/Position/Tracker";

const char* const INTERFACE_DESCRIPTION = 
    "Location Services Position Tracker";

const char* const INTERFACE_XML =
    "<interface name=\"org.alljoyn.locationservices.positionn.Tracker\">"
    "  <description language=\"en\">"
    "    This interface tracks the position between a reference entity and other entities that match the supplied filter" 
    "  </description>"
    "  <property name=\"Version\" type=\"q\" access=\"read\"/>"

    "  <property name=\"Filter\" type=\"((ss)a(ss))\" access=\"read\">"
    "    <description language=\"en\">"
    "      The position filter configured for this tracker" 
    "    </description>"
    "    <annotation name=\"org.freedesktop.DBus.Property.EmitsChangedSignal\" value=\"false\"/>"
    "  </property>"

    "  <method name=\"Matches\">"
    "    <description language=\"en\">"
    "      Return the list of entities that match the filter entity selection criteria"
    "    </description>"
    "    <arg name=\"entities\" type=\"a((ss)(sdddd))\" direction=\"out\"/>"
    "  </method>"

    "  <signal name=\"EntityChanged\" sessioncast=\"true\">"
    "    <description language=\"en\">"
    "      The entity has changed its position"
    "    </description>"
    "    <arg name=\"tracker\" type=\"o\" direction=\"out\"/>"
    "    <arg name=\"entity\" type=\"((ss)(sdddd))\" direction=\"out\"/>"
    "  </signal>"

    "  <signal name=\"EndOfLifeThresholdReached\" sessioncast=\"true\">"
    "    <description language=\"en\">"
    "      The threshold for reclaiming the filter has been crossed"
    "    </description>"
    "    <arg name=\"tracker\" type=\"o\" direction=\"out\"/>"
    "    <arg name=\"secondsRemaining\" type=\"u\" direction=\"out\"/>"
    "  </signal>"

    "  <method name=\"KeepAlive\">"
    "    <description language=\"en\">"
    "      Reset the the end of life timer"
    "    </description>"
    "    <annotation name=\"org.freedesktop.DBus.Method.NoReply\" value=\"true\"/>"
    "  </method>"
    "</interface>";
}}}

#endif /* POSITION_TRACKER_INTERFACE_H_ */