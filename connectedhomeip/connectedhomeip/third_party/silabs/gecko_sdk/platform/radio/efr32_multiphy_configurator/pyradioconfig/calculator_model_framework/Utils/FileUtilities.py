import os


class FileUtilities(object):

    #
    # delete the output directory and recreate it.  Start with an empty
    # directory for output files
    #
    @staticmethod
    def cleandir(dir,file_types=None):
        if os.path.exists(dir):
            filelist = os.listdir(dir)
            for file in filelist:
                if file_types is not None:
                    for file_type in file_types:
                        if file.endswith(file_type):
                            os.remove(dir + "/" + file)
                else:
                    os.remove(dir + "/" + file)
        else:
            os.mkdir(dir)

    # Attempt to create a directory, and allow an already-existing directory.
    @staticmethod
    def makedirs(directory):
        try:
            os.makedirs(directory)
        except OSError as e:
            if e.errno != os.errno.EEXIST:
                raise

    @staticmethod
    def printRootStructure(dirname,indent=0):
        print(dirname)
        for i in range(indent):
            print("   ",)
        print(os.path.basename(dirname))  # changed
        if os.path.isdir(dirname):
            for files in os.listdir(dirname):
                FileUtilities.printRootStructure(os.path.join(dirname,files),indent+1)  # changed


    @staticmethod
    def resource_path(relative):
        return os.path.join(
            os.environ.get(
                "_MEIPASS2",
                os.path.abspath(".")
            ),
            relative
        )
