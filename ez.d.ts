declare interface Long {
  "64bit_low": number,
  "64bit_high": number,
}

declare interface VanillaEntity {
  __type__: "entity";
  id: number;
  __unique_id__: Long;
  __identifier__: string;
}

declare interface Entity {
  readonly entityName: string;
  nameTag: string;
  pos: ScriptPosition;
  readonly valid: boolean;
  readonly vanilla: VanillaEntity;
  kill(): void;
}

declare interface PlayerEntry {
  readonly xuid: string;
  readonly uuid: string;
  readonly name: string;
  /** IP and port info */
  readonly address: string;
  /** Detect if player is still exists in server */
  readonly alive: boolean;
  /** A object shared between all instances */
  readonly aux: object;
  /** Get vanilla entity object */
  readonly vanilla: VanillaEntity;
  readonly entity: Entity;
  /** Get offline player entry */
  getOffline(): OfflinePlayerEntry;
  toString(): string;
}

declare interface OfflinePlayerEntry {
  readonly xuid: string;
  readonly uuid: string;
  readonly name: string;
  /** Get online player entry if the player is online, or you will get null */
  getOnline(): PlayerEntry | null;
  toString(): string;
}

declare module "ez:entity" {
  export function fromVanilla(vanilla: VanillaEntity): PlayerEntry | null;
}

declare module "ez:player" {
  export function getPlayerByXUID(xuid: string): PlayerEntry | null;
  export function getPlayerByUUID(uuid: string): PlayerEntry | null;
  export function getPlayerByNAME(name: string): PlayerEntry | null;
  export function getPlayerFromVanilla(vanilla: VanillaEntity): PlayerEntry | null;

  export function getOfflinePlayerByXUID(xuid: string): OfflinePlayerEntry | null;
  export function getOfflinePlayerByUUID(uuid: string): OfflinePlayerEntry | null;
  export function getOfflinePlayerByNAME(name: string): OfflinePlayerEntry | null;

  export function getPlayerList(): PlayerEntry[];

  /**
   * Called when player joined
   * @param cb Callback
   */
  export function onPlayerJoined(cb: (entry: PlayerEntry) => void): void;
  /**
   * Called when player initialized
   * @param cb Callback
   */
  export function onPlayerInitialized(cb: (entry: PlayerEntry) => void): void;
  /**
   * Called before player left
   * @param cb Callback
   */
  export function onPlayerLeft(cb: (entry: PlayerEntry) => void): void;
}

declare module "ez:basic-anti-cheat" {
  export function onDetected(
    cb: (entry: { name: string; target: PlayerEntry }) => void
  ): void;
}

declare module "ez:blacklist" {
  export function addByXUID(xuid: string, reason: string, op: string): void;
  export function addByUUID(uuid: string, reason: string, op: string): void;
  export function addByNAME(name: string, reason: string, op: string): void;
  export function removeByXUID(xuid: string): void;
  export function removeByUUID(uuid: string): void;
  export function removeByNAME(name: string): void;
}

declare module "ez:chat" {
  export function onChat(cb: (sender: string, content: string) => void): void;
  export function sendBroadcast(sender: string, content: string): void;
  export function sendAnnounce(content: string): void;
  export function send(target: PlayerEntry, content: string): void;
}

declare interface ScriptPosition {
  x: number;
  y: number;
  z: number;
}

declare interface CommandOrigin {
  type: number;
  entity: Entity;
  player?: PlayerEntry;
  name: string;
  dimension: number;
  permission: number;
  worldBuilder: number;
  blockpos: ScriptPosition;
  worldpos: ScriptPosition;
}

type CommandParameterTypeName =
  | "bool"
  | "int"
  | "float"
  | "string"
  | "json"
  | "enum"
  | "players"
  | "entities";

type CommandParameterTypeMap<Name extends CommandParameterTypeName> =
  Name extends "bool" ? boolean :
  Name extends "int" ? number :
  Name extends "float" ? number :
  Name extends "string" ? string :
  Name extends "json" ? Record<string, any> :
  Name extends "enum" ? number :
  Name extends "entities" ? Entity[] :
  Name extends "players" ? PlayerEntry[] :
  never;

declare interface CommandParameterDefinition {
  name: string;
  type: CommandParameterTypeName;
  optional: boolean;
  enum?: string;
}

type CommandParameterMap<Defs extends CommandParameterDefinition[]> = {
  [key in keyof Defs]: Defs[key] extends CommandParameterDefinition
  ? CommandParameterTypeMap<Defs[key]["type"]>
  : never;
};

type CommandHandlerResult = string | string[] | undefined;

declare module "ez:command" {
  /**
   * Execute server command
   * @param data command
   */
  export function executeCommand(
    data: string
  ): { statusMessage: string;[key: string]: any };

  /**
   * Set command handler for special `/!` command
   * @param handler handler function
   */
  export function setSlashCommandHandler(handler: (this: CommandOrigin, input: string) => string): void;

  /**
   * Register custom command
   * @param name command name
   * @param desc command description
   * @param permission permission level 0 = everyone 1 = op/commandblock 2 = op 4 = console
   */
  export function registerCommand(name: string, desc: string, permission: 0 | 1 | 2 | 3 | 4 | 5 | 6): void;

