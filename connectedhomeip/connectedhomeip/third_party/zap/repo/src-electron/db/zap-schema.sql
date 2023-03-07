/*
 * 
 * $$$$$$$\                     $$\                                              
 * $$  __$$\                    $$ |                                             
 * $$ |  $$ |$$$$$$\   $$$$$$$\ $$ |  $$\ $$$$$$\   $$$$$$\   $$$$$$\   $$$$$$$\ 
 * $$$$$$$  |\____$$\ $$  _____|$$ | $$  |\____$$\ $$  __$$\ $$  __$$\ $$  _____|
 * $$  ____/ $$$$$$$ |$$ /      $$$$$$  / $$$$$$$ |$$ /  $$ |$$$$$$$$ |\$$$$$$\  
 * $$ |     $$  __$$ |$$ |      $$  _$$< $$  __$$ |$$ |  $$ |$$   ____| \____$$\ 
 * $$ |     \$$$$$$$ |\$$$$$$$\ $$ | \$$\\$$$$$$$ |\$$$$$$$ |\$$$$$$$\ $$$$$$$  |
 * \__|      \_______| \_______|\__|  \__|\_______| \____$$ | \_______|\_______/ 
 *                                                 $$\   $$ |                    
 *                                                 \$$$$$$  |                    
 *                                                  \______/                     
 * 
 * You can create these giant separators via:
 * http://patorjk.com/software/taag/#p=display&f=Big%20Money-nw
 */
/*
 Global SQLite settings.
 */
PRAGMA foreign_keys = ON;
/*
 PACKAGE table contains the "packages" that are the sources for the
 loading of the other data. They may be individual files, or
 collection of files.
 
 Table records the CRC of the toplevel file at the time loading.
 */
DROP TABLE IF EXISTS "PACKAGE";
CREATE TABLE "PACKAGE" (
  "PACKAGE_ID" integer primary key autoincrement,
  "PARENT_PACKAGE_REF" integer,
  "PATH" text NOT NULL,
  "TYPE" text,
  "CRC" integer,
  "VERSION" integer,
  "CATEGORY" text,
  "DESCRIPTION" text,
  foreign key (PARENT_PACKAGE_REF) references PACKAGE(PACKAGE_ID)
);
/*
 PACKAGE_OPTION table contains generic 'options' that are encoded from within each packages. 
 */
DROP TABLE IF EXISTS "PACKAGE_OPTION";
CREATE TABLE "PACKAGE_OPTION" (
  "OPTION_ID" integer primary key autoincrement,
  "PACKAGE_REF" integer,
  "OPTION_CATEGORY" text,
  "OPTION_CODE" text,
  "OPTION_LABEL" text,
  foreign key (PACKAGE_REF) references PACKAGE(PACKAGE_ID) on delete cascade,
  UNIQUE(PACKAGE_REF, OPTION_CATEGORY, OPTION_CODE)
);
/* 
 PACKAGE_OPTION_DEFAULT table contains a link to a specified 'default value' for an options
 */
DROP TABLE IF EXISTS "PACKAGE_OPTION_DEFAULT";
CREATE TABLE "PACKAGE_OPTION_DEFAULT" (
  "OPTION_DEFAULT_ID" integer primary key autoincrement,
  "PACKAGE_REF" integer,
  "OPTION_CATEGORY" text,
  "OPTION_REF" integer,
  foreign key (PACKAGE_REF) references PACKAGE(PACKAGE_ID) on delete cascade,
  foreign key (OPTION_REF) references PACKAGE_OPTION(OPTION_ID) on delete cascade,
  UNIQUE(PACKAGE_REF, OPTION_CATEGORY)
);
/*
 PACKAGE EXTENSIONS table contains extensions of specific ZCL entities attached to the
 gen template packages. See docs/sdk-extensions.md, the section about "Template key: zcl"
 */
DROP TABLE IF EXISTS "PACKAGE_EXTENSION";
CREATE TABLE "PACKAGE_EXTENSION" (
  "PACKAGE_EXTENSION_ID" integer primary key autoincrement,
  "PACKAGE_REF" integer,
  "ENTITY" text,
  "PROPERTY" text,
  "TYPE" text,
  "CONFIGURABILITY" text,
  "LABEL" text,
  "GLOBAL_DEFAULT" text,
  foreign key (PACKAGE_REF) references PACKAGE(PACKAGE_ID) on delete cascade,
  UNIQUE(PACKAGE_REF, ENTITY, PROPERTY)
);
/*
 PACKAGE_EXTENSION_DEFAULTS table contains default values for specific entities. Each row provides
 default value for one item of a given entity, listed in PACKAGE_EXTENSION
 */
DROP TABLE IF EXISTS "PACKAGE_EXTENSION_DEFAULT";
CREATE TABLE "PACKAGE_EXTENSION_DEFAULT" (
  "PACKAGE_EXTENSION_REF" integer,
  "ENTITY_CODE" integer,
  "ENTITY_QUALIFIER" text,
  "PARENT_CODE" integer,
  "MANUFACTURER_CODE" integer,
  "VALUE" text,
  foreign key (PACKAGE_EXTENSION_REF) references PACKAGE_EXTENSION(PACKAGE_EXTENSION_ID) on delete cascade
);
/*
 *
 * $$$$$$$$\          $$\       $$\      $$\                 $$\           $$\ 
 * \____$$  |         $$ |      $$$\    $$$ |                $$ |          $$ |
 *     $$  / $$$$$$$\ $$ |      $$$$\  $$$$ | $$$$$$\   $$$$$$$ | $$$$$$\  $$ |
 *    $$  / $$  _____|$$ |      $$\$$\$$/$$ |$$  __$$\ $$  __$$ |$$  __$$\ $$ |
 *   $$  /  $$ /      $$ |      $$ \$$$ .$$ |$$ /  $$ |$$ /  $$ |$$$$$$$$ |$$ |
 *  $$  /   $$ |      $$ |      $$ |\$  /$$ |$$ |  $$ |$$ |  $$ |$$   ____|$$ |
 * $$$$$$$$\\$$$$$$$\ $$ |      $$ | \_/ $$ |\$$$$$$  |\$$$$$$$ |\$$$$$$$\ $$ |
 * \________|\_______|\__|      \__|     \__| \______/  \_______| \_______|\__|                                                                                                                                                                                                                                    
 */
/*
 SPEC table contains the spec information.
 */
DROP TABLE IF EXISTS "SPEC";
CREATE TABLE IF NOT EXISTS "SPEC" (
  "SPEC_ID" integer primary key autoincrement,
  "PACKAGE_REF" integer,
  "CODE" text NOT NULL,
  "DESCRIPTION" text,
  "CERTIFIABLE" integer,
  foreign key (PACKAGE_REF) references PACKAGE(PACKAGE_ID) on delete cascade,
  UNIQUE(PACKAGE_REF, CODE)
);
/*
 DOMAIN table contains domains directly loaded from packages.
 */
