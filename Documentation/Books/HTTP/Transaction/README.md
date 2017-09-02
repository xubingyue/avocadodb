HTTP Interface for Transactions
===============================

### Transactions

AvocadoDB's transactions are executed on the server. Transactions can be 
initiated by clients by sending the transaction description for execution to
the server.

Transactions in AvocadoDB do not offer separate *BEGIN*, *COMMIT* and *ROLLBACK*
operations as they are available in many other database products. 
Instead, AvocadoDB transactions are described by a JavaScript function, and the 
code inside the JavaScript function will then be executed transactionally.
At the end of the function, the transaction is automatically committed, and all
changes done by the transaction will be persisted. If an exception is thrown
during transaction execution, all operations performed in the transaction are
rolled back.

For a more detailed description of how transactions work in AvocadoDB please
refer to [Transactions](../../Manual/Transactions/index.html). 

<!-- js/actions/api-transaction.js -->
@startDocuBlock JSF_post_api_transaction
