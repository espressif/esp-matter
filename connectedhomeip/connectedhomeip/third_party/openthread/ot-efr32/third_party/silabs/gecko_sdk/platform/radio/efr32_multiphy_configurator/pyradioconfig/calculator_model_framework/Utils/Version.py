import re

__all__ = [ 'Version' ]

"""
Sortable Version class in the following format [major].[minor].[patch]
"""
class Version(object):
    major = 0
    minor = 0
    patch = 0

    def __init__(self, major, minor, patch):
        self.major = int(major)
        self.minor = int(minor)
        self.patch = int(patch)

    # Constructor from string only
    @classmethod
    def fromString(self, version):
        versionSplit = re.findall(r'\d+', version)
        return self(versionSplit[0], versionSplit[1], versionSplit[2])

    def __repr__(self):
        return str(self.major) + "." + str(self.minor) + "." + str(self.patch)

    def __str__(self):
        return str(self.major) + "." + str(self.minor) + "." + str(self.patch)

    # For sorting
    def __eq__(self, other):
        return self.__dict__ == other.__dict__

    def __lt__(self, other):
        # Compare the card with another card
        # (return true if we are smaller, false if
        # we are larger, 0 if we are the same)
        if self.major < other.major:
            return True
        elif self.major > other.major:
            return False

        if self.minor < other.minor:
            return True
        elif self.minor > other.minor:
            return False

        if self.patch < other.patch:
            return True
        elif self.patch > other.patch:
            return False

        return 0
