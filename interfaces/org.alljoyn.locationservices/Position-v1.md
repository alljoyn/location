Position.MD

# org.alljoyn.locationservices.Distance.Distance version 1
## Specification
The position service provides two  types of position data. The absolute position of an entity is described by WGS 84 data.  
The position service can alternatively specify the position of an entity with respect to an origin is described by origin, X, Y and Z coordinates specified in centimeters. 
The origin is a string which has a locally determined meaning. Users of this service can query for the position of an entity or a group of entities. 
Users of this service can also subscribe for position information of an entity or a group of entities and receive signals about change in their position.
Positions sensors (contributing devices) may report either absolute or relative position information.
The horizontal and vertical accuracy of the position data is also returned. This is accuracy information is supplied by the contributing device.

|                       |                                                                       |
|-----------------------|-----------------------------------------------------------------------|
| Version               | 1                                                                     |
| Annotation            | org.alljoyn.Bus.Secure = true                                         |


### Methods

#### PositionSubscribe(filter) -> (distanceTracker)

|                       |                                             |
|-----------------------|---------------------------------------------|


Request position tracking based on the supplied filter.  
This method returns a tracker. 
If a tracker exists that matches the filter it is returned,
otherwise a new tracker object is created.
Trackers have a lifetime and will be reaped after the lifetime is exceeded.

Input arguments:

  * **positionFilter** --- PositionFilter --- A filter to match entities
  

Output arguments:

  * **positionTracker** --- PositionTracker --- a PositionTracker object that is on the AllJoyn Bus.

Errors raised by this method:

#### QueryPosition(filter) -> (distanceTracker)
|                       |                                             |
|-----------------------|---------------------------------------------|

Query for position information based on the supplied filter

Input Arguments:

* **positionFilter** --- PositionFilter --- The filter to be used to select the entities to return 
* **powerPreference** --- uint32 --- Power is an enum of HIGH, LOW or UNSPECIFIED
* **accuracyPreference** --- uint32 --- Accuracy is an enum of HIGH, LOW or UNPSECIFIED

Output Arguments:

* **positionEntities** --- a[PositionEntity] --- An array of position entities that match the positionFilter


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

  * The xml definition of the [Distance service] (Position-v1.xml)
  * The [Theory of Operations] (theory-of-operations)
  * The [PositionTracker] (PositionTracker-v1)
  * the [PositionContributor] (PositionContributor)
