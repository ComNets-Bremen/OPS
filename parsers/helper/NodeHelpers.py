#!/usr/bin/env python

# Node helpers
#
# @author Jens Dede (jd@comnets.uni-bremen.de)
# 2018-01-13
#

## Helper to handle node data
#
class NodeInformationHandler:
    m_nodes = {}    # Dict to store the node information
    m_node_data_structure = {}

    def __init__(self, initDict=None):
        if initDict:
            self.m_nodes = initDict["node_data"]
            self.m_node_data_structure = initDict["data_structure"]

    ## Return the current defined base dictionary used by the instance
    #
    # @param name name  The name of the object
    def getNodeDict(self, name=""):
        newDict = dict(self.m_node_data_structure)
        newDict["name"] = name
        return newDict

    # Add a key to the structure. As default, only "name" is set
    #
    # @param key        The key to be added
    # @param default    The default value, default: 0
    def addKey(self, key, default=0):
        self.m_node_data_structure[key] = default

    ## String representation to get some meaningful printout information
    #
    def __str__(self):
        return "NodeInformationHandler with " + str(len(self.m_nodes)) + " nodes"

    ## Get a certain node dict by name
    #
    # @param name   name of the node
    def getNode(self, name):
        if not name in self.m_nodes:
            self.m_nodes[name] = self.getNodeDict(name)
        return self.m_nodes[name]

    ## Return all known nodes
    #
    def getKnownNodes(self):
        return self.m_nodes.keys()

    ## Get the number of known nodes
    #
    def getLength(self):
        return len(self.m_nodes)

    ## Accumulate all node data, ignore some nodes
    #
    # @param ignore_nodes   List of ignored nodes
    def getTotalData(self, ignore_nodes = []):
        allData = self.getNodeDict("total")
        for node in self.m_nodes:
            if node in ignore_nodes:
                continue
            nodeObj = self.m_nodes[node]
            for key in nodeObj:
                if key == "name":
                    continue
                allData[key] += nodeObj[key]
        return allData

    ## Return all data stored in this instance
    #
    def exportData(self):
        return {
                "node_data": self.m_nodes,
                "data_structure" : self.m_node_data_structure
                }


if __name__ == "__main__":
    # Testing functionality

    myNodes = NodeInformationHandler()

    keys = ["my_node_data", "allItems", "data_sent"]
    for key in keys:
        myNodes.addKey(key)

    print("Testnode 1", myNodes.getNode("Test"))
    print("Testnode 2", myNodes.getNode("Test2"))

    myNodes.getNode("Test")["data_sent"] += 2

    print(myNodes.getNode("Test")["data_sent"])
    print(myNodes.getNode("Test2")["data_sent"])

    myNodes.getNode("Test2")["data_sent"] -= 11

    print(myNodes.getNode("Test")["data_sent"])
    print(myNodes.getNode("Test2")["data_sent"])

    print(myNodes.getKnownNodes())

    print(myNodes.getTotalData())
    print(myNodes.getTotalData(["Test",]))

    # Export Data to dict
    exportDict = myNodes.exportData()

    # Create a new instance based on exported data
    mySecondDict = NodeInformationHandler(exportDict)
    print(mySecondDict)