DROP TABLE IF EXISTS "DOMAIN";
CREATE TABLE IF NOT EXISTS "DOMAIN" (
  "DOMAIN_ID" integer primary key autoincrement,
  "PACKAGE_REF" integer,
  "NAME" text,
  "LATEST_SPEC_REF" integer,
  foreign key (PACKAGE_REF) references PACKAGE(PACKAGE_ID) on delete cascade,
  foreign key (LATEST_SPEC_REF) references SPEC(SPEC_ID),
  UNIQUE(PACKAGE_REF, NAME)
);
/*
 CLUSTER table contains the clusters loaded from the ZCL XML files.
 */
DROP TABLE IF EXISTS "CLUSTER";
CREATE TABLE IF NOT EXISTS "CLUSTER" (
  "CLUSTER_ID" integer primary key autoincrement,
  "PACKAGE_REF" integer,
  "DOMAIN_NAME" text,
  "CODE" integer,
  "MANUFACTURER_CODE" integer,
  "NAME" text,
  "DESCRIPTION" text,
  "DEFINE" text,
  "IS_SINGLETON" integer,
  "REVISION" integer,
  "INTRODUCED_IN_REF" integer,
  "REMOVED_IN_REF" integer,
  foreign key (INTRODUCED_IN_REF) references SPEC(SPEC_ID),
  foreign key (REMOVED_IN_REF) references SPEC(SPEC_ID),
  foreign key (PACKAGE_REF) references PACKAGE(PACKAGE_ID) on delete cascade
);
/*
 COMMAND table contains commands contained inside a cluster.
 */
DROP TABLE IF EXISTS "COMMAND";
CREATE TABLE IF NOT EXISTS "COMMAND" (
  "COMMAND_ID" integer primary key autoincrement,
  "CLUSTER_REF" integer,
  "PACKAGE_REF" integer,
  "CODE" integer,
  "MANUFACTURER_CODE" integer,
  "NAME" text,
  "DESCRIPTION" text,
  "SOURCE" text,
  "IS_OPTIONAL" integer,
  "MUST_USE_TIMED_INVOKE" integer,
  "IS_FABRIC_SCOPED" integer,
  "INTRODUCED_IN_REF" integer,
  "REMOVED_IN_REF" integer,
  "RESPONSE_NAME" integer,
  "RESPONSE_REF" integer,
  "IS_DEFAULT_RESPONSE_ENABLED" integer,
  foreign key (INTRODUCED_IN_REF) references SPEC(SPEC_ID),
  foreign key (REMOVED_IN_REF) references SPEC(SPEC_ID),
  foreign key (CLUSTER_REF) references CLUSTER(CLUSTER_ID),
  foreign key (PACKAGE_REF) references PACKAGE(PACKAGE_ID) on delete cascade,
  foreign key (RESPONSE_REF) references COMMAND(COMMAND_ID)
);
/*
 COMMAND_ARG table contains arguments for a command.
 */
DROP TABLE IF EXISTS "COMMAND_ARG";
CREATE TABLE IF NOT EXISTS "COMMAND_ARG" (
  "COMMAND_REF" integer,
  "FIELD_IDENTIFIER" integer,
  "NAME" text,
  "TYPE" text,
  "MIN" text,
  "MAX" text,
  "MIN_LENGTH" integer,
  "MAX_LENGTH" integer,
  "IS_ARRAY" integer,
  "PRESENT_IF" text,
  "IS_NULLABLE" integer,
  "IS_OPTIONAL" integer,
  "COUNT_ARG" text,
  "INTRODUCED_IN_REF" integer,
  "REMOVED_IN_REF" integer,
  foreign key (INTRODUCED_IN_REF) references SPEC(SPEC_ID),
  foreign key (REMOVED_IN_REF) references SPEC(SPEC_ID),
  foreign key (COMMAND_REF) references COMMAND(COMMAND_ID) on delete cascade
);
/*
 EVENT table contains events for a given cluster. 
 */
DROP TABLE IF EXISTS "EVENT";
CREATE TABLE IF NOT EXISTS "EVENT" (
  "EVENT_ID" integer primary key autoincrement,
  "CLUSTER_REF" integer,
  "PACKAGE_REF" integer,
  "CODE" integer,
  "MANUFACTURER_CODE" integer,
  "NAME" text,
  "DESCRIPTION" text,
  "SIDE" text,
  "IS_OPTIONAL" integer,
  "IS_FABRIC_SENSITIVE" integer,
  "PRIORITY" text,
  "INTRODUCED_IN_REF" integer,
  "REMOVED_IN_REF" integer,
  foreign key (CLUSTER_REF) references CLUSTER(CLUSTER_ID),
  foreign key (PACKAGE_REF) references PACKAGE(PACKAGE_ID) on delete cascade,
  foreign key (INTRODUCED_IN_REF) references SPEC(SPEC_ID),
  foreign key (REMOVED_IN_REF) references SPEC(SPEC_ID)
);
/*
 EVENT_FIELD table contains events for a given cluster. 
 */
DROP TABLE IF EXISTS "EVENT_FIELD";
CREATE TABLE IF NOT EXISTS "EVENT_FIELD" (
  "EVENT_REF" integer,
  "FIELD_IDENTIFIER" integer,
  "NAME" text,
  "TYPE" text,
  "IS_ARRAY" integer,
  "IS_NULLABLE" integer,
  "IS_OPTIONAL" integer,
  "INTRODUCED_IN_REF" integer,
  "REMOVED_IN_REF" integer,
  foreign key (INTRODUCED_IN_REF) references SPEC(SPEC_ID),
  foreign key (REMOVED_IN_REF) references SPEC(SPEC_ID),
  foreign key (EVENT_REF) references EVENT(EVENT_ID) on delete cascade
);
/*
 ATTRIBUTE table contains attributes for the cluster.
 */
DROP TABLE IF EXISTS "ATTRIBUTE";
CREATE TABLE IF NOT EXISTS "ATTRIBUTE" (
  "ATTRIBUTE_ID" integer primary key autoincrement,
  "CLUSTER_REF" integer,
  "PACKAGE_REF" integer,
  "CODE" integer,
  "MANUFACTURER_CODE" integer,
  "NAME" text,
  "TYPE" text,
  "SIDE" text,
  "DEFINE" text,
  "MIN" text,
  "MAX" text,
  "MIN_LENGTH" integer,
  "MAX_LENGTH" integer,
  "REPORT_MIN_INTERVAL" integer,
  "REPORT_MAX_INTERVAL" integer,
  "REPORTABLE_CHANGE" text,
  "REPORTABLE_CHANGE_LENGTH" integer,
  "IS_WRITABLE" integer,
  "DEFAULT_VALUE" text,
  "IS_SCENE_REQUIRED" integer,
  "IS_OPTIONAL" integer,
  "REPORTING_POLICY" text,
  "STORAGE_POLICY" text,
  "IS_NULLABLE" integer,
  "ARRAY_TYPE" text,
  "MUST_USE_TIMED_WRITE" integer,
  "INTRODUCED_IN_REF" integer,
  "REMOVED_IN_REF" integer,
  foreign key (INTRODUCED_IN_REF) references SPEC(SPEC_ID),
  foreign key (REMOVED_IN_REF) references SPEC(SPEC_ID),
  foreign key (CLUSTER_REF) references CLUSTER(CLUSTER_ID),
  foreign key (PACKAGE_REF) references PACKAGE(PACKAGE_ID) on delete cascade
);
/*
 GLOBAL_ATTRIBUTE_DEFAULT table contains default values of attributes per cluster.
 Note that for the regular attribute defaults are already provided in DEFAULT_VALUE
 column in ATTRIBUTE table. The only place where this is needed is for the global
 attributes, which have CLUSTER_REF set to null in attribute table, so you need
 a per-cluster space for different default values.
 
 If a certain cluster/attribute combination does not exist in this table, the value
 should be table from ATTRIBUTE table directly.
 */
