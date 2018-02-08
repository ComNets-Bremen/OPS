#!/usr/bin/env python

# File helpers
#
# @author Jens Dede (jd@comnets.uni-bremen.de)
# 2018-01-11
#

import os.path

## Helper to create output files based on the input file name
#
class FileHelper(object):
    inputFile = None

    ## Constructor
    #
    # @param self       The object
    # @param inputFile  The input filename
    # @param absPath    Shall the filename be converted to an absolute path? Default: False
    def __init__(self, inputFile, absPath=False):
        self.setInputFile(inputFile, absPath)

    ## Return the input filename
    #
    # @param self       The object
    def getInputFile(self):
        return self.inputFile

    ## Set the input filename
    #
    # @param self       The object
    # @param inputFile  The input filename
    # @param absPath    Shall the filename be converted to an absolute path? Default: False
    def setInputFile(self, inputFile, absPath=False):
        if absPath:
            self.inputFile = os.path.abspath(inputFile)
        else:
            self.inputFile = inputFile

    ## Get the directory where the input file is located
    #
    # @param self       The object
    def getInputFileDir(self):
        return os.path.dirname(self.inputFile)

    ## Get the complete filename of the input file
    #
    # @param self       The object
    def getInputFilename(self):
        return os.path.basename(self.inputFile)

    ## Get the base filename (i.e., w/o the file extension) of the input file
    #
    # @param self       The object
    def getInputFileBaseName(self):
        return os.path.splitext(self.getInputFilename())[0]

    ## Get the file extension of the input file
    #
    # @param self       The object
    def getInputFileExtension(self):
        return os.path.splitext(self.getInputFilename())[1]

    ## Get the input file with a .pickle file extension
    #
    # @param self       The object
    def getPickleFilename(self):
        return os.path.join(self.getInputFileDir(), self.getInputFileBaseName()) + ".pickle"

    ## Get a generic output file in the directory of the input file
    #
    # @param self       The object
    # @param prefix     The prefix of the new filename, default: ""
    # @param suffix     The suffix of the new filename, default: "_out"
    # @param extension  The new file extension, default: "txt"
    def getGenericOutput(self, prefix="", suffix="_out", extension="txt"):
        newFilename = prefix + self.getInputFileBaseName() + suffix + "." + extension
        return os.path.join(self.getInputFileDir(), newFilename)

if __name__ == "__main__":
    print("Test functions in this module")
    filenames = [
            "/home/user/myFile.ini",
            "myRelFile.log",
            ]

    for filename in filenames:
        fh = FileHelper(filename)
        print(10*"#")
        print("Input filename               :", fh.getInputFile())
        print("Directory                    :", fh.getInputFileDir())
        print("File with original extension :", fh.getInputFilename())
        print("Base name (w/o extension)    :", fh.getInputFileBaseName())
        print("Path for pickle file         :", fh.getPickleFilename())
        print("Generic Output               :", fh.getGenericOutput())
