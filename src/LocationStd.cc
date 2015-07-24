/**
 * @file
 *
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

#include <qcc/platform.h>
#include <qcc/Debug.h>

#include <alljoyn/location/LocationStd.h>

#define QCC_MODULE "LOCATION"

namespace ajn {

/** org.alljoyn.locationservices Definitions */
const char* org::alljoyn::locationservices::InterfaceName = "org.alljoyn.locationservices";
const char* org::alljoyn::locationservices::InterfaceXml =
    "<interface name=\"org.alljoyn.locationservices\">"
    "  <property name=\"Version\" type=\"q\" access=\"read\"/>"
    "  <method name=\"EntityDiscovery\">"
    "    <arg name=\"entity\" type=\"(ss)\" direction=\"in\"/>"
    "  </method>"
    "  <method name=\"PresenceSubscribe\">"
    "    <arg name=\"filter\" type=\"(ss)\" direction=\"in\"/>"
    "    <arg name=\"tracker\" type=\"o\" direction=\"out\"/>"
    "  </method>"
    "  <method name=\"DistanceSubscribe\">"
    "    <arg name=\"entity\" type=\"(ss)\" direction=\"in\"/>"
    "    <arg name=\"filter\" type=\"(ssi)\" direction=\"in\"/>"
    "    <arg name=\"tracker\" type=\"o\" direction=\"out\"/>"
    "  </method>"
    "  <method name=\"GeofenceSubscribe\">"
    "    <arg name=\"entity\" type=\"(ss)\" direction=\"in\"/>"
    "    <arg name=\"filter\" type=\"(ssi)\" direction=\"in\"/>"
    "    <arg name=\"tracker\" type=\"o\" direction=\"out\"/>"
    "  </method>"
    "  <method name=\"Unsubscribe\">"
    "    <annotation name=\"org.freedesktop.DBus.Method.NoReply\" value=\"true\"/>"
    "    <arg name=\"tracker\" type=\"o\" direction=\"in\"/>"
    "  </method>"
    "</interface>";

/** org.alljoyn.locationservices.PresenceTracker Definitions */
const char* org::alljoyn::locationservices::PresenceTracker::InterfaceName = "org.alljoyn.locationservices.PresenceTracker";
const char* org::alljoyn::locationservices::PresenceTracker::InterfaceXml =
    "<interface name=\"org.alljoyn.locationservices.PresenceTracker\">"
    "  <property name=\"Version\" type=\"q\" access=\"read\"/>"
    "  <property name=\"Tracker\" type=\"s\" access=\"read\"/>"
    "  <property name=\"Matches\" type=\"a(say)\" access=\"read\">"
    "    <annotation name=\"org.freedesktop.DBus.Property.EmitsChangedSignal\" value=\"true\"/>"
    "  </property>"
    "</interface>";

/** org.alljoyn.locationservices.DistanceTracker Definitions */
const char* org::alljoyn::locationservices::DistanceTracker::InterfaceName = "org.alljoyn.locationservices.DistanceTracker";
const char* org::alljoyn::locationservices::DistanceTracker::InterfaceXml =
    "<interface name=\"org.alljoyn.locationservices.DistanceTracker\">"
    "  <property name=\"Version\" type=\"q\" access=\"read\"/>"
    "  <property name=\"Tracker\" type=\"s\" access=\"read\"/>"
    "  <property name=\"Matches\" type=\"a(ssi)\" access=\"read\">"
    "    <annotation name=\"org.freedesktop.DBus.Property.EmitsChangedSignal\" value=\"true\"/>"
    "  </property>"
    "    <signal name=\"Tracking\">"
    "      <arg name=\"entity\" type=\"(ssi)\" direction=\"out\"/>"
    "    </signal>"
    "</interface>";

/** org.alljoyn.locationservices.GeofenceTracker Definitions */
const char* org::alljoyn::locationservices::GeofenceTracker::InterfaceName = "org.alljoyn.locationservices.GeofenceTracker";
const char* org::alljoyn::locationservices::GeofenceTracker::InterfaceXml =
    "<interface name=\"org.alljoyn.locationservices.GeofenceTracker\">"
    "  <property name=\"Version\" type=\"q\" access=\"read\"/>"
    "  <property name=\"Tracker\" type=\"s\" access=\"read\"/>"
    "  <property name=\"Matches\" type=\"a(ssb)\" access=\"read\">"
    "    <annotation name=\"org.freedesktop.DBus.Property.EmitsChangedSignal\" value=\"true\"/>"
    "  </property>"
    "    <signal name=\"Tracking\">"
    "      <arg name=\"entity\" type=\"(ssb)\" direction=\"out\"/>"
    "    </signal>"
    "</interface>";

/** org.alljoyn.locationservices.LocationTracker Definitions */
const char* org::alljoyn::locationservices::LocationTracker::InterfaceName = "org.alljoyn.locationservices.LocationTracker";
const char* org::alljoyn::locationservices::LocationTracker::InterfaceXml =
    "<interface name=\"org.alljoyn.locationservices.LocationTracker\">"
    "  <property name=\"Version\" type=\"q\" access=\"read\"/>"
    "  <property name=\"Tracker\" type=\"s\" access=\"read\"/>"
    "  <property name=\"Matches\" type=\"a(ss)\" access=\"read\">"
    "    <annotation name=\"org.freedesktop.DBus.Property.EmitsChangedSignal\" value=\"true\"/>"
    "  </property>"
    "    <signal name=\"Tracking\">"
    "      <arg name=\"entity\" type=\"(ss)\" direction=\"out\"/>"
    "    </signal>"
    "</interface>";

}
