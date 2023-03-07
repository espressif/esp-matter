## Example Summary

The mailbox example shows how to use the `ti.sysbios.knl.Mailbox module` which
addresses the problem of a task needing to wait on data before continuing its
execution.

## Example Usage

* Run the application. Two tasks, `writerTask` and `readerTask`, will print
messages being passed between them to the console.

## Application Design Details

* A `MsgObj` struct is defined to be used for messages in this example. The
struct contains two data members: `id` and `val`. In `readerTask` and
`writerTask`, a local `MsgObj` is declared to store the mailbox data. When
using Mailbox_post or Mailbox_pend, the local `MsgObj msg` is copied to or
from the internal mailbox buffer, respectively.  When changing the data
members of this structure, considerations should be made for padding and
data alignment.

* A `MailboxMsgObj` struct is defined to be used for the mailbox buffer. The
struct contains two data members: `Mailbox_MbxElem elem` and `MsgObj obj`.
Mailbox messages are stored in a queue that requires a header in front of
each message. `Mailbox_MbxElem` is defined such that the header and its size
are factored into the total data size requirement for a mailbox instance.
Because Mailbox_MbxElem contains Int data types, padding may be added to this
struct depending on the data members defined in MsgObj.

> The application is responsible for ensuring the buffer supplied is of
sufficient size.

* A writer task, `writerTask`, is responsible for putting messages into the
mailbox.  It loops for `NUMMSGS` iterations; placing a message in the mailbox.
The values that are put in the mailbox are also print to the console.

* A reader task, `readerTask`, is responsible for reading messages from the
mailbox.  It loops for `NUMMSGS` iterations; blocking indefinitely until a
message is available to read. The values read are printed to the console.

> In this example, only one task is writing messages to the mailbox. However,
it is possible that multiple tasks can write messages to the same mailbox
instance.

## References
* For GNU and IAR users, please read the following website for details about
enabling [semi-hosting](http://processors.wiki.ti.com/index.php/TI-RTOS_Examples_SemiHosting)
in order to view console output.

* For more help, search the SYS/BIOS User Guide.
