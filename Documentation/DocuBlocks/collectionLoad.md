

@brief loads a collection
`collection.load()`

Loads a collection into memory.

@EXAMPLES

@EXAMPLE_AVOCADOSH_OUTPUT{collectionLoad}
~ db._create("example");
  col = db.example;
  col.load();
  col;
~ db._drop("example");
@END_EXAMPLE_AVOCADOSH_OUTPUT


