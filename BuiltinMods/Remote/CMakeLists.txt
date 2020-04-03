cmake_minimum_required (VERSION 3.8)

add_mod (Remote ws-gw flatbuffers::flatbuffers)
delay_load (Remote ChatAPI Blacklist)