DROP TABLE IF EXISTS "GLOBAL_ATTRIBUTE_DEFAULT";
CREATE TABLE IF NOT EXISTS "GLOBAL_ATTRIBUTE_DEFAULT" (
  "GLOBAL_ATTRIBUTE_DEFAULT_ID" integer primary key autoincrement,
  "CLUSTER_REF" integer NOT NULL,
  "ATTRIBUTE_REF" integer NOT NULL,
  "DEFAULT_VALUE" text,
  foreign key(CLUSTER_REF) references CLUSTER(CLUSTER_ID),
  foreign key(ATTRIBUTE_REF) references ATTRIBUTE(ATTRIBUTE_ID) on delete cascade
);
/*
 GLOBAL_ATTRIBUTE_BIT is carrying information about the mappings of a 
 bit for a given global attribute value. Example are FeatureMap global
 attributes in Matter implementation. For that case, the value
 of global attribute carries both the value, as well as the meaning
 of which bit corresponds to whith TAG. Hence this separate table that
 links those.
 */
DROP TABLE IF EXISTS "GLOBAL_ATTRIBUTE_BIT";
CREATE TABLE IF NOT EXISTS "GLOBAL_ATTRIBUTE_BIT" (
  "GLOBAL_ATTRIBUTE_DEFAULT_REF" integer NOT NULL,
  "BIT" integer NOT NULL,
  "VALUE" integer,
  "TAG_REF" integer NOT NULL,
  foreign key(GLOBAL_ATTRIBUTE_DEFAULT_REF) references GLOBAL_ATTRIBUTE_DEFAULT(GLOBAL_ATTRIBUTE_DEFAULT_ID) on delete cascade,
  foreign key(TAG_REF) references TAG(TAG_ID)
);
/*
 DEVICE_TYPE table contains device types directly loaded from packages.
 */
DROP TABLE IF EXISTS "DEVICE_TYPE";
CREATE TABLE IF NOT EXISTS "DEVICE_TYPE" (
  "DEVICE_TYPE_ID" integer primary key autoincrement,
  "PACKAGE_REF" integer,
  "DOMAIN" text,
  "CODE" integer,
  "PROFILE_ID" integer,
  "NAME" text,
  "DESCRIPTION" text,
  foreign key (PACKAGE_REF) references PACKAGE(PACKAGE_ID) on delete cascade
);
/*
 DEVICE_TYPE_CLUSTER contains clusters that belong to the device type.
 */
DROP TABLE IF EXISTS "DEVICE_TYPE_CLUSTER";
CREATE TABLE IF NOT EXISTS "DEVICE_TYPE_CLUSTER" (
  "DEVICE_TYPE_CLUSTER_ID" integer primary key autoincrement,
  "DEVICE_TYPE_REF" integer,
  "CLUSTER_REF" integer,
  "CLUSTER_NAME" text,
  "INCLUDE_CLIENT" integer,
  "INCLUDE_SERVER" integer,
  "LOCK_CLIENT" integer,
  "LOCK_SERVER" integer,
  foreign key (DEVICE_TYPE_REF) references DEVICE_TYPE(DEVICE_TYPE_ID) on delete cascade,
  foreign key (CLUSTER_REF) references CLUSTER(CLUSTER_ID)
);
/*
 DEVICE_TYPE_ATTRIBUTE contains attribuets that belong to a device type cluster. 
 */
DROP TABLE IF EXISTS "DEVICE_TYPE_ATTRIBUTE";
CREATE TABLE IF NOT EXISTS "DEVICE_TYPE_ATTRIBUTE" (
  "DEVICE_TYPE_CLUSTER_REF" integer,
  "ATTRIBUTE_REF" integer,
  "ATTRIBUTE_NAME" text,
  foreign key (DEVICE_TYPE_CLUSTER_REF) references DEVICE_TYPE_CLUSTER(DEVICE_TYPE_CLUSTER_ID) on delete cascade,
  foreign key (ATTRIBUTE_REF) references ATTRIBUTE(ATTRIBUTE_ID) on delete cascade
);
/*
 DEVICE_TYPE_COMMAND contains attributes that belong to a device type cluster. 
 */
DROP TABLE IF EXISTS "DEVICE_TYPE_COMMAND";
CREATE TABLE IF NOT EXISTS "DEVICE_TYPE_COMMAND" (
  "DEVICE_TYPE_CLUSTER_REF" integer,
  "COMMAND_REF" integer,
  "COMMAND_NAME" text,
  foreign key (DEVICE_TYPE_CLUSTER_REF) references DEVICE_TYPE_CLUSTER(DEVICE_TYPE_CLUSTER_ID) on delete cascade,
  foreign key (COMMAND_REF) references COMMAND(COMMAND_ID) on delete cascade
);
/*
 TAG table contains tags. They can be used for access control and feature maps.
 */
DROP TABLE IF EXISTS "TAG";
CREATE TABLE IF NOT EXISTS "TAG" (
  "TAG_ID" integer primary key autoincrement,
  "PACKAGE_REF" integer,
  "CLUSTER_REF" integer,
  "NAME" text,
  "DESCRIPTION" text,
  foreign key (PACKAGE_REF) references PACKAGE(PACKAGE_ID) on delete cascade,
  foreign key (CLUSTER_REF) references CLUSTER(CLUSTER_ID) on delete cascade
);
/*
 *
 * $$$$$$$$\                                      
 * \__$$  __|                                     
 *    $$ |$$\   $$\  $$$$$$\   $$$$$$\   $$$$$$$\ 
 *    $$ |$$ |  $$ |$$  __$$\ $$  __$$\ $$  _____|
 *    $$ |$$ |  $$ |$$ /  $$ |$$$$$$$$ |\$$$$$$\  
 *    $$ |$$ |  $$ |$$ |  $$ |$$   ____| \____$$\ 
 *    $$ |\$$$$$$$ |$$$$$$$  |\$$$$$$$\ $$$$$$$  |
 *    \__| \____$$ |$$  ____/  \_______|\_______/ 
 *        $$\   $$ |$$ |                          
 *        \$$$$$$  |$$ |                          
 *         \______/ \__|                          
 */
/*
 DISCRIMINATOR table contains the data types loaded from packages
 */
