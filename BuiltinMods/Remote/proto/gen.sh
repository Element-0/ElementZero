#!/bin/bash
flatc --cpp "$@" --scoped-enums --gen-object-api *.fbs
