# Coding Style and Conventions #

The exporter (gpExport) itself isn't enormous (about 3400 lines), but the code is still much easier to read, if one knows the conventions, and the used coding-style.

## Naming-conventions ##
Classes, types and variables follow the Java-conventions. Classes and types start with a capital character, and variables start with lower case. Both use CamelCase for compound words.

Examples:
```
 Vertex v;
 MeshExtractor jointNames;
```


Methods are slightly different (following a convention that has been used at Wootsoft): public methods start with a capital character, and private ones with a lower-case. Protected methods are slightly a mix-up (sorry).

STL is extensively used for containers, but not for algorithms. We intentionally avoided the use of the `<algorithm>` framework to keep the code as simple as possible.

Structs are classes without intelligence. They should have no (or very little) methods.

The programming style is as functional as possible. In general instances will not hold intermediate results of a sequence of method calls, but the control flow is made explicit by returning and passing the data.

This approach is not efficient but makes the code much more readable.