DROP TABLE IF EXISTS "DISCRIMINATOR";
CREATE TABLE IF NOT EXISTS "DISCRIMINATOR" (
  "DISCRIMINATOR_ID" integer NOT NULL PRIMARY KEY autoincrement,
  "NAME" text,
  "PACKAGE_REF" integer,
  FOREIGN KEY (PACKAGE_REF) REFERENCES PACKAGE(PACKAGE_ID),
  CONSTRAINT DISCRIMINATOR_INFO UNIQUE("NAME", "PACKAGE_REF")
);
/*
 DATA_TYPE table contains the all data types loaded from packages
 */
DROP TABLE IF EXISTS "DATA_TYPE";
CREATE TABLE IF NOT EXISTS "DATA_TYPE" (
  "DATA_TYPE_ID" integer NOT NULL PRIMARY KEY autoincrement,
  "NAME" text,
  "DESCRIPTION" text,
  "DISCRIMINATOR_REF" integer,
  "PACKAGE_REF" integer,
  FOREIGN KEY (DISCRIMINATOR_REF) REFERENCES DISCRIMINATOR(DISCRIMINATOR_ID) ON DELETE CASCADE ON UPDATE CASCADE,
  FOREIGN KEY (PACKAGE_REF) REFERENCES PACKAGE(PACKAGE_ID) ON DELETE CASCADE ON UPDATE CASCADE
);
/*
 DATA_TYPE_CLUSTER table is a junction table between the data types and clusters.
 This table stores the information on which data types are shared across clusters
 Note: The reason for having cluster code in this table is to load the Cluster
 reference during post loading. In terms of the schema an exception was made for
 loading cluster references into this table. For eg: See processZclPostLoading
 */
DROP TABLE IF EXISTS DATA_TYPE_CLUSTER;
CREATE TABLE DATA_TYPE_CLUSTER (
  DATA_TYPE_CLUSTER_ID integer NOT NULL PRIMARY KEY autoincrement,
  CLUSTER_REF integer,
  CLUSTER_CODE integer,
  DATA_TYPE_REF integer,
  FOREIGN KEY (CLUSTER_REF) REFERENCES CLUSTER(CLUSTER_ID) ON DELETE CASCADE ON UPDATE CASCADE,
  FOREIGN KEY (DATA_TYPE_REF) REFERENCES DATA_TYPE(DATA_TYPE_ID) ON DELETE CASCADE ON UPDATE CASCADE
);
/*
 NUMBER table contains the all numbers loaded from packages
 */
DROP TABLE IF EXISTS "NUMBER";
CREATE TABLE NUMBER (
  NUMBER_ID integer NOT NULL PRIMARY KEY,
  SIZE integer,
  IS_SIGNED integer,
  FOREIGN KEY (NUMBER_ID) REFERENCES DATA_TYPE(DATA_TYPE_ID) ON DELETE CASCADE ON UPDATE CASCADE
);
/*
 STRING table contains the all strings loaded from packages
 */
DROP TABLE IF EXISTS "STRING";
CREATE TABLE STRING (
  STRING_ID integer NOT NULL PRIMARY KEY,
  IS_LONG integer,
  SIZE integer,
  IS_CHAR integer,
  FOREIGN KEY (STRING_ID) REFERENCES DATA_TYPE(DATA_TYPE_ID) ON DELETE CASCADE ON UPDATE CASCADE
);
/*
 ATOMIC table contains the atomic types loaded from packages
 */
DROP TABLE IF EXISTS "ATOMIC";
CREATE TABLE IF NOT EXISTS "ATOMIC" (
  "ATOMIC_ID" integer primary key autoincrement,
  "PACKAGE_REF" integer,
  "NAME" text,
  "DESCRIPTION" text,
  "ATOMIC_IDENTIFIER" integer,
  "ATOMIC_SIZE" integer,
  "IS_DISCRETE" integer default false,
  "IS_STRING" integer default false,
  "IS_LONG" integer default false,
  "IS_CHAR" integer default false,
  "IS_SIGNED" integer default false,
  foreign key (PACKAGE_REF) references PACKAGE(PACKAGE_ID)
);
/*
 BITMAP table contains the bitmaps directly loaded from packages.
 */
DROP TABLE IF EXISTS "BITMAP";
CREATE TABLE IF NOT EXISTS BITMAP (
  BITMAP_ID integer NOT NULL PRIMARY KEY,
  SIZE integer,
  FOREIGN KEY (BITMAP_ID) REFERENCES DATA_TYPE(DATA_TYPE_ID) ON DELETE CASCADE ON UPDATE CASCADE
);
/*
 BITMAP_FIELD contains items that make up a bitmap.
 */
DROP TABLE IF EXISTS "BITMAP_FIELD";
CREATE TABLE IF NOT EXISTS BITMAP_FIELD (
  BITMAP_FIELD_ID integer NOT NULL PRIMARY KEY autoincrement,
  BITMAP_REF integer,
  FIELD_IDENTIFIER integer,
  NAME text(100),
  MASK integer,
  FOREIGN KEY (BITMAP_REF) REFERENCES BITMAP(BITMAP_ID)
);
/*
 ENUM table contains enums directly loaded from packages.
 */
DROP TABLE IF EXISTS "ENUM";
CREATE TABLE IF NOT EXISTS "ENUM" (
  ENUM_ID integer NOT NULL PRIMARY KEY,
  SIZE integer,
  FOREIGN KEY (ENUM_ID) REFERENCES DATA_TYPE(DATA_TYPE_ID) ON DELETE CASCADE ON UPDATE CASCADE
);
/*
 ENUM_ITEM table contains individual enum items.
 */
DROP TABLE IF EXISTS "ENUM_ITEM";
CREATE TABLE IF NOT EXISTS "ENUM_ITEM" (
  "ENUM_ITEM_ID" integer NOT NULL PRIMARY KEY autoincrement,
  "ENUM_REF" integer,
  "NAME" text,
  "DESCRIPTION" text,
  "FIELD_IDENTIFIER" integer,
  "VALUE" integer,
  FOREIGN KEY (ENUM_REF) REFERENCES "ENUM"(ENUM_ID) ON DELETE CASCADE ON UPDATE CASCADE
);
/*
 STRUCT table contains structs directly loaded from packages.
 */
DROP TABLE IF EXISTS "STRUCT";
CREATE TABLE IF NOT EXISTS STRUCT (
  STRUCT_ID integer NOT NULL PRIMARY KEY,
  IS_FABRIC_SCOPED integer,
  SIZE integer,
  FOREIGN KEY (STRUCT_ID) REFERENCES DATA_TYPE(DATA_TYPE_ID)
);
/*
 STRUCT_ITEM table contains individual struct items.
 */
