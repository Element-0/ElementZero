cmake_minimum_required (VERSION 3.8)

def_mod (Remote
    LINK ws-gw flatbuffers::flatbuffers
    DELAY_LINK ChatAPI Blacklist)