"""
All files in this package are where phys, profiles, and calculators get defined.

The package structure should follow as follows:
    /parts/
        /[part_family]/
              /phys/          - WHere phy's are defined in classes
              /profiles/      - Where profiles are defined in classes
              /calculators/   - Where calculations are defined in classes
              /filters/       - Where phy filers are defined in classes
              __init__.py     - Module file
              _version.py     - Version tracking file
        /common/
              /calculators/   - Where core (global) calculations are defined in classes

"""