DROP TABLE IF EXISTS "STRUCT_ITEM";
CREATE TABLE IF NOT EXISTS STRUCT_ITEM (
  STRUCT_ITEM_ID integer NOT NULL PRIMARY KEY autoincrement,
  STRUCT_REF integer,
  FIELD_IDENTIFIER integer,
  NAME text(100),
  IS_ARRAY integer,
  IS_ENUM integer,
  MIN_LENGTH integer,
  MAX_LENGTH integer,
  IS_WRITABLE integer,
  IS_NULLABLE integer,
  IS_OPTIONAL integer,
  IS_FABRIC_SENSITIVE integer,
  SIZE integer,
  DATA_TYPE_REF integer NOT NULL,
  FOREIGN KEY (STRUCT_REF) REFERENCES STRUCT(STRUCT_ID) ON DELETE CASCADE ON UPDATE CASCADE,
  FOREIGN KEY (DATA_TYPE_REF) REFERENCES DATA_TYPE(DATA_TYPE_ID) ON DELETE CASCADE ON UPDATE CASCADE
);
/*
 *  $$$$$$\                                                    
 * $$  __$$\                                                   
 * $$ /  $$ | $$$$$$$\  $$$$$$$\  $$$$$$\   $$$$$$$\  $$$$$$$\ 
 * $$$$$$$$ |$$  _____|$$  _____|$$  __$$\ $$  _____|$$  _____|
 * $$  __$$ |$$ /      $$ /      $$$$$$$$ |\$$$$$$\  \$$$$$$\  
 * $$ |  $$ |$$ |      $$ |      $$   ____| \____$$\  \____$$\ 
 * $$ |  $$ |\$$$$$$$\ \$$$$$$$\ \$$$$$$$\ $$$$$$$  |$$$$$$$  |
 * \__|  \__| \_______| \_______| \_______|\_______/ \_______/ 
 */
DROP TABLE IF EXISTS "OPERATION";
CREATE TABLE IF NOT EXISTS "OPERATION" (
  "OPERATION_ID" integer primary key autoincrement,
  "PACKAGE_REF" integer,
  "NAME" text,
  "DESCRIPTION" text,
  foreign key (PACKAGE_REF) references PACKAGE(PACKAGE_ID) on delete cascade
);
DROP TABLE IF EXISTS "ROLE";
CREATE TABLE IF NOT EXISTS "ROLE" (
  "ROLE_ID" integer primary key autoincrement,
  "PACKAGE_REF" integer,
  "LEVEL" integer,
  "NAME" text,
  "DESCRIPTION" text,
  foreign key (PACKAGE_REF) references PACKAGE(PACKAGE_ID) on delete cascade
);
DROP TABLE IF EXISTS "ACCESS_MODIFIER";
CREATE TABLE IF NOT EXISTS "ACCESS_MODIFIER" (
  "ACCESS_MODIFIER_ID" integer primary key autoincrement,
  "PACKAGE_REF" integer,
  "NAME" text,
  "DESCRIPTION" text,
  foreign key (PACKAGE_REF) references PACKAGE(PACKAGE_ID) on delete cascade
);
DROP TABLE IF EXISTS "ACCESS";
CREATE TABLE IF NOT EXISTS "ACCESS" (
  "ACCESS_ID" integer primary key autoincrement,
  "OPERATION_REF" integer,
  "ROLE_REF" integer,
  "ACCESS_MODIFIER_REF" integer,
  foreign key (OPERATION_REF) references OPERATION(OPERATION_ID),
  foreign key (ROLE_REF) references ROLE(ROLE_ID),
  foreign key (ACCESS_MODIFIER_REF) references ACCESS_MODIFIER(ACCESS_MODIFIER_ID)
);
DROP TABLE IF EXISTS "CLUSTER_ACCESS";
CREATE TABLE IF NOT EXISTS "CLUSTER_ACCESS" (
  "CLUSTER_REF" integer,
  "ACCESS_REF" integer,
  foreign key(ACCESS_REF) references ACCESS(ACCESS_ID),
  foreign key(CLUSTER_REF) references CLUSTER(CLUSTER_ID)
);
DROP TABLE IF EXISTS "ATTRIBUTE_ACCESS";
CREATE TABLE IF NOT EXISTS "ATTRIBUTE_ACCESS" (
  "ATTRIBUTE_REF" integer,
  "ACCESS_REF" integer,
  foreign key(ACCESS_REF) references ACCESS(ACCESS_ID),
  foreign key(ATTRIBUTE_REF) references ATTRIBUTE(ATTRIBUTE_ID)
);
DROP TABLE IF EXISTS "COMMAND_ACCESS";
CREATE TABLE IF NOT EXISTS "COMMAND_ACCESS" (
  "COMMAND_REF" integer,
  "ACCESS_REF" integer,
  foreign key(ACCESS_REF) references ACCESS(ACCESS_ID),
  foreign key(COMMAND_REF) references COMMAND(COMMAND_ID)
);
DROP TABLE IF EXISTS "EVENT_ACCESS";
CREATE TABLE IF NOT EXISTS "EVENT_ACCESS" (
  "EVENT_REF" integer,
  "ACCESS_REF" integer,
  foreign key(ACCESS_REF) references ACCESS(ACCESS_ID),
  foreign key(EVENT_REF) references EVENT(EVENT_ID)
);
DROP TABLE IF EXISTS "DEFAULT_ACCESS";
CREATE TABLE IF NOT EXISTS "DEFAULT_ACCESS" (
  "PACKAGE_REF" integer,
  "ENTITY_TYPE" text,
  "ACCESS_REF" integer,
  foreign key(ACCESS_REF) references ACCESS(ACCESS_ID),
  foreign key (PACKAGE_REF) references PACKAGE(PACKAGE_ID) on delete cascade
);
/*
 *
 *  $$$$$$\                                $$\                                 $$\            $$\               
 * $$  __$$\                               \__|                                $$ |           $$ |              
 * $$ /  \__| $$$$$$\   $$$$$$$\  $$$$$$$\ $$\  $$$$$$\  $$$$$$$\         $$$$$$$ | $$$$$$\ $$$$$$\    $$$$$$\  
 * \$$$$$$\  $$  __$$\ $$  _____|$$  _____|$$ |$$  __$$\ $$  __$$\       $$  __$$ | \____$$\\_$$  _|   \____$$\ 
 *  \____$$\ $$$$$$$$ |\$$$$$$\  \$$$$$$\  $$ |$$ /  $$ |$$ |  $$ |      $$ /  $$ | $$$$$$$ | $$ |     $$$$$$$ |
 * $$\   $$ |$$   ____| \____$$\  \____$$\ $$ |$$ |  $$ |$$ |  $$ |      $$ |  $$ |$$  __$$ | $$ |$$\ $$  __$$ |
 * \$$$$$$  |\$$$$$$$\ $$$$$$$  |$$$$$$$  |$$ |\$$$$$$  |$$ |  $$ |      \$$$$$$$ |\$$$$$$$ | \$$$$  |\$$$$$$$ |
 *  \______/  \_______|\_______/ \_______/ \__| \______/ \__|  \__|       \_______| \_______|  \____/  \_______|
 */
/*
 USER table contains a reference to a single "user", which really refers to a given cookie on the 
 browser side. There is no login management here, so this just refers to a unique browser instance.
 */
DROP TABLE IF EXISTS "USER";
CREATE TABLE IF NOT EXISTS "USER" (
  "USER_ID" integer primary key autoincrement,
  "USER_KEY" text,
  "CREATION_TIME" integer,
  UNIQUE(USER_KEY)
);
/*
 SESSION table contains the list of known and remembered sessions.
 In case of electron SESSION_WINID is the window ID for a given
 session.
 */
