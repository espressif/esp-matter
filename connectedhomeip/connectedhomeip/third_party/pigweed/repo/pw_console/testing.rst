.. _module-pw_console-testing:

=====================
Manual Test Procedure
=====================

``pw_console`` is a Terminal based user interface which is difficult to
completely test in an automated fashion. Unit tests that don't depend on the
user interface are preferred but not always possible. For those situations
manual tests should be added here to validate expected behavior.

Run in Test Mode
================

Begin each section below by running the console in test mode:

.. code-block:: shell

  touch /tmp/empty.yaml
  env PW_CONSOLE_CONFIG_FILE='/tmp/empty.yaml' pw console --test-mode

Test Sections
=============

Log Pane: Basic Actions
^^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :widths: 5 45 45 5
   :header-rows: 1

   * - #
     - Test Action
     - Expected Result
     - ✅

   * - 1
     - Click the :guilabel:`Fake Device` window title
     - Log pane is focused
     - |checkbox|

   * - 2
     - In the main menu enable :guilabel:`[File] > Log Table View > Hide Date`
     - The time column shows only the time. E.g. ``09:34:53``.
     - |checkbox|

   * - 3
     - In the main menu turn off :guilabel:`[File] > Log Table View > Hide Date`
     - The time column shows the date and time. E.g. ``20220208 09:34:53``.
     - |checkbox|

   * - 4
     - Click :guilabel:`Search` on the log toolbar
     - | The search bar appears
       | The cursor should appear after the ``/``
     - |checkbox|

   * - 5
     - Press :kbd:`Ctrl-c`
     - The search bar disappears
     - |checkbox|

   * - 6
     - Click :guilabel:`Follow` on the log toolbar
     - Logs stop following
     - |checkbox|

   * - 7
     - Click :guilabel:`Table` on the log toolbar
     - Table mode is disabled
     - |checkbox|

   * - 8
     - Click :guilabel:`Wrap` on the log toolbar
     - Line wrapping is enabled
     - |checkbox|

   * - 9
     - Click :guilabel:`Clear` on the log toolbar
     - | All log lines are erased
       | Follow mode is on
       | New lines start appearing
     - |checkbox|

   * - 10
     - | Mouse drag across a few log messages
     - | Entire logs are highlighted and a dialog
       | box appears in the upper right
     - |checkbox|

   * - 11
     - | Without scrolling mouse drag across a set
       | of different log messages.
     - | The old selection disappears leaving only the new selection.
     - |checkbox|

   * - 12
     - | Click the :guilabel:`Cancel` button
       | in the selection dialog box.
     - | The selection and the dialog box disappears.
     - |checkbox|

   * - 13
     - | Mouse drag across a few log messages and
       | click the :guilabel:`Save as File` button.
     - | The save as file dialog appears with the
       | :guilabel:`[x] Selected Lines Only` opion checked.
     - |checkbox|

   * - 14
     - | Press :kbd:`Cancel`
     - | The save dialog closes
     - |checkbox|

   * - 15
     - | Click the :guilabel:`Save` button on the log toolbar.
       | A dialog appears prompting for a file.
     - | The current working directory should be pre-filled.
     - |checkbox|

   * - 16
     - | Check :guilabel:`[x] Table Formatting`
       | Uncheck :guilabel:`[ ] Selected Lines Only`
       | Add ``/log.txt`` to the end and press :kbd:`Enter`
       | Click the menu :guilabel:`[File] > Exit`
     - | In the terminal run ``cat log.txt`` to verify logs
       | were saved correctly.
     - |checkbox|

