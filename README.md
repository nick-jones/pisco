# Pisco

Pisco is a very basic in-memory wildcarded search service, interfaced via [Apache Thrift](http://thrift.apache.org/)

This is just a play project, and is very early stage. **Do not use this!**

## Running

Build, and then run the compiled binary:

```
make
./pisco
```

## Interfacing

The service can be interacted with via Apache Thrift libraries of any language flavour. To generate, run:

```
thrift --gen <language> def/pisco.thrift
```

.. substituting `<language>` with your language of choice. Refer to the
[Thrift documentation](http://thrift.apache.org/docs/) for further help.