DROP TABLE IF EXISTS "SESSION";
CREATE TABLE IF NOT EXISTS "SESSION" (
  "SESSION_ID" integer primary key autoincrement,
  "USER_REF" integer,
  "SESSION_KEY" text,
  "CREATION_TIME" integer,
  "DIRTY" integer default 1,
  foreign key (USER_REF) references USER(USER_ID),
  UNIQUE(SESSION_KEY)
);
/*
 SESSION_KEY_VALUE table contains the data points that are simple
 key/value pairs.
 */
DROP TABLE IF EXISTS "SESSION_KEY_VALUE";
CREATE TABLE IF NOT EXISTS "SESSION_KEY_VALUE" (
  "SESSION_REF" integer,
  "KEY" text,
  "VALUE" text,
  foreign key (SESSION_REF) references SESSION(SESSION_ID) on delete cascade,
  UNIQUE(SESSION_REF, KEY)
);
/*
 SESSION_LOG table contains general purpose text log for the session
 */
DROP TABLE IF EXISTS "SESSION_LOG";
CREATE TABLE IF NOT EXISTS "SESSION_LOG" (
  "SESSION_REF" integer,
  "TIMESTAMP" text,
  "LOG" text,
  foreign key (SESSION_REF) references SESSION(SESSION_ID) on delete cascade
);
/*
 SESSION_PACKAGE table is a junction table, listing which packages
 are used for a given session.
 */
DROP TABLE IF EXISTS "SESSION_PACKAGE";
CREATE TABLE IF NOT EXISTS "SESSION_PACKAGE" (
  "SESSION_REF" integer,
  "PACKAGE_REF" integer,
  "REQUIRED" integer default false,
  "ENABLED" integer default true,
  foreign key (SESSION_REF) references SESSION(SESSION_ID) on delete cascade,
  foreign key (PACKAGE_REF) references PACKAGE(PACKAGE_ID) on delete cascade,
  UNIQUE(SESSION_REF, PACKAGE_REF)
);
/*
 ENDPOINT_TYPE contains the bulk of the configuration: clusters, attributes, etc.
 */
DROP TABLE IF EXISTS "ENDPOINT_TYPE";
CREATE TABLE IF NOT EXISTS "ENDPOINT_TYPE" (
  "ENDPOINT_TYPE_ID" integer primary key autoincrement,
  "SESSION_REF" integer,
  "NAME" text,
  "DEVICE_TYPE_REF" integer,
  foreign key (SESSION_REF) references SESSION(SESSION_ID) on delete cascade,
  foreign key(DEVICE_TYPE_REF) references DEVICE_TYPE(DEVICE_TYPE_ID)
);
/*
 ENDPOINT table contains the toplevel configured endpoints.
 */
DROP TABLE IF EXISTS "ENDPOINT";
CREATE TABLE IF NOT EXISTS "ENDPOINT" (
  "ENDPOINT_ID" integer primary key autoincrement,
  "SESSION_REF" integer,
  "ENDPOINT_TYPE_REF" integer,
  "PROFILE" integer,
  "ENDPOINT_IDENTIFIER" integer,
  "NETWORK_IDENTIFIER" integer,
  "DEVICE_IDENTIFIER" integer,
  "DEVICE_VERSION" integer,
  foreign key (SESSION_REF) references SESSION(SESSION_ID) on delete cascade,
  foreign key (ENDPOINT_TYPE_REF) references ENDPOINT_TYPE(ENDPOINT_TYPE_ID) on delete
  set NULL
);
/*
 SESSION_CLUSTER contains the on/off values for cluster.
 SIDE is client or server
 STATE is 1 for ON and 0 for OFF.
 */
DROP TABLE IF EXISTS "ENDPOINT_TYPE_CLUSTER";
CREATE TABLE IF NOT EXISTS "ENDPOINT_TYPE_CLUSTER" (
  "ENDPOINT_TYPE_CLUSTER_ID" integer primary key autoincrement,
  "ENDPOINT_TYPE_REF" integer,
  "CLUSTER_REF" integer,
  "SIDE" text,
  "ENABLED" integer default false,
  foreign key (ENDPOINT_TYPE_REF) references ENDPOINT_TYPE(ENDPOINT_TYPE_ID) on delete cascade,
  foreign key (CLUSTER_REF) references CLUSTER(CLUSTER_ID),
  UNIQUE(ENDPOINT_TYPE_REF, CLUSTER_REF, SIDE)
);
/*
 ENDPOINT_TYPE_ATTRIBUTE table contains the user data configuration for the various parameters that exist
 for an attribute on an endpoint. This essentially lets you determine if something should be included or not.
 */
DROP TABLE IF EXISTS "ENDPOINT_TYPE_ATTRIBUTE";
CREATE TABLE IF NOT EXISTS "ENDPOINT_TYPE_ATTRIBUTE" (
  "ENDPOINT_TYPE_ATTRIBUTE_ID" integer primary key autoincrement,
  "ENDPOINT_TYPE_REF" integer,
  "ENDPOINT_TYPE_CLUSTER_REF" integer,
  "ATTRIBUTE_REF" integer,
  "INCLUDED" integer default false,
  "STORAGE_OPTION" text,
  "SINGLETON" integer default false,
  "BOUNDED" integer default false,
  "DEFAULT_VALUE" text,
  "INCLUDED_REPORTABLE" integer default false,
  "MIN_INTERVAL" integer default 1,
  "MAX_INTERVAL" integer default 65534,
  "REPORTABLE_CHANGE" integer default 0,
  foreign key (ENDPOINT_TYPE_REF) references ENDPOINT_TYPE(ENDPOINT_TYPE_ID) on delete cascade,
  foreign key (ENDPOINT_TYPE_CLUSTER_REF) references ENDPOINT_TYPE_CLUSTER(ENDPOINT_TYPE_CLUSTER_ID),
  foreign key (ATTRIBUTE_REF) references ATTRIBUTE(ATTRIBUTE_ID),
  UNIQUE(
    ENDPOINT_TYPE_REF,
    ATTRIBUTE_REF,
    ENDPOINT_TYPE_CLUSTER_REF
  )
);
/*
 ENDPOINT_TYPE_COMMAND table contains the user data configuration for the various parameters that exist
 for commands on an endpoint. This essentially lets you determine if something should be included or not.
 */
DROP TABLE IF EXISTS "ENDPOINT_TYPE_COMMAND";
CREATE TABLE IF NOT EXISTS "ENDPOINT_TYPE_COMMAND" (
  "ENDPOINT_TYPE_COMMAND_ID" integer primary key autoincrement,
  "ENDPOINT_TYPE_REF" integer,
  "ENDPOINT_TYPE_CLUSTER_REF" integer,
  "COMMAND_REF" integer,
  "INCOMING" integer default false,
  "OUTGOING" integer default false,
  foreign key (ENDPOINT_TYPE_REF) references ENDPOINT_TYPE(ENDPOINT_TYPE_ID) on delete cascade,
  foreign key (ENDPOINT_TYPE_CLUSTER_REF) references ENDPOINT_TYPE_CLUSTER(ENDPOINT_TYPE_CLUSTER_ID),
  foreign key (COMMAND_REF) references COMMAND(COMMAND_ID),
  UNIQUE(
    ENDPOINT_TYPE_REF,
    COMMAND_REF,
    ENDPOINT_TYPE_CLUSTER_REF
  )
);
/*
 ENDPOINT_TYPE_EVENT table contains the user data configuration for the various parameters that exist
 for events on an endpoint. This essentially lets you determine if something should be included or not.
 */
