
Letní 2019/2020

Body: 70/100

# About

The application shows the map, connections and buses on them. It simulates bus movement according to simulation time.

Map data is loaded from ./examples/map.json.
Connections and bus departures are loaded from ./examples/connections.json.

On the left side of UI, user can adjust currently viewed information (such as the name of streets, colour scheme according to traffic levels, etc.), can modify time and make interactions with map - adjust traffic levels or create/delete street closures.

# map.json

Information about streets is stored here. Each street has its unique id, the time it takes to ride through, coordinates x and y where it starts, coordinates where it ends. Those coordinates must be within <0, 100>. Street can also have a name but doesn't need to.

# connections.json

Information about all connections and the buses operation on them is stored in this file. Each connection must be named, and all streets must contain their respective unique id which must match a street denoted in map.json. Other than the UID each street contain 2 booleans - 'right_direction' denotes whether the direction of travel is from the street start to the end or end to start, while 'stop' denotes whether the street has a bus stop or not.
For buses, each bus contains the departure time in seconds since midnight, denoted by 'time', and 'return' marks whether the bus is going in the opposite direction.

# Error Codes

If a fatal error occurs, the application is exited with the following return code.

1 - "connections.json" file is invalid or contains unexpected information

2 - "map.json" file is invalid or contains unexpected information

99 - internal error
