# Interfacing with Maya #

The interface between Maya and plugins is surprisingly well documented in Maya's help and on the web. We therefore only summarize the techniques and don't go into much detail.

In order to write an exporter one needs to interface in some way with Maya. The more obvious one is of course by writing a plugin, and implementing Maya's interfaces. Another interesting approach consists in writing a stand-alone application that uses Maya's libraries. We recommend keeping the exporter general enough, so it can be used as both.

## Plugin ##
If the exporter should be accessible from within Maya, the exporter needs to derive from `MPxFileTranslator`. Furthermore some methods must be implemented:
  * `haveReadMethod`: most of the time returns false.
  * `haveWriteMethod`: most of the time returns true.
  * `reader`: most of the time returns an error.
  * `writer`: exports the scene. This is the method, that does all the work.
  * `identifyFile`: recognizes the plugin's files (or not).
  * `defaultExtension`: obvious.

Note that `gpExport` already does all this work for you. By reusing the framework only two methods need to be implemented:
  * `Export`: more or less equivalent to writer.
  * `getExtension`: more or less equivalent to defaultExtension.

## Executable ##
Writing an executable is even easier (as long as one knows what libraries are needed, and how to link them to your executable). One just needs to initialize the `MLibrary`, which can be conveniently done using `gpExport`'s `MayaLibrary`-class. In the latter case one just needs instantiate this class, as shown in `executable.cpp`:
```
MayaLibrary library((char*)exporterName.c_str());

loadScene(inFile);

MFileObject outMFile;
outMFile.setFullName(outFile.c_str());
ExporterConfig config(outMFile, "", false);

exporter.Export(config);
```

Compiling the cpp-file is easy. The linking on the other hand is more difficult. I heavily advise to reuse `gpExport`'s Makefile. It could save you a lot of time.