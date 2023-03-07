from rail_scripts import rail_model_types

class RAILModel(object):

    def __init__(self, data):
        self._elements = []
        assert type(data) == dict and 'RAILModel' in data, "Invalid RAILModel data format"
        for name, value in data['RAILModel'].items():
            newElement = self._generateInstance(name, value)
            self._elements.append(newElement)

    # Override so we only expose public attributes
    def __dir__(self):
        return self._elements

    # This method is used to dynamically generate 'RAILModel*' objects that get
    # assigned to class attributes
    @classmethod
    def _generateInstance(self, name, value=None):
        if name == "_elements" and type(value) is list:
            for newElement in value:
                self._generateInstance(newElement)
        elif isinstance(name, dict):
            for key, value in name.items():
                self._generateInstance(key, value)
        elif name in rail_model_types.supportedClassType:
            className = getattr(rail_model_types, name)
            instance = className(model=self, **value)
            setattr(self, instance.name, instance)
            return instance.name
        else:
            setattr(self, name, value)
            return name