DROP TABLE IF EXISTS "ENDPOINT_TYPE_EVENT";
CREATE TABLE IF NOT EXISTS "ENDPOINT_TYPE_EVENT" (
  "ENDPOINT_TYPE_EVENT_ID" integer primary key autoincrement,
  "ENDPOINT_TYPE_REF" integer,
  "ENDPOINT_TYPE_CLUSTER_REF" integer,
  "EVENT_REF" integer,
  "INCLUDED" integer default false,
  foreign key (ENDPOINT_TYPE_REF) references ENDPOINT_TYPE(ENDPOINT_TYPE_ID) on delete cascade,
  foreign key (ENDPOINT_TYPE_CLUSTER_REF) references ENDPOINT_TYPE_CLUSTER(ENDPOINT_TYPE_CLUSTER_ID),
  foreign key (EVENT_REF) references EVENT(EVENT_ID),
  UNIQUE(
    ENDPOINT_TYPE_REF,
    EVENT_REF,
    ENDPOINT_TYPE_CLUSTER_REF
  )
);
/**
 PACKAGE_EXTENSION_VALUE contains the value of the given package
 extension for a given entity.
 */
DROP TABLE IF EXISTS "PACKAGE_EXTENSION_VALUE";
CREATE TABLE IF NOT EXISTS "PACKAGE_EXTENSION_VALUE" (
  "PACKAGE_EXTENSION_VALUE_ID" integer primary key autoincrement,
  "PACKAGE_EXTENSION_REF" integer,
  "SESSION_REF" integer,
  "ENTITY_CODE" integer,
  "PARENT_CODE" integer,
  "VALUE" text,
  foreign key (PACKAGE_EXTENSION_REF) references PACKAGE_EXTENSION(PACKAGE_EXTENSION_ID) on delete cascade,
  foreign key (SESSION_REF) references SESSION(SESSION_ID),
  UNIQUE(
    PACKAGE_EXTENSION_REF,
    SESSION_REF,
    ENTITY_CODE,
    PARENT_CODE
  )
);
/*
 * 
 * $$$$$$$$\        $$\                                                   
 * \__$$  __|       \__|                                                  
 *    $$ | $$$$$$\  $$\  $$$$$$\   $$$$$$\   $$$$$$\   $$$$$$\   $$$$$$$\ 
 *    $$ |$$  __$$\ $$ |$$  __$$\ $$  __$$\ $$  __$$\ $$  __$$\ $$  _____|
 *    $$ |$$ |  \__|$$ |$$ /  $$ |$$ /  $$ |$$$$$$$$ |$$ |  \__|\$$$$$$\  
 *    $$ |$$ |      $$ |$$ |  $$ |$$ |  $$ |$$   ____|$$ |       \____$$\ 
 *    $$ |$$ |      $$ |\$$$$$$$ |\$$$$$$$ |\$$$$$$$\ $$ |      $$$$$$$  |
 *    \__|\__|      \__| \____$$ | \____$$ | \_______|\__|      \_______/ 
 *                      $$\   $$ |$$\   $$ |                              
 *                      \$$$$$$  |\$$$$$$  |                              
 *                       \______/  \______/                               
 */
CREATE TRIGGER IF NOT EXISTS "INSERT_TRIGGER_SESSION_KEY_VALUE"
AFTER
INSERT ON "SESSION_KEY_VALUE" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = NEW.SESSION_REF;
END;
CREATE TRIGGER IF NOT EXISTS "INSERT_TRIGGER_SESSION_PACKAGE"
AFTER
INSERT ON "SESSION_PACKAGE" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = NEW.SESSION_REF;
END;
CREATE TRIGGER IF NOT EXISTS "UPDATE_TRIGGER_SESSION_KEY_VALUE"
AFTER
UPDATE ON "SESSION_KEY_VALUE" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = NEW.SESSION_REF;
END;
CREATE TRIGGER IF NOT EXISTS "UPDATE_TRIGGER_SESSION_PACKAGE"
AFTER
UPDATE ON "SESSION_PACKAGE" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = NEW.SESSION_REF;
END;
CREATE TRIGGER IF NOT EXISTS "DELETE_TRIGGER_SESSION_KEY_VALUE"
AFTER DELETE ON "SESSION_KEY_VALUE" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = OLD.SESSION_REF;
END;
CREATE TRIGGER IF NOT EXISTS "INSERT_TRIGGER_SESSION_LOG"
AFTER
INSERT ON "SESSION_LOG" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = NEW.SESSION_REF;
END;
CREATE TRIGGER IF NOT EXISTS "UPDATE_TRIGGER_SESSION_LOG"
AFTER
UPDATE ON "SESSION_LOG" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = NEW.SESSION_REF;
END;
CREATE TRIGGER IF NOT EXISTS "DELETE_TRIGGER_SESSION_LOG"
AFTER DELETE ON "SESSION_LOG" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = OLD.SESSION_REF;
END;
CREATE TRIGGER IF NOT EXISTS "INSERT_TRIGGER_ENDPOINT_TYPE"
AFTER
INSERT ON "ENDPOINT_TYPE" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = NEW.SESSION_REF;
END;
CREATE TRIGGER IF NOT EXISTS "UPDATE_TRIGGER_ENDPOINT_TYPE"
AFTER
UPDATE ON "ENDPOINT_TYPE" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = NEW.SESSION_REF;
END;
CREATE TRIGGER IF NOT EXISTS "DELETE_TRIGGER_ENDPOINT_TYPE"
AFTER DELETE ON "ENDPOINT_TYPE" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = OLD.SESSION_REF;
END;
CREATE TRIGGER IF NOT EXISTS "INSERT_TRIGGER_ENDPOINT"
AFTER
INSERT ON "ENDPOINT" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = NEW.SESSION_REF;
END;
CREATE TRIGGER IF NOT EXISTS "UPDATE_TRIGGER_ENDPOINT"
AFTER
UPDATE ON "ENDPOINT" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = NEW.SESSION_REF;
END;
CREATE TRIGGER IF NOT EXISTS "DELETE_TRIGGER_ENDPOINT"
AFTER DELETE ON "ENDPOINT" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = OLD.SESSION_REF;
END;
CREATE TRIGGER IF NOT EXISTS "INSERT_TRIGGER_ENDPOINT_TYPE_CLUSTER"
AFTER
INSERT ON "ENDPOINT_TYPE_CLUSTER" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = (
    SELECT SESSION_REF
    FROM ENDPOINT_TYPE
    WHERE ENDPOINT_TYPE_ID = NEW.ENDPOINT_TYPE_REF
  );