  /**
   * Register custom alias for command (max aliases per command: 1)
   * @param name original command name
   * @param altname new command name
   */
  export function registerAlias(name: string, altname: string): void

  /**
   * Register custom command handler
   * @param name command name
   * @param defs command parameter definitions
   * @param handler command handler
   */
  export function registerOverride<Defs extends CommandParameterDefinition[]>(
    name: string,
    defs: Defs,
    handler: (this: CommandOrigin, ...args: CommandParameterMap<Defs>) => CommandHandlerResult
  ): void;

  /**
   * Add command enum
   * @param name enum name
   * @param value enum values
   */
  export function addEnum(name: string, value: string[]): void
}

declare interface ItemStack {
  readonly raw_name: string;
  readonly name: string;
  readonly hover_name: string;
  readonly custom_hover_name: boolean;
  readonly id: number;
  readonly aux: number;
  readonly count: number;
  readonly max_count: number;
  readonly enchanted: boolean;
  readonly enchants: EnchantmentInstance[];
  dump(): ArrayBuffer;
  equals(stack: ItemStack): boolean;
  toString(): string;
}

declare interface EnchantmentInstance {
  type: number;
  level: number;
  name(): string;
  toString(): string;
}

declare module "ez:inventory" {
  export function getOffHandItem(player: PlayerEntry): ItemStack;
  export function getEquipmentItems(
    player: PlayerEntry
  ): [ItemStack, ItemStack, ItemStack, ItemStack];
  // prettier-ignore
  export function getInventoryItems(
    player: PlayerEntry
  ): [
      ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack,
      ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack,
      ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack,
      ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack,
    ];
  // prettier-ignore
  export function getEnderChestItems(
    player: PlayerEntry
  ): [
      ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack,
      ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack,
      ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack,
      ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack,
    ];
}

declare module "ez:utils" {
  export function delay(time: number): Promise<void>;
}

declare interface BossBar {
  /**
   * Detect if the bossbar still valid
   * It will be invalid after player left or having been destoryed
   */
  valid(): boolean;
  updateText(text: string): void;
  updatePercent(percent: number): void;
  show(): void;
  hide(): void;
  destory(): void;
}

declare module "ez:bossbar" {
  export function create(
    player: PlayerEntry,
    text: string,
    percent: number
  ): BossBar;
}

declare type Sqlite3BindType =
  | number
  | null
  | undefined
  | string
  | ArrayBuffer
  | ArrayBufferView;
declare type Sqlite3ColumnType = number | null | string | ArrayBuffer;

declare interface Sqlite3Statement {
  reset(): void;
  clearBindings(): void;
  bind(index: number | string, value: Sqlite3BindType): void;
  bindAll(obj: Array<Sqlite3BindType>): void;
  bindAll(obj: Record<string, Sqlite3BindType>): void;
  exec(): void;
  execWith(obj: Array<Sqlite3BindType>): void;
  execWith(obj: Record<string, Sqlite3BindType>): void;
  forEach(fn: (...columns: Array<Sqlite3ColumnType>) => boolean | void): void;
  forEachWith(
    obj: Array<Sqlite3BindType>,
    fn: (...columns: Array<Sqlite3ColumnType>) => boolean | void
  ): void;
  forEachWith(
    obj: Record<string, Sqlite3BindType>,
    fn: (...columns: Array<Sqlite3ColumnType>) => boolean | void
  ): void;
}

declare interface Sqlite3Database {
  exec(sql: string): void;

  prepare(sql: string): Sqlite3Statement;
}

declare module "ez:sqlite3" {
  export function open(filename: string): Sqlite3Database;
}

declare interface FakeScoreboard {
  init(target: PlayerEntry, name: string, is_asc: boolean): void;
  deinit(target: PlayerEntry): void;
  set(target: PlayerEntry, id: number, name: string, score: number): void;
  remove(target: PlayerEntry, id: number): void;
}

declare module "ez:scoreboard" {
  export const sidebar: FakeScoreboard;
  export const list: FakeScoreboard;
}

declare module "ez:economy" {
  export function getBalance(player: PlayerEntry): number;
  export function updateBalance(
    player: PlayerEntry,
    value: number,
    reason: string
  ): void;
}

declare module "ez:network-stats" {
  export function getNetworkStats(
    player: PlayerEntry
  ): {
    ping: number;
    avgping: number;
    packetloss: number;
    avgpacketloss: number;
  };
}

declare module "ez:formui" {
  /**
   * Send form to player
   * @param player target player
   * @param request form content
   * @param resp callback
   */
  export function send(player: PlayerEntry, request: object, resp: (fn: object) => void): void;
}

/// Encode text in utf-8
declare interface TextEncoder {
  new();
  encode(text: string): Uint8Array
}

/// Decode text in utf-8
declare interface TextDecoder {
  new();
  decode(text: string): Uint8Array
}

declare interface HttpResponse {
  status: number;
  statusText: string;
  headers: Record<string, string>;
  data: ArrayBuffer;
}

declare function HttpRequest(
  method: "GET" | "POST" | "HEAD" | "PUT" | "PATCH" | "DELETE",
  url: string,
  init: { headers: Record<string, string>, body: ArrayBuffer }
): Promise<HttpResponse>;

