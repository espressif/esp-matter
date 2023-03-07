.. _module-pw_console-internals:

Internal Design
===============

Threads and Event Loops
-----------------------

In `ptpython`_ and `IPython`_ all user repl code is run in the foreground. This
allows interrupts like ``Ctrl-C`` and functions like ``print()`` and
``time.sleep()`` to work as expected. Pigweed's Console doesn't use this
approach as it would hide or freeze the `prompt_toolkit`_ user interface while
running repl code.

To get around this issue all user repl code is run in a dedicated thread with
stdout and stderr patched to capture output. This lets the user interface stay
responsive and new log messages to continue to be displayed.

Here's a diagram showing how ``pw_console`` threads and `asyncio`_ tasks are
organized.

.. mermaid::

   flowchart LR
       classDef eventLoop fill:#e3f2fd,stroke:#90caf9,stroke-width:1px;
       classDef thread fill:#fffde7,stroke:#ffeb3b,stroke-width:1px;
       classDef plugin fill:#fce4ec,stroke:#f06292,stroke-width:1px;
       classDef builtinFeature fill:#e0f2f1,stroke:#4db6ac,stroke-width:1px;

       %% Subgraphs are drawn in reverse order.

       subgraph pluginThread [Plugin Thread 1]
           subgraph pluginLoop [Plugin Event Loop 1]
               toolbarFunc-->|"Refresh<br/>UI Tokens"| toolbarFunc
               toolbarFunc[Toolbar Update Function]
           end
           class pluginLoop eventLoop;
       end
       class pluginThread thread;

       subgraph pluginThread2 [Plugin Thread 2]
           subgraph pluginLoop2 [Plugin Event Loop 2]
               paneFunc-->|"Refresh<br/>UI Tokens"| paneFunc
               paneFunc[Pane Update Function]
           end
           class pluginLoop2 eventLoop;
       end
       class pluginThread2 thread;

       subgraph replThread [Repl Thread]
           subgraph replLoop [Repl Event Loop]
               Task1 -->|Finished| Task2 -->|Cancel with Ctrl-C| Task3
           end
           class replLoop eventLoop;
       end
       class replThread thread;

       subgraph main [Main Thread]
           subgraph mainLoop [User Interface Event Loop]
               log[[Log Pane]]
               repl[[Python Repl]]
               pluginToolbar([User Toolbar Plugin])
               pluginPane([User Pane Plugin])
               class log,repl builtinFeature;
               class pluginToolbar,pluginPane plugin;
           end
           class mainLoop eventLoop;
       end
       class main thread;

       repl-.->|Run Code| replThread
       pluginToolbar-.->|Register Plugin| pluginThread
       pluginPane-.->|Register Plugin| pluginThread2

.. _IPython: https://ipython.readthedocs.io/
.. _prompt_toolkit: https://python-prompt-toolkit.readthedocs.io/
.. _asyncio: https://docs.python.org/3/library/asyncio.html
.. _ptpython: https://github.com/prompt-toolkit/ptpython/