END;
CREATE TRIGGER IF NOT EXISTS "UPDATE_TRIGGER_ENDPOINT_TYPE_CLUSTER"
AFTER
UPDATE ON "ENDPOINT_TYPE_CLUSTER" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = (
    SELECT SESSION_REF
    FROM ENDPOINT_TYPE
    WHERE ENDPOINT_TYPE_ID = NEW.ENDPOINT_TYPE_REF
  );
END;
CREATE TRIGGER IF NOT EXISTS "DELETE_TRIGGER_ENDPOINT_TYPE_CLUSTER"
AFTER DELETE ON "ENDPOINT_TYPE_CLUSTER" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = (
    SELECT SESSION_REF
    FROM ENDPOINT_TYPE
    WHERE ENDPOINT_TYPE_ID = OLD.ENDPOINT_TYPE_REF
  );
END;
CREATE TRIGGER IF NOT EXISTS "INSERT_TRIGGER_ENDPOINT_TYPE_ATTRIBUTE"
AFTER
INSERT ON "ENDPOINT_TYPE_ATTRIBUTE" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = (
    SELECT SESSION_REF
    FROM ENDPOINT_TYPE
    WHERE ENDPOINT_TYPE_ID = NEW.ENDPOINT_TYPE_REF
  );
END;
CREATE TRIGGER IF NOT EXISTS "UPDATE_TRIGGER_ENDPOINT_TYPE_ATTRIBUTE"
AFTER
UPDATE ON "ENDPOINT_TYPE_ATTRIBUTE" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = (
    SELECT SESSION_REF
    FROM ENDPOINT_TYPE
    WHERE ENDPOINT_TYPE_ID = NEW.ENDPOINT_TYPE_REF
  );
END;
CREATE TRIGGER IF NOT EXISTS "DELETE_TRIGGER_ENDPOINT_TYPE_ATTRIBUTE"
AFTER DELETE ON "ENDPOINT_TYPE_ATTRIBUTE" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = (
    SELECT SESSION_REF
    FROM ENDPOINT_TYPE
    WHERE ENDPOINT_TYPE_ID = OLD.ENDPOINT_TYPE_REF
  );
END;
CREATE TRIGGER IF NOT EXISTS "INSERT_TRIGGER_ENDPOINT_TYPE_COMMAND"
AFTER
INSERT ON "ENDPOINT_TYPE_COMMAND" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = (
    SELECT SESSION_REF
    FROM ENDPOINT_TYPE
    WHERE ENDPOINT_TYPE_ID = NEW.ENDPOINT_TYPE_REF
  );
END;
CREATE TRIGGER IF NOT EXISTS "UPDATE_TRIGGER_ENDPOINT_TYPE_COMMAND"
AFTER
UPDATE ON "ENDPOINT_TYPE_COMMAND" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = (
    SELECT SESSION_REF
    FROM ENDPOINT_TYPE
    WHERE ENDPOINT_TYPE_ID = NEW.ENDPOINT_TYPE_REF
  );
END;
CREATE TRIGGER IF NOT EXISTS "DELETE_TRIGGER_ENDPOINT_TYPE_COMMAND"
AFTER DELETE ON "ENDPOINT_TYPE_COMMAND" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = (
    SELECT SESSION_REF
    FROM ENDPOINT_TYPE
    WHERE ENDPOINT_TYPE_ID = OLD.ENDPOINT_TYPE_REF
  );
END;
CREATE TRIGGER IF NOT EXISTS "INSERT_TRIGGER_ENDPOINT_TYPE_EVENT"
AFTER
INSERT ON "ENDPOINT_TYPE_EVENT" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = (
    SELECT SESSION_REF
    FROM ENDPOINT_TYPE
    WHERE ENDPOINT_TYPE_ID = NEW.ENDPOINT_TYPE_REF
  );
END;
CREATE TRIGGER IF NOT EXISTS "UPDATE_TRIGGER_ENDPOINT_TYPE_EVENT"
AFTER
UPDATE ON "ENDPOINT_TYPE_EVENT" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = (
    SELECT SESSION_REF
    FROM ENDPOINT_TYPE
    WHERE ENDPOINT_TYPE_ID = NEW.ENDPOINT_TYPE_REF
  );
END;
CREATE TRIGGER IF NOT EXISTS "DELETE_TRIGGER_ENDPOINT_TYPE_EVENT"
AFTER DELETE ON "ENDPOINT_TYPE_EVENT" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = (
    SELECT SESSION_REF
    FROM ENDPOINT_TYPE
    WHERE ENDPOINT_TYPE_ID = OLD.ENDPOINT_TYPE_REF
  );
END;
CREATE TRIGGER IF NOT EXISTS "INSERT_TRIGGER_PACKAGE_EXTENSION_VALUE"
AFTER
INSERT ON "PACKAGE_EXTENSION_VALUE" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = NEW.SESSION_REF;
END;
CREATE TRIGGER IF NOT EXISTS "UPDATE_TRIGGER_PACKAGE_EXTENSION_VALUE"
AFTER
UPDATE ON "PACKAGE_EXTENSION_VALUE" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = NEW.SESSION_REF;
END;
CREATE TRIGGER IF NOT EXISTS "DELETE_TRIGGER_PACKAGE_EXTENSION_VALUE"
AFTER DELETE ON "PACKAGE_EXTENSION_VALUE" BEGIN
UPDATE SESSION
SET DIRTY = 1
WHERE SESSION_ID = OLD.SESSION_REF;
END;
/*
 * 
 *  $$$$$$\  $$\           $$\                 $$\             $$\            $$\               
 * $$  __$$\ $$ |          $$ |                $$ |            $$ |           $$ |              
 * $$ /  \__|$$ | $$$$$$\  $$$$$$$\   $$$$$$\  $$ |       $$$$$$$ | $$$$$$\ $$$$$$\    $$$$$$\  
 * $$ |$$$$\ $$ |$$  __$$\ $$  __$$\  \____$$\ $$ |      $$  __$$ | \____$$\\_$$  _|   \____$$\ 
 * $$ |\_$$ |$$ |$$ /  $$ |$$ |  $$ | $$$$$$$ |$$ |      $$ /  $$ | $$$$$$$ | $$ |     $$$$$$$ |
 * $$ |  $$ |$$ |$$ |  $$ |$$ |  $$ |$$  __$$ |$$ |      $$ |  $$ |$$  __$$ | $$ |$$\ $$  __$$ |
 * \$$$$$$  |$$ |\$$$$$$  |$$$$$$$  |\$$$$$$$ |$$ |      \$$$$$$$ |\$$$$$$$ | \$$$$  |\$$$$$$$ |
 *  \______/ \__| \______/ \_______/  \_______|\__|       \_______| \_______|  \____/  \_______|
 */
/*
 Random settings, essentially application preferences
 */
CREATE TABLE IF NOT EXISTS "SETTING" (
  "CATEGORY" text,
  "KEY" text,
  "VALUE" text,
  UNIQUE(CATEGORY, KEY)
);
/* EO SCHEMA */
