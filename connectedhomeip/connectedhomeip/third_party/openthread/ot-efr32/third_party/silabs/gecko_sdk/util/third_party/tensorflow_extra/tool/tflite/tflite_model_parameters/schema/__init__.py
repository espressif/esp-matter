import os 
import sys 

# We need to add the schema directory to the Python path
# as the generated flatbuffer modules expect the modules
# to be vailable at the root:
# e.g. The generated code contains lines like:
# from Entry import Entry
_curdir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(_curdir)