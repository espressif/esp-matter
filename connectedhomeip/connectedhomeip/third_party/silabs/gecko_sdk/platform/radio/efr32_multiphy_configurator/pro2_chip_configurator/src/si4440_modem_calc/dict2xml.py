'''
Created on Apr 9, 2013

@author: sesuskic
'''
from xml.dom.minidom import Document
from collections import OrderedDict

__all__ = ["dict2xml"]

class dict2xml(object):

    def __init__(self, structure):
        self.doc     = Document()
        if len(structure) == 1:
            k = list(structure.keys())
            rootName    = str(k[0])
            self.root   = self.doc.createElement(rootName)

            self.doc.appendChild(self.root)
            self.build(self.root, structure[rootName])

    def build(self, father, structure):
        if (type(structure) == dict or
            type(structure) == OrderedDict):
            for k in structure:
                tag = self.doc.createElement(k)
                father.appendChild(tag)
                self.build(tag, structure[k])

        elif type(structure) == list:
            tagName     = father.tagName
            tag = self.doc.createElement(tagName)
            idx = 0
#             grandFather.removeChild(father)
            for l in structure:
                tag = self.doc.createElement(tagName + '_{:02}'.format(idx))
                self.build(tag, l)
                father.appendChild(tag)
                idx += 1

        else:
            data    = str(structure)
            tag     = self.doc.createTextNode(data)
            father.appendChild(tag)

    def display(self):
        return self.doc.toprettyxml(indent="    ")
