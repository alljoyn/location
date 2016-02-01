# org.alljoyn.locationservices.Distance.DistanceTracker version 1
## Specification

This interface tracks the distance between a reference entity and other entities that match the supplied filter
Updates are triggered only when the distance change of an existing
matching entity is detected and is for more than the proximity distance 
(measured in cm) in the subscribed filter.
DistanceTrackers have a lifetime threshold and a lifetime. 
When an instance of a DistanceTrackers lifetime crosses the lifetime threshold, the service signals
the application via the DistanceTrackers using the EndofLifeThreasholdReached. 
If the application calls the KeepAlive method, the lifetime threshold and the lifetime is reset.
If a KeepAlive is not recieved before the lifetime expires, the DistanceTrackers is reaped.

|                       |                                                                       |
|-----------------------|-----------------------------------------------------------------------|
| Version               | 1                                                                     |
| Annotation            | org.alljoyn.Bus.Secure = true                                         |

### Properties


#### Filter
|                       |                                                                       |
|-----------------------|-----------------------------------------------------------------------|
| Type                  | PresenceFilter                                                        |
| Access                | read-only                                                             |
| Annotation            | org.freedesktop.DBus.Property.EmitsChangedSignal = true               |

This is the filter that created the DistanceTracker.

### Methods

#### Matches() -> distanceEntity[]
|                       |                                                                       |
|-----------------------|-----------------------------------------------------------------------|
This returns the list of entities that match the filter that created the DistanceTrackers.


Output Arguments:

* **entities** --- DistanceEntity[] --- An array of presence entities that match the distanceFilter


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


This signal is emmited when the distance from the reference entity has changed by at least the configured delta amount

Output arguments:

  * **tracker** --- DistanceTracker --- Distance Tracker
  * **entity** --- DistanceEntity --- The entity whose state has changed

#### EndOfLifeThresholdReached

|                       |                                   |
|-----------------------|-----------------------------------|
| Signal Type           | sessioncast                       |

Output arguments:

  * **tracker** --- DistanceTracker --- Distance Tracker
  * **secondsRemaining** --- uint32 --- The number of seconds remaining before the tracker is destroyed

This signal is emitted when the EndOfLife threshold is crossed.
Applications must call the KeepAlive method to reset the EndOfLife timer

### Named Types

#### struct Entity

Entities are objects that are tracked by the location services.
An entity may or may not be a member of the AllJoyn bus. 
An entity may be known or anonymous. 
A known entity is registered with the service by an application. 
Entities that are discovered, and not pre-registered are designated as unknown entities.
An entity is comprised of two strings, a unique identifier and a media specific identifier. 
The unique identifier is a human readable string.
The media specific identifier will normally be the MAC address. 
Location Services uses both the unique identifier and the media specific identifier when matching 
entities. Anonymous entities will have a unique identifier of 00000000-0000-0000-0000-000000000000
and a media specific identifier of their MAC address.

  * **entityDescriptor** --- string --- A human readable description of the entity
  * **entityMac** --- array of bytes --- the device specific ID (usually a MAC address)
 
#### struct DistanceEntity

Distance services representation of device information
It contains an entity (a human readable description and a unique id of the device), a distance (an integer
representing the distance of the device from a reference entity and a multiplier), and a representation of accuracy (an integer
representing the accuracy of the measurement and a multiplier)

  * **entity** -- Entity -- The entity for the device
  * **distance** --- double --- integer representing the distance of the device from a reference entity  
  * **distanceMultiplier** --- uint32 --- distance multiplier  
  * **accuracy** --- uint32 --- integer representing the accuracy of the measurment  
  * **accuracyMultiplier** --- uint32 --- accuracy multiplier

#### struct DistanceFilter

A DistanceFilter is used to query or subscribe to information held in the distance service.
The filter contains service settings along with selection criteria for specifying entities of interest.

  * **referenceEntity** -- Entity -- The reference entity is the entity from which distances to other entites are measured
  * **entityParser** --- Entity --- The entity parser is an entity filled with regular expressions for matching
  * **entityList** --- a[Entity] --- An array of entities that match the filter criteria

## References
  * theroy-of-operations
