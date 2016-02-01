# org.alljoyn.locationservices.Position.PositionTracker version 1
## Specification

This interface tracks the position of entities that match the supplied filter
PositionTracker have a lifetime threshold and a lifetime. 
When an instance of a PositionTracker lifetime crosses the lifetime threshold, the service signals
the application via the PositionTracker using the EndofLifeThreasholdReached. 
If the application calls the KeepAlive method, the lifetime threshold and the lifetime is reset.
If a KeepAlive is not recieved before the lifetime expires, the PositionTracker is reaped.

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

#### Matches() -> positionEntity[]
|                       |                                                                       |
|-----------------------|-----------------------------------------------------------------------|
This returns the list of entities that match the filter that created the DistanceTrackers.


Output Arguments:

* **entities** --- PositionEntity[] --- An array of position entities that match the positionFilter


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


This signal is emmited when the distance from the reference entity has changed its position

Output arguments:

  * **tracker** --- PositionTracker --- Position Tracker
  * **entity** --- PositionEntity --- The entity whose state has changed

#### EndOfLifeThresholdReached

|                       |                                   |
|-----------------------|-----------------------------------|
| Signal Type           | sessioncast                       |

Output arguments:

  * **tracker** --- PositionTracker --- Position Tracker
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


#### struct PositionTime

The representation of time for the Position Service

  * **year** --- uint16 --- Representation for year
  * **month** --- byte --- Representation for month
  * **day** --- byte --- Representation for day
  * **hour** --- byte --- Representation for hour
  * **second** --- byte --- Representation for second
  * **millisecond** --- uint16 --- Representation for millisecond

 
#### struct PositionData

The representation of position data. 
The field origin defines the reference point (e.g. wgs84)

  * **origin** -- string -- Text representation fo the origin reference point
  * **x** --- uint32 --- Representation of position on x-axis
  * **y** --- uint32 --- Representation of position on y-axis
  * **z** --- uint32 --- Representation of position on z-axis
  * **horizontalAccuracy** --- uint32 --- Representation of the horizontal accuracy
  * **horizontalAccuracyMultiplier** --- uint32 --- Representation of horizontal accuracy units
  * **verticalAccuracy** --- uint32 --- Representation of the vertical accuracy
  * **verticalAccuracyMultiplier** --- uint32 --- Representation of vertical accuracy units
  * **time** --- PositionTime --- Representation of the time the position was recorded

#### struct PositionEntity

Position services representation of device information

  * **entity** -- Entity -- A representation of the device
  * **positionData** --- PositionData --- The representation of position data.


#### struct PositionFilter

A filter to match entities the application is interested in

  * **powerPreference** -- uint32 -- the power preference is an enum of HIGH, LOW or UNSPECIFIED
  * **accuracyPreference** --- uint32 --- the accuracy preference is an enum of HIGH, LOW or UNPSECIFIED 
  * **entityParser** -- Entity -- the entity parser is an entity filled with regular expressions for matching
  * **entityList** --- a[Entity] --- The entity list is a list of entities for matching 
  
  
## References
  * theroy-of-operations
