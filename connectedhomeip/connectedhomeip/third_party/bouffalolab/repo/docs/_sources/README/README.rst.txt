How to add .h files
-------------------

-  for example: we have a head File named ``example.h`` in
   ``components/sys`` directory,then change to ``docs`` directory,modify
   the ``INPUT`` configuration in the ``Doxyfile`` file,here we should
   add a line with ``../../components/sys/example.h`` .
-  change to ``docs/zh_CH/API/sys`` subdirectory,new a file named
   ``example.rst`` .
-  reference ``docs/zh_CH/API/sys/cronalarms.rst`` ,here we replace
   ``cronalarms`` with ``example`` .
-  change to ``docs/zh_CH`` subdirectory,modify ``index.rst`` ,add a
   line with ``API/sys/example`` in the last line of the file.

Building Documentation
----------------------

-  At first,run ``pip3 install --user -r requirements.txt`` install
   environment
-  Change to docs/zh\_CN subdirectory and run ``make html``
-  ``make`` will probably prompt you to run a python pip install step to
   get some other Python-related prerequisites. Run the command as
   shown, then re-run ``make html`` to build the docs.

