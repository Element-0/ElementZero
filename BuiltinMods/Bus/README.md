# Mini bus mod
Remote control mod
working in progress

## Install backend
0. Install rust's cargo
1. Run `cargo install minibus`
2. Run minibus by `minibus`

options:
* `-l` listen port
* `--webapi` web port
* `--webbase` base path for api, currently used for basic security...

## APIs
* POST /map/elementzero/executeCommand
  * body is command content
  * response is command output in rawtext
* POST /map/elementzero/executeCommand.json
  * body is command content
  * response is command output in json format
* POST /map/elementzero/announce
  * body is announce content
  * response is empty
* GET(SSE) /listen/elementzero/chat
  * content format
    * first line: sender name
    * second line: content
* GET(SSE) /listen/elementzero/joined
* GET(SSE) /listen/elementzero/left
  * player info (name\nxuid\nuuid)
* POST /map/elementzero/find-player
  * body is player name/xuid/uuid
  * response is player info if found