# Framework #

This page discusses `gpExport`'s usage as framework.

## Architecture ##
![http://gpexport.googlecode.com/svn/wiki/gpExporter_uml.png](http://gpexport.googlecode.com/svn/wiki/gpExporter_uml.png)

Although the UML-diagram doesn't show it, all classes inherit virtually from `gpBase`, which is just a helper-class providing a status-variable (needed for almost all Maya-calls), and ways to check it.

The `doXXX`- and `shouldXXX`-functions are template-methods, to configure the exporter. They are all taken care of by GPExporter, which takes the configuration from the exporterConfig, that has been passed to it (`IExporterConfig` is basically just an interface for all these parameters).

In addition to the shown classes, gpExport provides two additional classes:
  * a converter-class
  * and a fileWriter-class

Both of them are utility-classes for implementing the abstract WriteScene-method.

![http://gpexport.googlecode.com/svn/wiki/converter_fileWriter.png](http://gpexport.googlecode.com/svn/wiki/converter_fileWriter.png)

## Extending gpExport ##
This section essentially describes, how to use gpExport to easily implement a standard exporter. I'll take MS3D (MilkShake3D) as example. As this is really not a MS3D-tutorial, I won't go into details for the writing-part though.

### Exporter ###

Let's start by completing the GPExporter-class ms3dExporter.hpp:

```
class MS3DExporter
    : virtual protected GPBase,
      public GPExporter,
      protected MS3DWriter
{
public:
    void Export(const IExporterConfig& config)
        { GPExporter::Export(config); }

protected:
    void WriteScene(const Scene& extractedScene,
                    const std::string& outFile)
        { MS3DWriter::WriteScene(extractedScene, outFile); }
};
```

`MS3DExporter` redirects its Export-method to the `GPExporter`-class, which will call `WriteScene` after having extracted the necessary information. `WriteScene` itself is forwarded to the `MS3DWriter`-class.

`MS3DWriter` uses `Converter` and `FileWriter` to do its task. In particulary it doesn't implement the `WriteScene` itself, but provides the `doWriteScene`-method, which is called by the `FileWriter`-class.

Completing the GPExporter-class is not sufficient. One still needs to create a plugin or executable.

### Plugin ###
`gpExport` already provides a basic plugin-class that should be sufficient for most uses. Obviously it isn't complete, as the plugin-class contains exporter-specific information (the name, extension, etc.). Information, that can be demanded at runtime over virtual methods are pure virtual in the `gpExport`-plugin class. These are the `getExtension`- and `Export`-method. A custom exporter therefore needs to implement these methods (`ms3dPlugin.cpp`):

```
class MS3DPlugin
    : public GPTranslation::Plugin
```

```
// template-method (of GPTranslation::Plugin)
std::string
getExtension() const
    {
        return extension;
    }
```

```
void
Export(const IExporterConfig& config)
    {
        // MS3DExporter::Export(config); // can't do :(
        MS3DExporter exporter;
        exporter.Export(config);
    }
```

In addition Maya needs two static methods: `initializePlugin` and `uninitializePlugin`. `initializePlugin` provides a plugin-description (`MPnPlugin`) to Maya, and registers the exporter (translator). One parameter is a static function, that allows to create our translator:

```
static
void*
creator()
    {
        return new MS3DPlugin();
    }
```

```
MStatus initializePlugin(MObject obj)
{
    using namespace MS3DTranslation;

    MFnPlugin plugin(obj,
                     vendor,
                     version,
                     requiredApiVersion);

    // Register the translator:
    return plugin.registerFileTranslator(translatorName,
                                         (char*)pixmapName,
                                         MS3DPlugin::creator,
                                         (char*)optionsScript,
                                         (char*)defaultOptions);
}
```

All those variables (`vendor`, ...) have been defined earlier:

```
const char* const translatorName = "MS3D_Export";
const char* const pixmapName = "none";
const char* const optionsScript = "MS3D_opts";
const char* const defaultOptions = "";
const char* const extension = "ms3d";
const char* const vendor = "OpenSource";
const char* const version = "0.1";
const char* const requiredApiVersion = "Any";
```

And, to finish, the `uninitializePlugin`-function:

```
MStatus uninitializePlugin(MObject obj)
{
    MFnPlugin plugin(obj);
    return plugin.deregisterFileTranslator(MS3DTranslation::translatorName);
}
```

### Executable ###
The code for executables is even simpler: One just needs to provide the classical `main`-function, and call `exportScene` of the `executable.hpp`-header (ms3dExecutable):

```
int
main(int argc, char** argv)
{
    try
    {
        if (argc != 3)
        {
            printUsage(argv[0]);
            return -1;
        }

        MS3DTranslation::MS3DExporter exporter;
        
        GPTranslation::exportScene(argv[0],
                                   exporter,
                                   argv[1],
                                   argv[2]);
        return 0;
    }
    catch (...)
    {
        return -1;
    }
}
```

Here is the signature of the exportScene-function:

```
void exportScene(const std::string& exporterName,
                 GPExporter& exporter,
                 const std::string& inFile,
                 const std::string& outFile);
```

This simplicity induces some restrictions: there's no way of paramizing the exporter. If this is needed (which is usually the case), one needs to copy some lines out of the `executable.cpp`-file, and implement the `exportScene`-method on his own. Fortunately the code is short (6 code-lines) and not difficult.

The only difficulty in creating executables lies in the linking. A Maya-executable needs a bunch of libraries (in the correct order). If you want to win some time, use `gpExport`'s Makefile.