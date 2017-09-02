

@brief returns a single collection or null
`db._collection(collection-name)`

Returns the collection with the given name or null if no such collection
exists.

`db._collection(collection-identifier)`

Returns the collection with the given identifier or null if no such
collection exists. Accessing collections by identifier is discouraged for
end users. End users should access collections using the collection name.

@EXAMPLES

Get a collection by name:

@EXAMPLE_AVOCADOSH_OUTPUT{collectionDatabaseName}
  db._collection("demo");
@END_EXAMPLE_AVOCADOSH_OUTPUT

Get a collection by id:

```
avocadosh> db._collection(123456);
[AvocadoCollection 123456, "demo" (type document, status loaded)]
```

Unknown collection:

@EXAMPLE_AVOCADOSH_OUTPUT{collectionDatabaseNameUnknown}
  db._collection("unknown");
@END_EXAMPLE_AVOCADOSH_OUTPUT