Log Pane: Search and Filtering
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :widths: 5 45 45 5
   :header-rows: 1

   * - #
     - Test Action
     - Expected Result
     - ✅

   * - 1
     - Click the :guilabel:`Fake Device` window title
     - Log pane is focused
     - |checkbox|

   * - 2
     - Press :kbd:`/`
     - | The search bar appears
       | The cursor should appear after the ``/``
     - |checkbox|

   * - 3
     - | Type ``lorem``
       | Press :kbd:`Enter`
     - | Logs stop following
       | ``Lorem`` words are highlighted in cyan
       | The cursor on the first log message
       | The search toolbar is un-focused and displays:
       | ``Match 1 / 10`` where the second number (the total match count)
       | increases once every 10 seconds when new logs arrive.
     - |checkbox|

   * - 4
     - Press :kbd:`Ctrl-f`
     - | The search bar is focused
       | The cursor should appear after ``/Lorem``
     - |checkbox|

   * - 5
     - Press :kbd:`Ctrl-c`
     - | The search bar disappears
       | ``Lorem`` words are no longer highlighted
     - |checkbox|

   * - 6
     - Press :kbd:`/`
     - | The search bar appears and is empty
       | The cursor should appear after ``/``
     - |checkbox|

   * - 7
     - Click :guilabel:`Matcher:` once
     - ``Matcher:STRING`` is shown
     - |checkbox|

   * - 8
     - | Type ``[=``
       | Press :kbd:`Enter`
     - | All instances of ``[=`` should be highlighted
       | The cursor should be on log message 2
     - |checkbox|

   * - 7
     - Press :kbd:`/`
     - | The search bar is focused
       | The cursor should appear after the ``/[=``
     - |checkbox|

   * - 8
     - Press :kbd:`Ctrl-c`
     - | The search bar disappears
       | ``[=`` matches are no longer highlighted
     - |checkbox|

   * - 9
     - Press :kbd:`/`
     - | The search bar appears and is empty
       | The cursor should appear after ``/``
     - |checkbox|

   * - 10
     - Press :kbd:`Up`
     - The text ``[=`` should appear in the search input field
     - |checkbox|

   * - 11
     - Click :guilabel:`Search Enter`
     - | All instances of ``[=`` should be highlighted
       | The cursor should be on log message 12
     - |checkbox|

   * - 12
     - Click :guilabel:`Add Filter`
     - | A ``Filters`` toolbar will appear
       | showing the new filter: ``<\[= (X)>``.
       | Only log messages matching ``[=`` appear in the logs.
       | Follow mode is enabled
     - |checkbox|

   * - 13
     - | Press :kbd:`/`
     - | The search bar appears and is empty
       | The cursor should appear after ``/``
     - |checkbox|

   * - 14
     - | Type ``# 1`` and press :kbd:`Enter`
       | Click :guilabel:`Add Filter`
     - | The ``Filters`` toolbar shows a new filter: ``<\#\ 1 (X)>``.
       | Only log messages matching both filters will appear in the logs.
     - |checkbox|

   * - 15
     - | Click the first :guilabel:`(X)`
       | in the filter toolbar.
     - | The ``Filters`` toolbar shows only one filter: ``<\#\ 1 (X)>``.
       | More log messages will appear in the log window
       | Lines all end in: ``# 1.*``
     - |checkbox|

   * - 16
     - Click :guilabel:`Clear Filters`
     - | The ``Filters`` toolbar will disappear.
       | All log messages will be shown in the log window.
     - |checkbox|

   * - 17
     - | Press :kbd:`/`
       | Type ``BAT``
       | Click :guilabel:`Column` until ``Column:Module`` is shown
       | Press :kbd:`Enter`
     - | Logs stop following
       | ``BAT`` is highlighted in cyan
       | The cursor on the 3rd log message
       | The search toolbar is un-focused and displays:
       | ``Match 1 / 10`` where the second number (the total match count)
     - |checkbox|

   * - 18
     - Press :kbd:`n`
     - | ``BAT`` is highlighted in cyan
       | The cursor on the 7th log message and is in the center of the
       | log window (not the bottom).
     - |checkbox|

   * - 19
     - Click :guilabel:`Jump to new matches`
     - | :guilabel:`Jump to new matches` is checked and every 5 seconds
       | the cursor jumps to the latest matching log message.
     - |checkbox|

   * - 20
     - Click :guilabel:`Follow`
     - | :guilabel:`Jump to new matches` is unchecked
       | The cursor jumps to every new log message once a second.
     - |checkbox|

   * - 21
     - | Click :guilabel:`Add Filter`
     - | The Filters toolbar appears with one filter: ``<module BAT (X)>``
       | Only logs with Module matching ``BAT`` appear.
     - |checkbox|

   * - 22
     - Click :guilabel:`Clear Filters`
     - | The ``Filters`` toolbar will disappear.
       | All log messages will be shown in the log window.
     - |checkbox|

   * - 23
     - | Press :kbd:`/`
       | Type ``BAT``
       | Click :guilabel:`Invert`
     - ``[x] Invert`` setting is shown
     - |checkbox|

   * - 24
     - | Press :kbd:`Enter` then click :guilabel:`Add Filter`
     - | The Filters toolbar appears
       | One filter is shown: ``<NOT module BAT (X)>``
       | Only logs with Modules other than ``BAT`` appear.
     - |checkbox|

Help Windows
^^^^^^^^^^^^

.. list-table::
   :widths: 5 45 45 5
   :header-rows: 1

   * - #
     - Test Action
     - Expected Result
     - ✅

   * - 1
     - Click the :guilabel:`[Help] > User Guide`
     - | Window appears showing the user guide with
       | RST formatting and syntax highlighting
     - |checkbox|

   * - 2
     - Press :guilabel:`q`
     - Window is hidden
     - |checkbox|

   * - 3
     - Click the :guilabel:`[Help] > Keyboard Shortcuts`
     - Window appears showing the keybind list
     - |checkbox|

   * - 4
     - Press :kbd:`F1`
     - Window is hidden
     - |checkbox|

   * - 5
     - Click the :guilabel:`[Help] > Console Test Mode Help`
     - | Window appears showing help with content
       | ``Welcome to the Pigweed Console Test Mode!``
     - |checkbox|

   * - 6
     - Click the :guilabel:`Close q` button.
     - Window is hidden
     - |checkbox|

Floating Windows
^^^^^^^^^^^^^^^^

.. list-table::
   :widths: 5 45 45 5
   :header-rows: 1

   * - #
     - Test Action
     - Expected Result
     - ✅

   * - 1
     - Start ``pw-console --test-mode`` press ``Ctrl-p``
     - The :guilabel:`Menu Items` command runner dialog appears.
     - |checkbox|

   * - 2
     - Type :kbd:`exit` and press :kbd:`enter`.
     - The console exits.
     - |checkbox|

   * - 3
     - Restart ``pw-console`` but without the ``--test-mode`` option.
     - Console starts up with ONLY the Python Results and Repl windows.
     - |checkbox|

   * - 4
     - Press ``Ctrl-p``
     - The :guilabel:`Menu Items` command runner dialog appears.
     - |checkbox|


Window Management
^^^^^^^^^^^^^^^^^

.. list-table::
   :widths: 5 45 45 5
   :header-rows: 1

   * - #
     - Test Action
     - Expected Result
     - ✅

   * - 1
     - | Click the :guilabel:`Fake Device` window title
     - Log pane is focused
     - |checkbox|

   * - 2
     - | Click the menu :guilabel:`Windows > #: Fake Device...`
       | Click :guilabel:`Duplicate pane`
     - | 3 panes are visible:
       | Log pane on top
       | Repl pane in the middle
       | Log pane on the bottom
     - |checkbox|

   * - 3
     - | Click the :guilabel:`Python Repl` window title
     - Python Repl pane is focused
     - |checkbox|

   * - 4
     - Click the :guilabel:`View > Move Window Down`
     - | 3 panes are visible:
       | Log pane on top
       | Log pane in the middle
       | Repl pane on the bottom
     - |checkbox|

   * - 5
     - Click the :guilabel:`View > Move Window Down` again
     - | Nothing changes
       | Windows remain in the same order
     - |checkbox|

   * - 6
     - Click the :guilabel:`View > Move Window Up`
     - | 3 panes are visible:
       | Log pane on top
       | Repl pane in the middle
       | Log pane on the bottom
     - |checkbox|

   * - 7
     - | Click the menu :guilabel:`Windows > #: Fake Device...`
       | Click :guilabel:`Remove pane`
     - | 2 panes are visible:
       | Repl pane on the top
       | Log pane on bottom
     - |checkbox|

   * - 8
     - | Click the :guilabel:`Python Repl`
       | window title
     - Repl pane is focused
     - |checkbox|

   * - 9
     - | Hold the keys :guilabel:`Alt- -`
       | `Alt` and `Minus`
     - Repl pane shrinks
     - |checkbox|

   * - 10
     - Hold the keys :guilabel:`Alt-=`
     - Repl pane enlarges
     - |checkbox|

   * - 11
     - | Click the menu :guilabel:`Windows > 1: Logs fake_device.1`
       | Click :guilabel:`Duplicate pane`
     - | 3 panes are visible:
       | 2 Log panes on the left
       | Repl pane on the right
     - |checkbox|

   * - 12
     - | Click the left top :guilabel:`Logs` window title
     - Log pane is focused
     - |checkbox|

   * - 13
     - Click the :guilabel:`View > Move Window Right`
     - | 3 panes are visible:
       | 1 Log panes on the left
       | 1 Log and Repl pane on the right
     - |checkbox|

   * - 14
     - | Click the menu :guilabel:`Windows > Column 2 View Modes`
       | Then click :guilabel:`[ ] Tabbed Windows`
     - | 2 panes are visible:
       | 1 Log panes on the left
       | 1 Log panes on the right
       | A tab bar on the top of the right side
       | `Logs fake_device.1` is highlighted
     - |checkbox|

   * - 15
     - | On the right side tab bar
       | Click :guilabel:`Python Repl`
     - | 2 panes are visible:
       | 1 Log pane on the left
       | 1 Repl pane on the right
       | `Python Repl` is highlighted
       | on the tab bar
     - |checkbox|

Mouse Window Resizing
^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :widths: 5 45 45 5
   :header-rows: 1

   * - #
     - Test Action
     - Expected Result
     - ✅

   * - 1
     - | Click the :guilabel:`Fake Device` window
     - Log pane is focused
     - |checkbox|

   * - 2
     - | Left click and hold the :guilabel:`-==-` of that window
       | Drag the mouse up and down
     - This log pane is resized
     - |checkbox|

   * - 3
     - | Left click and hold the :guilabel:`-==-`
       | of the :guilabel:`PwConsole Debug` window
       | Drag the mouse up and down
     - | The :guilabel:`PwConsole Debug` should NOT be focused
       | The window should be resized as expected
     - |checkbox|

   * - 4
     - Click the :guilabel:`View > Move Window Right`
     - :guilabel:`Fake Device` should appear in a right side split
     - |checkbox|

   * - 5
     - | Left click and hold anywhere on the vertical separator
       | Drag the mouse left and right
     - | The window splits should be resized as expected
     - |checkbox|

   * - 6
     - Click the :guilabel:`View > Balance Window Sizes`
     - Window split sizes should reset to equal widths
     - |checkbox|

   * - 7
     - | Focus on the :guilabel:`Python Repl` window
       | Click the :guilabel:`View > Move Window Left`
     - | :guilabel:`Python Repl` should appear in a left side split
       | There should be 3 vertical splits in total
     - |checkbox|

   * - 8
     - | Left click and hold anywhere on the vertical separator
       | between the first two splits (Python Repl and the middle split)
       | Drag the mouse left and right
     - | The first two window splits should be resized.
       | The 3rd split size should not change.
     - |checkbox|

Copy Paste
^^^^^^^^^^

.. list-table::
   :widths: 5 45 45 5
   :header-rows: 1

   * - #
     - Test Action
     - Expected Result
     - ✅

   * - 1
     - | Click the :guilabel:`Fake Device` window title
     - Log pane is focused
     - |checkbox|

   * - 2
     - | Mouse drag across a few log messages
     - | A dialog appears in the upper right showing
       | the number of lines selected and
       | buttons for :guilabel:`Cancel`, :guilabel:`Select All`,
       | :guilabel:`Save as File`, and :guilabel:`Copy`.
     - |checkbox|

   * - 3
     - | Click the :guilabel:`Copy` button
     - | Try pasting into a separate text editor
       | Log lines like this should be pasted:
       | ``20:07:25  INF  APP    Log message [    =     ] # 25``
       | ``20:07:25  INF  RADIO  Log message [     =    ] # 26``
     - |checkbox|

   * - 4
     - | Copy this text in your browser or
       | text editor to the system clipboard:
       | ``print('copy paste test!')``
     - | Click the :guilabel:`Python Repl` window title
       | Press :kbd:`Ctrl-v`
       | ``print('copy paste test!')`` appears
       | after the prompt.
     - |checkbox|

   * - 5
     - Press :kbd:`Enter`
     - | This appears in Python Results:
       | ``In [1]: print('copy paste test!')``
       | ``copy paste test!``
     - |checkbox|

   * - 6
     - | Click :guilabel:`Ctrl-Alt-c -> Copy Output`
       | on the Python Results toolbar
       | Try pasting into a separate text editor
     - | The contents of the Python Results
       | are in the system clipboard.
     - |checkbox|

   * - 7
     - Click the :guilabel:`Python Results` window title
     - | Python Results is focused with cursor
       | appearing below the last line
     - |checkbox|

   * - 8
     - | Click and drag over ``copy paste text``
       | highlighting won't appear until
       | after the mouse button is released
     - | ``copy paste text`` is highlighted
     - |checkbox|

   * - 9
     - | Press :kbd:`Ctrl-c`
       | Try pasting into a separate text editor
     - | ``copy paste text`` should appear (and is
       | in the system clipboard)
     - |checkbox|

   * - 10
     - Click the :guilabel:`Python Repl` window title
     - Python Repl is focused
     - |checkbox|

   * - 11
     - | Type ``print('hello there')`` into the Python input.
       | Mouse drag select that text
       | Press :kbd:`Ctrl-c`
     - | The selection should disappear.
       | Try pasting into a separate text editor, the paste should
       | match the text you drag selected.
     - |checkbox|

Incremental Stdout
^^^^^^^^^^^^^^^^^^

.. list-table::
   :widths: 5 45 45 5
   :header-rows: 1

   * - #
     - Test Action
     - Expected Result
     - ✅

   * - 1
     - | Click the :guilabel:`Python Repl` window title
     - Python Repl pane is focused
     - |checkbox|

   * - 2
     - | Enter the following text and hit enter twice
       | ``import time``
       | ``for i in range(10):``
       | ``print(i); time.sleep(1)``
     - | ``Running...`` should appear in the python with
       | increasing integers incrementally appearing above
       | (not all at once after a delay).
     - |checkbox|

Python Repl & Output
^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :widths: 5 45 45 5
   :header-rows: 1

   * - #
     - Test Action
     - Expected Result
     - ✅

   * - 1
     - Click the ``Logs`` window title
     - Log pane is focused
     - |checkbox|

   * - 2
     - Click empty whitespace in the ``Python Results`` window
     - Python Results pane is focused
     - |checkbox|

   * - 3
     - Click empty whitespace in the ``Python Repl`` window
     - Python Repl pane is focused
     - |checkbox|

   * - 4
     - | Enter the following text and press :kbd:`Enter` to run
       | ``[i for i in __builtins__ if not i.startswith('_')]``
     - | The results should appear pretty printed
       | with each list element on it's own line:
       |
       |   >>> [i for i in __builtins__ if not i.startswith('_')]
       |   [ 'abs',
       |     'all',
       |     'any',
       |     'ascii'
       |
     - |checkbox|

   * - 5
     - | Enter the following text and press :kbd:`Enter` to run
       | ``globals()``
     - | The results should appear pretty printed
     - |checkbox|

   * - 6
     - | With the cursor over the Python Results,
       | use the mouse wheel to scroll up and down.
     - | The output window should be able to scroll all
       | the way to the beginning and end of the buffer.
     - |checkbox|

   * - 7
     - Click empty whitespace in the ``Python Repl`` window
     - Python Repl pane is focused
     - |checkbox|

   * - 8
     - | Enter the following text and press :kbd:`Enter` to run
       | ``!ls``
     - | 1. Shell output of running the ``ls`` command should appear in the
       | results window.
       | 2. A new log window pane should appear titled ``Shell Output``.
       | 3. The Shell Output window should show the command that was run and the
       | output:
       | ``$ ls``
       | ``activate.bat``
       | ``activate.sh``
     - |checkbox|

Web Log Viewer
^^^^^^^^^^^^^^

.. list-table::
   :widths: 5 45 45 5
   :header-rows: 1

   * - #
     - Test Action
     - Expected Result
     - ✅

   * - 1
     - | Start the pw console test mode by
       | running ``pw console --test-mode``
     - | Console starts up showing an ``Fake Device`` window.
     - |checkbox|

   * - 2
     - | Focus on ``Fake Device`` panel and press :kbd:`Shift-o` to enable web log viewer
     - | This should hide log stream in the console and automatically copy the
       | URL to log viewer to the clipboard
     - |checkbox|
   * - 3
     - | Focus on the ``Fake Keys`` panel with a filter applied. Then press
       | :kbd:`Shift-o` to enable another web log viewer for that new pane. Open the
       | new URL in Chrome
     - | This log viewer should have filters pre-applied
     - |checkbox|
   * - 4
     - | Press :kbd:`Shift-o` again on both log panes to disable web log view
     - | This should re-enable log stream in console and stop streaming logs to
       | web view
     - |checkbox|



Early Startup
^^^^^^^^^^^^^

.. list-table::
   :widths: 5 45 45 5
   :header-rows: 1

   * - #
     - Test Action
     - Expected Result
     - ✅

   * - 1
     - | Start the pw console test mode by
       | running ``pw console --test-mode``
     - | Console starts up showing an ``All Logs`` window.
     - |checkbox|

   * - 2
     - | Click the :guilabel:`All Logs` window title
       | Press :kbd:`g` to jump to the top of the log history
     - | These log messages should be at the top:
       | ``DBG Adding plugins...``
       | ``DBG Starting prompt_toolkit full-screen application...``
       | ``DBG pw_console test-mode starting...``
       | ``DBG pw_console.PwConsoleEmbed init complete``
     - |checkbox|

Quit Confirmation Dialog
^^^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :widths: 5 45 45 5
   :header-rows: 1

   * - #
     - Test Action
     - Expected Result
     - ✅

   * - 1
     - | Press :kbd:`Ctrl-d`
     - | The quit dialog appears
     - |checkbox|

   * - 2
     - | Press :kbd:`n`
     - | The quit dialog disappears
     - |checkbox|

   * - 3
     - | Press :kbd:`Ctrl-d`
     - | The quit dialog appears
     - |checkbox|

   * - 4
     - | Press :kbd:`y`
     - | The console exits
     - |checkbox|

   * - 5
     - | Restart the console and
       | Press :kbd:`Ctrl-d` twice in quick succession.
     - | The console exits
     - |checkbox|

   * - 6
     - | Restart the console and Press :kbd:`F1`
     - | The help window appears
     - |checkbox|

   * - 7
     - | Press :kbd:`Ctrl-d`
     - | The quit dialog appears on top of the help window
     - |checkbox|

   * - 8
     - | Press :kbd:`n`
     - | The quit dialog disappears and the help window is
       | back in focus.
     - |checkbox|

   * - 9
     - | Press :kbd:`q`
     - | The help window disappears and the Python Repl is in focus.
     - |checkbox|

   * - 10
     - | Type some text into the Python Repl.
       | Press :kbd:`Home` or move the cursor to the
       | beginning of the text you just entered.
       | Press :kbd:`Ctrl-d`
     - | Each :kbd:`Ctrl-d` press deletes one character
     - |checkbox|

   * - 11
     - | Press :kbd:`Ctrl-c` to clear the Python Repl text
       | Press :kbd:`Ctrl-d`
     - | The quit dialog appears.
     - |checkbox|

Add note to the commit message
==============================

Add a ``Testing:`` line to your commit message and mention the steps
executed. For example:

.. code-block:: text

   Testing: Log Pane Steps 1-6

.. |checkbox| raw:: html

    <input type="checkbox">
