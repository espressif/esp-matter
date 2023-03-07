
"""
SVD Info
^^^^^^^^
Information about the source CMSIS SVD file used to create this package.

  .. autoclass:: RM_SVD_Info
      :members:
      :show-inheritance:
      :inherited-members:

"""


__all__ = [ 'RM_SVD_Info' ]


class RM_SVD_Info(object):
    """
    Any register/field/enum names that are aliased during the generation
    of this python static source code package are stored in an ``svdInfo``
    attribute in the top-level device class. This attribute is an instance
    instance of this class.

    .. code-block:: py

          # show entire formatted info
          print(rm.svdInfo)

    :vartype filename: ``str``
    :ivar filename: The filename of the CMSIS SVD XML used to generate this package.
    :vartype md5sum: ``str``
    :ivar md5sum: The MD5 hex digest of the CMSIS SVD XML used to generate this package.
    :vartype aliased_regs: ``list`` of [``str``, ``str``] or ``NoneType``
    :ivar aliased_regs: A list of [ [ <orig_name>, <aliased_name>], ... ]
    :vartype aliased_fields: ``list`` of [``str``, ``str``] or ``NoneType``
    :ivar aliased_fields: A list of [ [ <orig_name>, <aliased_name>], ... ]
    :vartype aliased_enums: ``list`` of [``str``, ``str``] or ``NoneType``
    :ivar aliased_enums: A list of [ [ <orig_name>, <aliased_name>], ... ]


    """

    def __init__(self, filename, md5sum, aliased_regs=None, aliased_fields=None,
                 aliased_enums=None):
        """
        Store the filename, compute the MD5 hash, and stored full names
        for any aliased registers, fields, and enums.

        :type  filename: ``str``
        :param filename: The filename of the SVD XML file.
        :type  md5sum: ``str``
        :param md5sum: The MD5 hex digest of the SVD XML file.
        :type  aliased_regs: ``list`` of [``str``, ``str``]
        :param aliased_regs: list of [ <orig_name>, <aliased_name>]
        :type  aliased_fields: ``list`` of [``str``, ``str``]
        :param aliased_fields: list of [ <orig_name>, <aliased_name>]
        :type  aliased_enums: ``list`` of [``str``, ``str``]
        :param aliased_enums: list of [ <orig_name>, <aliased_name>]
        """
        self.filename = filename
        self.md5sum = md5sum
        self.aliased_regs = aliased_regs
        self.aliased_fields = aliased_fields
        self.aliased_enums = aliased_enums

    def __str__(self):
        out = ""
        out += "  Filename:   {}\n".format(self.filename)
        out += "  MD5SUM:     {}\n".format(self.md5sum)
        if self.aliased_regs:
            out += "  Aliased Registers: {}\n".format(len(self.aliased_regs))
            for orig, alias in self.aliased_regs:
                out += "    {} -> {}\n".format(orig, alias)

        if self.aliased_fields:
            out += "  Aliased Fields:    {}\n".format(len(self.aliased_fields))
            for orig, alias in self.aliased_fields:
                out += "    {} -> {}\n".format(orig, alias)

        if self.aliased_enums:
            out += "Aliased Enums: {}\n".format(len(self.aliased_enums))
            for orig, alias in self.aliased_enums:
                out += "    {} -> {}\n".format(orig, alias)

        return out
