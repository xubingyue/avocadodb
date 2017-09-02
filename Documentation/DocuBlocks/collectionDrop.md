

@brief drops a collection
`collection.drop()`

Drops a *collection* and all its indexes.

@EXAMPLES

@EXAMPLE_AVOCADOSH_OUTPUT{collectionDrop}
~ db._create("example");
  col = db.example;
  col.drop();
  col;
~ db._drop("example");
@END_EXAMPLE_AVOCADOSH_OUTPUT


