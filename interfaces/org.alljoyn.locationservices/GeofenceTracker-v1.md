# org.alljoyn.locationservices.Geofence.GeofenceTracker version 1
## Specification

This interface tracks fenceline crossings around a reference entity by other entities that match the supplied filter
GeofenceTracker have a lifetime threshold and a lifetime. 
When an instance of a GeofenceTracker lifetime crosses the lifetime threshold, the service signals
the application via the GeofenceTracker using the EndofLifeThreasholdReached. 
If the application calls the KeepAlive method, the lifetime threshold and the lifetime is reset.
If a KeepAlive is not recieved before the lifetime expires, the GeofenceTracker is reaped.

|                       |                                                                       |
|-----------------------|-----------------------------------------------------------------------|
| Version               | 1                                                                     |
| Annotation            | org.alljoyn.Bus.Secure = true                                         |

### Properties


#### Filter
|                       |                                                                       |
|-----------------------|-----------------------------------------------------------------------|
| Type                  | PositionFilter                                                        |
| Access                | read-only                                                             |
| Annotation            | org.freedesktop.DBus.Property.EmitsChangedSignal = true               |

The position filter configured for this tracker

### Methods

#### Matches() -> geofenceEntity[]
|                       |                                                                       |
|-----------------------|-----------------------------------------------------------------------|
Return the list of entities that match the filter entity selection criteria.


Output Arguments:

* **entities** --- GeofenceEntity[] --- An array of geofence entities that match the geofenceFilter


#### KeepAlive()
|                       |                                             |
|-----------------------|---------------------------------------------|
| Annotation            | org.freedesktop.DBus.Method.NoReply = true  |

Reset the keep alive timer on the filter
 

### Signals

#### EntityChanged
|                       |                                   |
|-----------------------|-----------------------------------|
| Signal Type           | sessioncast                       |


This signal is emmited when the entity has crossed the geofence boarder for the reference entity

Output arguments:

  * **tracker** --- GeofenceTracker --- Geofence Tracker
  * **entity** --- GeofenceEntity --- The entity whose state has changed

#### EndOfLifeThresholdReached

|                       |                                   |
|-----------------------|-----------------------------------|
| Signal Type           | sessioncast                       |

Output arguments:

  * **tracker** --- GeofenceTracker --- Geofence Tracker
  * **secondsRemaining** --- uint32 --- The number of seconds remaining before the tracker is destroyed

This signal is emitted when the EndOfLife threshold is crossed.
Applications must call the KeepAlive method to reset the EndOfLife timer

### Named Types

#### struct Entity

Entity is a generic representation of a device
Entities are objects that are tracked by the location services.
An entity may or may not be a member of the AllJoyn bus. 
An entity may be known or anonymous. 
A known entity is registered with the service by an application. 
Entities that are discovered, and not pre-registered are designated as unknown entities.
An entity is comprised of two strings, a unique identifier and a media specific identifier. 
The unique identifier is a human readable string.
The media specific identifier is a unique descriptive string or it can be the MAC address if available. 
Location Services uses both the unique identifier and the media specific identifier when matching 
entities. Anonymous entities will have a unique identifier of 00000000-0000-0000-0000-000000000000
and a media specific identifier.

  * **entityDescriptor** --- string --- A human readable description of the entity
  * **entityMac** --- array of bytes --- the device specific ID (usually a MAC address)


#### struct GeofenceEntity

A GeofenceEntity is Geofence services representation of device information 
It contains an entity (a human readable description and a unique id of the device) and an integer
representing ... 

  * **entity** -- Entity -- The entity for the device
  * **withinFenceline** --- boolean --- boolean to indicate if the device is within the fenceline

#### struct GeofenceFilter

A GeofenceFilter is used to match Service settings along with selection criteria for specifying entities of interest

  * **referenceFenceline** -- double -- The reference fenceline is the distance around the reference entity that forms the geofence boundary
  * **referenceEntity** -- Entity -- The reference entity is the entity from which distances to other entites are measured
  * **entityParser** --- Entity --- The entity parser is an entity filled with regular expressions for matching
  * **entityList** --- a[Entity] --- The entity list is a list of entities for matching

  
## References
  * theroy-of-operations
